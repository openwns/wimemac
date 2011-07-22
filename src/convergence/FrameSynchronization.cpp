/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2011
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WIMEMAC/convergence/FrameSynchronization.hpp>
#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WIMEMAC/convergence/PreambleGenerator.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/crc/CRC.hpp>
#include <WNS/probe/bus/utils.hpp>

#include <DLL/Layer2.hpp>

using namespace wimemac::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    FrameSynchronization,
    wns::ldk::probe::Probe,
    "wimemac.convergence.FrameSynchronization",
    wns::ldk::FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    FrameSynchronization,
    wns::ldk::FunctionalUnit,
    "wimemac.convergence.FrameSynchronization",
    wns::ldk::FUNConfigCreator);

FrameSynchronization::FrameSynchronization(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config):
    wns::ldk::fu::Plain<FrameSynchronization, FrameSynchronizationCommand>(fun),
    wns::ldk::probe::Probe(),

    logger(config.get("logger")),
    curState(Idle),
    synchronizedToAddress(),
    slcCapture(config.get<wns::Ratio>("myConfig.slcCapture")),
    slgCapture(config.get<wns::Ratio>("myConfig.slgCapture")),
    idleCapture(config.get<wns::Ratio>("myConfig.idleCapture")),
    detectionThreshold(config.get<wns::Ratio>("myConfig.detectionThreshold")),
    signalRxErrorAlthoughNotSynchronized(config.get<bool>("myConfig.signalRxErrorAlthoughNotSynchronized")),
    lastFrameEnd(0),
    managerName(config.get<std::string>("managerName")),
    crcCommandName(config.get<std::string>("crcCommandName"))
{
    // read the localIDs from the config
    wns::probe::bus::ContextProviderCollection localContext(&fun->getLayer()->getContextProviderCollection());
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        localContext.addProvider(wns::probe::bus::contextprovider::Constant(key, value));
        MESSAGE_SINGLE(VERBOSE, logger, "Using Local IDName '"<<key<<"' with value: "<<value);
    }

}

FrameSynchronization::~FrameSynchronization()
{

}

void FrameSynchronization::doSendData(const wns::ldk::CompoundPtr& compound)
{
    // Stop any synchronization
    switch(curState)
    {
    case Idle:
        break;
    case Synchronized:
        // signal end of frame to each observer
        MESSAGE_SINGLE(NORMAL, logger, "Send data during decoding -> signal rx error");
        this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(0, false, true));
        // Fall through to set state to Idle
    case waitForFinalDelivery:
    case Garbled:
        MESSAGE_SINGLE(NORMAL, logger, "Send data during reception -> reset curState to idle");
        lastFrameEnd = wns::simulator::getEventScheduler()->getTime();
        if(hasTimeoutSet())
            cancelTimeout();
        curState = Idle;
        synchronizedToAddress = wns::service::dll::UnicastAddress();
        break;
    default:
        assure(false, "Unknown state");
    }

    // pass through
    getConnector()->getAcceptor(compound)->sendData(compound);
}

void FrameSynchronization::doOnData(const wns::ldk::CompoundPtr& compound)
{
    if(friends.manager->isPreamble(compound->getCommandPool()))
    {
        this->processPreamble(compound);
    }
    else
    {
        this->processPSDU(compound);
    }
}

