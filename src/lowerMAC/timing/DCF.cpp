/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WIMEMAC/lowerMAC/timing/DCF.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/arq/ARQ.hpp>
#include <WNS/service/phy/ofdma/Notification.hpp>
#include <WNS/container/UntypedRegistry.hpp>

#include <DLL/UpperConvergence.hpp>

using namespace wimemac::lowerMAC::timing;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::lowerMAC::timing::DCF,
    wns::ldk::FunctionalUnit,
    "wimemac.lowerMAC.timing.DCF",
    wns::ldk::FUNConfigCreator);

DCF::DCF(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_) :
    wns::ldk::fu::Plain<DCF, wns::ldk::EmptyCommand>(fun),
    csName(config_.get<std::string>("csName")),
    rxStartEndName(config_.get<std::string>("rxStartEndName")),
    drpSchedulerName(config_.get<std::string>("drpSchedulerName")),
    backoffDisabled(config_.get<bool>("myConfig.backoffDisabled")),
    backoff(this, config_),
    sendNow(false),
    logger(config_.get("logger"))
{
} // DCF::DCF


DCF::~DCF()
{
} // DCF::~DCF

void DCF::onFUNCreated()
{
    friends.drpScheduler = getFUN()->findFriend<wimemac::drp::DRPScheduler*>(drpSchedulerName);

    if(not backoffDisabled)
    {
        // backoff observes the channel state
        backoff.wns::Observer<wimemac::convergence::IChannelState>::startObserving
            (getFUN()->findFriend<wimemac::convergence::ChannelStateNotification*>(csName));

        // backoff gets notified of failed receptions
        backoff.wns::Observer<wimemac::convergence::IRxStartEnd>::startObserving
            (getFUN()->findFriend<wimemac::convergence::RxStartEndNotification*>(rxStartEndName));
    }
} // DCF::onFUNCreated

void
DCF::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(sendNow,
           "called doSendData, but sendNow is false");
    sendNow = false;

    assure(getConnector()->hasAcceptor(compound),
           "lower FU is not accepting");

    getConnector()->getAcceptor(compound)->sendData(compound);

}

void
DCF::doOnData(const wns::ldk::CompoundPtr& compound)
{
    // simply forward to the upper FU
    getDeliverer()->getAcceptor(compound)->onData(compound);
}

bool
DCF::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    assure(not backoffDisabled,
           "Backoff was disabled, hence no frames can be accepted");

    if(sendNow and getConnector()->hasAcceptor(compound))
    {
        MESSAGE_SINGLE(NORMAL, logger, "backoff asked, sendNow is " << sendNow);
        return true;
    }


    int numTransmissions = friends.drpScheduler->getNumOfRetransmissions(compound) + 1; // numTransmissions begins with 1
    if(numTransmissions > 1)
    {
        // It was already tried to send this compound
        MESSAGE_SINGLE(NORMAL, logger, "Compound retransmission, transmission number " << numTransmissions);
        sendNow = backoff.transmissionRequest(numTransmissions);
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "Compound's first transmission");
        sendNow = backoff.transmissionRequest(1);
    }

    MESSAGE_SINGLE(NORMAL, logger, "backoff asked, sendNow is " << sendNow);

    return(sendNow and getConnector()->hasAcceptor(compound));
}

void
DCF::waitingTransmissions(int numTransmissions)
{
    bool directGo = backoff.transmissionRequest(numTransmissions);
}

void
DCF::doWakeup()
{
    getReceptor()->wakeup();
}

void
DCF::onDRPStart()
{
    backoff.OnDRPreservationChange(true);
}

void
DCF::onDRPStop()
{
    backoff.OnDRPreservationChange(false);
}

void DCF::backoffExpired()
{
    MESSAGE_SINGLE(NORMAL, logger, "Backoff expired, startPCAtransmission");
    this->sendNow = true;

    // Trigger start of a PCA transmission
    if(!friends.drpScheduler->startPCAtransmission())
    {
        MESSAGE_SINGLE(NORMAL, logger, "PCA transmission could not be started immediately -> reset sendNow");
        this->sendNow = false;
    }
}