void FrameSynchronization::processPreamble(const wns::ldk::CompoundPtr& compound)
{
    assure(friends.manager->isPreamble(compound->getCommandPool()),
           "called processPreamble for non-preamble");

    bool crcOk = getFUN()->getCommandReader(crcCommandName)->readCommand<wimemac::convergence::ErrorModellingCommand>(compound->getCommandPool())->local.checkOK;

    wns::simulator::Time fDur = friends.manager->getpsduDuration(compound->getCommandPool());
    if(!friends.manager->hasPayload(compound->getCommandPool())) fDur = 0;

    wns::Ratio sinr = getFUN()->getCommandReader(crcCommandName)->readCommand<wimemac::convergence::ErrorModellingCommand>(compound->getCommandPool())->local.sinr;

    if(sinr < detectionThreshold)
    {
        MESSAGE_SINGLE(NORMAL, logger, "ProcessPreamble(idle), SINR= " << sinr << " below detection threshold -> DROP");
        return;
    }

    wns::Ratio captureThreshold;
    switch(curState)
    {
    case Idle:
        MESSAGE_SINGLE(NORMAL, logger, "ProcessPreamble(idle), SINR= " << sinr << " CRC " << crcOk);
        assure(lastFrameEnd <= wns::simulator::getEventScheduler()->getTime(), "State is idle, but last frame has not finished");
        assure(!this->hasTimeoutSet(), "State is idle, but timeout is set");
        captureThreshold = idleCapture;
        break;

    case Synchronized:
        MESSAGE_SINGLE(NORMAL, logger, "processPreamble(synchronized), SINR= " << sinr << " CRC " << crcOk);
        assure(lastFrameEnd >= wns::simulator::getEventScheduler()->getTime(), "State is synchronized, but lastFrameEnd is over");
        assure(this->hasTimeoutSet(), "State is decoding, but no timeout set");
        captureThreshold = slcCapture;
        break;

    case waitForFinalDelivery:
        MESSAGE_SINGLE(NORMAL, logger, "processPreamble(waitForFinalDelivery), SINR= " << sinr << " CRC " << crcOk);
        assure(this->hasTimeoutSet(), "State is waitForFinalDelivery, but no timeout set");
        assure(lastFrameEnd+10e-9 >= wns::simulator::getEventScheduler()->getTime(), "State is waitForFinalDelivery, but lastFrameEnd is over");
        captureThreshold = slcCapture;
        break;

    case Garbled:
        MESSAGE_SINGLE(NORMAL, logger, "processPreamble(garbled), SINR= " << sinr << " CRC " << crcOk);
        assure(this->hasTimeoutSet(), "State is garbled, but no timeout set");
        assure(lastFrameEnd >= wns::simulator::getEventScheduler()->getTime(), "State is garbled, but lastFrameEnd is over");
        //if the preamble and the header are decoded well and the crc check is passed, a threshold of slgCapture prevents the station from 
        //synchronizing to the communication partner in case of a DRP connection
        if(friends.manager-> getDRPchannelAccess() == true )
        {
          if(crcOk == true) captureThreshold = idleCapture;
        }
        else
        {         
          captureThreshold = slgCapture;
        }
        break;

    default:
        assure(false, "Unknown state");
    }

    if (sinr > captureThreshold)
    {
        // capture has taken place
        if(curState == Synchronized)
        {
            // signal end of frame to each observer
            MESSAGE_SINGLE(NORMAL, logger, "ProcessPreamble -> Capture current decoding, signal rx end");
            this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(0, false, true));
        }

        if(crcOk)
        {
//             if(friends.manager->hasPayload(compound->getCommandPool()))
//             {
                this->syncToNewPreamble(fDur, friends.manager->getTransmitterAddress(compound->getCommandPool()));
//             }
//             else
//             {
//                 if((wns::simulator::getEventScheduler()->getTime()) > lastFrameEnd)
//                 {
//                     // only change lastFrameEnd if the new frame is longer than the current one
//                     lastFrameEnd = wns::simulator::getEventScheduler()->getTime();
//                     MESSAGE_SINGLE(NORMAL, logger, "Rx preamble without payload, sync ended " << lastFrameEnd);
//                 }
//                 else
//                 {
//                     MESSAGE_SINGLE(NORMAL, logger, "Rx preamble without payload, sync ended " << wns::simulator::getEventScheduler()->getTime() << ", afterwards garbled until " << lastFrameEnd);
//                 }
//             }

            // deliver preamble
            getDeliverer()->getAcceptor(compound)->onData(compound);
        }
        else
        {
            this->failedSyncToNewPreamble(fDur);
        }
    }
    else
    {
        if ((curState == Synchronized) or (curState == waitForFinalDelivery))
        {
            // special handling if the state is synchronized:
            // Wait for the end of the current frame, and then change into the garbled state if neccessary
            if(wns::simulator::getEventScheduler()->getTime() + fDur > lastFrameEnd)
            {
                lastFrameEnd = wns::simulator::getEventScheduler()->getTime() + fDur;
                MESSAGE_SINGLE(NORMAL, logger, "Rx preamble while synchronized, but no capture, decode frame and then garbled until " << lastFrameEnd);
            }
            else
            {
                MESSAGE_SINGLE(NORMAL, logger, "Rx preamble while synchronized, no capture, frame is shorter than current one");
            }
        }
        else
        {
            this->failedSyncToNewPreamble(fDur);
        }
    }
}

void FrameSynchronization::failedSyncToNewPreamble(wns::simulator::Time fDur)
{
    if((wns::simulator::getEventScheduler()->getTime() + fDur) >= lastFrameEnd)
    {
        lastFrameEnd = wns::simulator::getEventScheduler()->getTime() + fDur;
        this->setNewTimeout(fDur);
        MESSAGE_SINGLE(NORMAL, logger, "Rx preamble, but no sync possible -> garbled until " << lastFrameEnd);
    }
    else
    {
        assure(this->hasTimeoutSet(), "lastFrameEnd is not over, but no timeout set");
        MESSAGE_SINGLE(NORMAL, logger, "Rx another preamble, but no sync possible -> garbled for " << fDur << " and afterwards until " << lastFrameEnd);
    }
    this->synchronizedToAddress = wns::service::dll::UnicastAddress();
    curState = Garbled;
}

void FrameSynchronization::syncToNewPreamble(const wns::simulator::Time fDur, const wns::service::dll::UnicastAddress transmitter)
{
    //MESSAGE_SINGLE(NORMAL, logger, "Rx preamble, signal rxStart");
    //assure(fDur > 0, "Preamble must have duration larger than zero");
    if (fDur > 0)
    {
        this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(fDur, true, false));

        this->setNewTimeout(fDur);
        if((wns::simulator::getEventScheduler()->getTime() + fDur) > lastFrameEnd)
        {
            // only change lastFrameEnd if the new frame is longer than the current one
            lastFrameEnd = wns::simulator::getEventScheduler()->getTime() + fDur;
            MESSAGE_SINGLE(NORMAL, logger, "Rx preamble, synced until " << lastFrameEnd);
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "Rx preamble, synced for " << fDur << ", afterwards garbled until " << lastFrameEnd);
        }
        this->synchronizedToAddress = transmitter;
        curState = Synchronized;
    }
    else MESSAGE_SINGLE(NORMAL, logger, "Rx preamble without payload, rx already ended");
}

void FrameSynchronization::onTimeout()
{
    if(curState == Synchronized)
    {
        // finished reception
        MESSAGE_SINGLE(NORMAL, logger, "End of decoding, signal rxEnd");
        this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(0, false, false));
        curState = waitForFinalDelivery;
        setTimeout(10e-9);
        return;
    }

    // Frame reception as indicated by the preamble is over, stop synchronization
    if(lastFrameEnd > wns::simulator::getEventScheduler()->getTime())
    {
        // there are still active transmissions, but the preamble was missed
        curState = Garbled;
        this->setTimeout(lastFrameEnd-wns::simulator::getEventScheduler()->getTime());
        MESSAGE_SINGLE(NORMAL, logger, "End of frame, change state to garbled until " << lastFrameEnd);
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "End of frame, change state to idle");
        curState = Idle;
    }
    this->synchronizedToAddress = wns::service::dll::UnicastAddress();
}

void FrameSynchronization::processPSDU(const wns::ldk::CompoundPtr& compound)
{
    assure(!friends.manager->isPreamble(compound->getCommandPool()),
           "called processPSDU for non-psdu");

    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "Received psdu. Synchronized: " << (curState == Synchronized or curState == waitForFinalDelivery);
    m << "; Transmitter ok: " << (friends.manager->getTransmitterAddress(compound->getCommandPool()) == this->synchronizedToAddress);
    m << "; CRC ok: " << (getFUN()->getCommandReader(crcCommandName)->
                          readCommand<wimemac::convergence::ErrorModellingCommand>(compound->getCommandPool())->
                          local.checkOK);
    MESSAGE_END();

    wns::Ratio sinr = getFUN()->getCommandReader(crcCommandName)->readCommand<wimemac::convergence::ErrorModellingCommand>(compound->getCommandPool())->local.sinr;

    if((curState == Synchronized or curState == waitForFinalDelivery) and
       (friends.manager->getTransmitterAddress(compound->getCommandPool()) == this->synchronizedToAddress))
    {
        if(getFUN()->getCommandReader(crcCommandName)->readCommand<wimemac::convergence::ErrorModellingCommand>(compound->getCommandPool())->local.checkOK)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Received matching psdu for current synchronization");
            getDeliverer()->getAcceptor(compound)->onData(compound);
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "Received (synchronized) psdu, but CRC error -> DROP");
            // Signal rxError event
            this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(0, false, true));
        }
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "Received psdu, but not synchronized -> DROP");
        // signal rxError only in case the user specifically asked for this kind
        // of strange behavior (i.e. not synchronized --> CRC cannot be checked
        // at all!
        if(this->signalRxErrorAlthoughNotSynchronized)
        {
            this->wns::Subject<IRxStartEnd>::forEachObserver(OnRxStartEnd(0, false, true));
        }
    }

    // delivery occured
    if((curState == waitForFinalDelivery) and (friends.manager->getTransmitterAddress(compound->getCommandPool()) == this->synchronizedToAddress))
    {
        assure(hasTimeoutSet(), "State is waitForFinalDelivery, but no timeout set");
        cancelTimeout();
        onTimeout();
    }
}

void FrameSynchronization::onFUNCreated()
{
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::IManagerServices*>(managerName);
}

bool FrameSynchronization::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    return getConnector()->hasAcceptor(compound);
}

void FrameSynchronization::doWakeup()
{
    getReceptor()->wakeup();
}
