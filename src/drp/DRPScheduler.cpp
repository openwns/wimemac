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

#include <WIMEMAC/drp/DRPScheduler.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::drp::DRPScheduler,
    wns::ldk::FunctionalUnit,
    "wimemac.drp.DRPScheduler",
    wns::ldk::FUNConfigCreator );

using namespace wimemac::drp;

DRPScheduler::DRPScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ ) :
        config(config_),
        wns::ldk::fu::Plain<DRPScheduler, DRPSchedulerCommand>(fun),
        managerName(config_.get<std::string>("managerName")),
        dcfName(config_.get<std::string>("dcfName")),
        txopName(config_.get<std::string>("txopName")),
        beaconBuilderName(config_.get<std::string>("beaconBuilderName")),
        maxPER(config_.get<double>("maxPER")),
        pcaPortionProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),"wimemac.drpscheduler.pcaPortion")),
        patternPEROffset(config_.get<double>("patternPEROffset")),
        isDroppingAfterRetr(config_.get<int>("isDroppingAfterRetr")),
        perMIBServiceName(config.get<std::string>("perMIBServiceName")),
        logger(config_.get("logger"))


{   //create new queues to store the outgoing compounds
    DRPQueues = new helper::Queues(config, fun);
    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: QueueSize : " << (config_.get<long int>("queuesize")));

    //Don't start a drp outgoing connection yet
    AccessPermission = false;
    isPCAtransmissionActive = false;
    PCAcompoundsWereSent = false;
    //wnsscheduler = wns::simulator::getEventScheduler();

    friends.keyReader = fun->getProxy()->getCommandReader("upperConvergence");
}

DRPScheduler::~DRPScheduler()
{
    for (SendBufferContainer::iterator it = SendBuffer.begin(); it != SendBuffer.end(); it++)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Buffer for target " << it->second->GetTarget() << " had to retransmit numOfPackets : " << it->second->GetNumOfTotalRetransmissions() );
    }
}

int
DRPScheduler::getNumOfRetransmissions(const wns::ldk::CompoundPtr& compound)
{
    wns::service::dll::UnicastAddress target = friends.manager->getReceiverAddress(compound->getCommandPool());
    return PCABuffer[target]->GetNumOfRetransmissions(compound);
}

void
DRPScheduler::doOnData( const wns::ldk::CompoundPtr& compound )
{
    //for incoming compounds nothing should be done
    //     MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Receive Compound");

    // Write pcaPortionProbe
    if(getCommand(compound->getCommandPool())->getTxType() == PCA)
        pcaPortionProbe->put(compound, 1);
    else if(getCommand(compound->getCommandPool())->getTxType() == DRP)
        pcaPortionProbe->put(compound, 0);
    else assure(false,"TxType is not properly set : " << getCommand(compound->getCommandPool())->getTxType());
    getDeliverer()->getAcceptor( compound )->onData( compound );
}

bool
DRPScheduler::doIsAccepting( const wns::ldk::CompoundPtr& compound) const
{
    //MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: doIsAccepting is called");
    return (DRPQueues->isAccepting(compound));

}

void
DRPScheduler::doSendData( const wns::ldk::CompoundPtr& compound )
{
    wns::service::dll::UnicastAddress iam
    = getFUN()->findFriend<dll::UpperConvergence*>("upperConvergence")->getMACAddress();

    wns::service::dll::UnicastAddress target
    = friends.manager->getReceiverAddress(compound->getCommandPool());

    //outgoing compounds should be stored in a queue
    DRPQueues->put(compound);

    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: New compound arrived for target " << target);
    //if a drp transmission has started, inform the intermediate buffer about the new incoming compound
    if((AccessPermission == true) && (AccessRx == target))
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: The target DRP buffer is active -> inform about the new arrival ");
        SendBuffer[AccessRx]->NewArrival();
    }
    else if((AccessPermission == true) && (AccessRx != target))
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: There is already a DRP buffer active for target " << AccessRx);
        return;
    }

    // only try any PCA action if it's enabled and no DRP transmission is currently active
    if(AccessPermission == false && friends.manager->getPCAchannelAccess())
    {
        if((isPCAtransmissionActive) && (ActivePCArx == target))
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: The target PCA buffer is active -> inform about the new arrival ");
            PCABuffer[ActivePCArx]->NewArrival();
        }
        else if((isPCAtransmissionActive) && (ActivePCArx != target))
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: There is already a PCA buffer active for target " << ActivePCArx);
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Try to start a new PCA transmission");
            startPCAtransmission();
        }
    }
}

wns::service::dll::UnicastAddress
DRPScheduler::getCurrentTransmissionTarget()
{
    // DRP Target
    if(AccessPermission == true) return AccessRx;

    // PCA Target
    if(friends.manager->getPCAchannelAccess() && isPCAtransmissionActive) return ActivePCArx;

    // else no Target
    return wns::service::dll::UnicastAddress();
}

void
DRPScheduler::onFUNCreated()
{
    friends.bb
    = getFUN()->findFriend<wimemac::management::BeaconBuilder*>(beaconBuilderName);
    friends.dcf
    = getFUN()->findFriend<wimemac::lowerMAC::timing::DCF*>(dcfName);
    friends.txop
    = getFUN()->findFriend<wimemac::lowerMAC::TXOP*>(txopName);
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);

    // signal packet success/errors to MIB
    perMIB = getFUN()->getLayer<dll::Layer2*>()->getManagementService<wimemac::management::PERInformationBase>(perMIBServiceName);
}

void
DRPScheduler::doWakeup()
{
//     MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Phy finished sending beacon!");
}

bool
DRPScheduler::isBeacon(const wns::ldk::CommandPool* commandPool) const
{
    return friends.bb->isBeacon(commandPool);
}

//TimeToTransmit is invoked by DRPManager. It's a signal to start a drp transmission. First an intermediate buffer is
//created and the buffer starts buffering. If the intermediate buffer is ready, the compounds are sent.
//AccessPermission is set to true in order to inform the buffer about new arrivals in outgoing direction
void
DRPScheduler::TimeToTransmit(wns::service::dll::UnicastAddress macaddress, wns::simulator::Time duration)
{
    wns::service::dll::UnicastAddress iam
    = getFUN()->findFriend<dll::UpperConvergence*>("upperConvergence")->getMACAddress();

    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: TimeToTransmit is called for target " << macaddress
    <<" and I am " << iam
    <<". TXOP duration :" << duration);

    if(SendBuffer.find(macaddress) == SendBuffer.end())
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Create a new buffer for " << macaddress <<" and started buffering ");
        SendBuffer[macaddress] = new TempSendBuffer(DRPQueues,macaddress,this,logger);	
    }
    else
    {
//        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Buffer exists for " << macaddress <<" started buffering ");
//        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler begin TXOP number of compound in tempqueue: "<< SendBuffer[macaddress]->numCompounds());
//        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler begin TXOP number of compound in main queue: "<< DRPQueues->numCompoundsForMacAddress(macaddress));
    }

    AccessPermission = true;
    AccessRx = macaddress;
    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Set Timeout DRP");
    setTimeout(duration);

//     MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Timer exists " << hasTimeoutSet() );

    SendBuffer[macaddress]->SetTxopDuration(duration);
    if(SendBuffer[macaddress]->StartBuffering());

}


void
DRPScheduler::SendCompounds(wns::service::dll::UnicastAddress macaddress)
{
    assure(!((AccessPermission == true) && (isPCAtransmissionActive == true)),"Both DRP and PCA transmissions are active!");
    assure(((AccessPermission == true) or (isPCAtransmissionActive == true)),"SendCompounds called without neither DRP nor PCA transmission active");

    if(AccessPermission)
    {
        assure(macaddress == AccessRx, "SendCompounds() was called for target " << macaddress << ", but AccessRx is " << AccessRx);

        if(SendBuffer[macaddress]->IsBufferEmpty() == false)
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: start sending ");
            wns::ldk::CompoundPtr compound = SendBuffer[macaddress]->GetCompound();

            wns::ldk::CompoundPtr compound2send = compound->copy();
            DRPSchedulerCommand* drpsc = activateCommand(compound2send->getCommandPool());
            drpsc->peer.type = DRP;
            if(!getConnector()->hasAcceptor(compound2send))
            {
                MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Error, can't send compound in a regular DRP reservation!");
            }
            else
            {
                //MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, send compound with DRP channel access");
                SendBuffer[macaddress]->TransmitCompound(compound);
                getConnector()->getAcceptor(compound2send)->sendData(compound2send);
            }
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, Buffer is empty ");
        }
    }
    else if(isPCAtransmissionActive)
    {
        assure(macaddress == ActivePCArx, "SendCompounds() was called for target " << macaddress << ", but ActivePCArx is " << ActivePCArx);

        if(PCABuffer[macaddress]->IsBufferEmpty() == false)
        {
            wns::ldk::CompoundPtr compound = PCABuffer[macaddress]->GetCompound();

            wns::ldk::CompoundPtr compound2send = compound->copy();
            DRPSchedulerCommand* drpsc = activateCommand(compound2send->getCommandPool());
            drpsc->peer.type = PCA;

            if(!getConnector()->hasAcceptor(compound2send))
            {
                MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Can't begin or send any more compounds in this PCA transmission!");
                stopPCAtransmission();
            }
            else
            {
                PCABuffer[macaddress]->TransmitCompound(compound);
                getConnector()->getAcceptor(compound2send)->sendData(compound2send);
                PCAcompoundsWereSent = true;
            }
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, PCA Buffer is empty ");
        }
    }

 }

wimemac::convergence::PhyMode
DRPScheduler::getPhyMode(wns::service::dll::UnicastAddress rx, int masNumber)
{
    return friends.bb->getPhyMode(rx, masNumber);
}

//An ACK arrives, send next	
void
DRPScheduler::Acknowledgment(wns::service::dll::UnicastAddress rx)
{
    // Inform MIB about succesfull transmission
    if(AccessPermission)
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, DRP compound is acknowledged, call ImmACK for target " << rx);
        // Only use compounds sent in a DRP transmission for PER evaluation
        perMIB->onSuccessfullTransmission(rx);
        SendBuffer[rx]->ImmAck();
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, PCA compound is acknowledged, call ImmACK for target " << rx);
        // If AccessPermission is false then the ACK belongs to a PCA transmission
        PCABuffer[rx]->ImmAck();
    }

}

void
DRPScheduler::failedAck(wns::service::dll::UnicastAddress rx)
{
    // Inform MIB about unsuccesfull transmission
    if(AccessPermission)
    {
        // Only use compounds sent in a DRP transmission for PER evaluation
        perMIB->onFailedTransmission(rx);
    }
    
    if(isPCAtransmissionActive)
    {
        // Close TxOP in case a compound is not acknowledged!
        stopPCAtransmission();
        friends.txop->closeTXOP(false);
    }
}

bool
DRPScheduler::adjustMCSdown(wns::service::dll::UnicastAddress rx)
{
    if (perMIB->knowsPER(rx))
    {
        if (perMIB->getPER(rx) > friends.manager->getDesiredPER())
        {
            MESSAGE_SINGLE(NORMAL, logger, "PER Evaluation: The PER for address " << rx << " is ABOVE the threshold of " << friends.manager->getDesiredPER() << ". It is : " << perMIB->getPER(rx));
            perMIB->reset(rx);
            return true;
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "PER Evaluation: The PER for address " << rx << " is below the threshold of " << friends.manager->getDesiredPER() << ". It is : " << perMIB->getPER(rx));
            return false;
        }
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "PER Evaluation: There is no PER estimate yet for address : " << rx);
        return false;
    }
}

bool
DRPScheduler::UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap)
{
    return friends.bb->UpdateMapWithPeerAvailabilityMap(rx, DRPMap);
}

void
DRPScheduler::RequestIE(wns::service::dll::UnicastAddress rx, wimemac::management::BeaconCommand::ProbeElementID elementID)
{
    friends.bb->RequestIE(rx, elementID);
}

//DRP transmission time is over, stop sending and buffering
void
DRPScheduler::onTimeout()
{
    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: On Timeout ");
    if(AccessPermission)
    {
        // DRP transmission is over
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, Time is over, stop DRP transmission to " << AccessRx);
        SendBuffer[AccessRx]->StopBuffering();
        AccessPermission = false;
        //DRPQueues->RemoveCompounds(AccessRx);

        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler number of compounds in tempqueue: "<< SendBuffer[AccessRx]->numCompounds());
        //  MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler number of compound in main queue: "<< DRPQueues->numCompoundsForMacAddress(AccessRx));
    }
    if(isPCAtransmissionActive)
    {
        // PCA transmission is over
        stopPCAtransmission();
    }
}

wns::simulator::Time
DRPScheduler::getNextTransmissionDuration()
{
    assure(PCABuffer.find(ActivePCArx) != PCABuffer.end(), "There is no TempBuffer for the current PCAReceiver" << ActivePCArx);

    Bit currentCompoundSize_ = PCABuffer[ActivePCArx]->getCurrentCompoundSize();
    Bit nextCompoundSize_ = PCABuffer[ActivePCArx]->getNextCompoundSize();

    wns::simulator::Time currentduration = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU(currentCompoundSize_,
        getPhyMode(ActivePCArx,friends.manager->getMASNumber(wns::simulator::getEventScheduler()->getTime())));
    wns::simulator::Time nextduration = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU(nextCompoundSize_,
        getPhyMode(ActivePCArx,friends.manager->getMASNumber(wns::simulator::getEventScheduler()->getTime())));

    // Check if the sending of the next compound does not interfere with a scheduled DRP reservation
    //      This Compound + SIFS + ACK + SIFS + Next Compound + SIFS + ACK + SIFS + Guard < NextDRPReservation
    if((nextCompoundSize_ > 0)
        && (wns::simulator::getEventScheduler()->getTime() + currentduration + 10E-6 + 13.125E-6 + 10E-6 + nextduration + 10E-6 + 13.125E-6 + 10E-6 < nextDRPReservationTime - 12E-6))
        return nextduration;
    else return 0;
}

wns::service::dll::UnicastAddress
DRPScheduler::getNextReceiver() const
{
    return DRPQueues->getNextPCAReceiver();
}

bool
DRPScheduler::startPCAtransmission()
{
    assure(AccessPermission == false, "A PCA transmission was started although a DRP transmission is already active");
  
    bool successfulStart = false;

    // Transmission already started
    if(hasTimeoutSet()) return false;

    wns::simulator::Time timeUntilNextDRPReservation = getTimeUntilNextDRPReservation();

    if(timeUntilNextDRPReservation > 0)
    {
        ActivePCArx = getNextReceiver();
        if(ActivePCArx != wns::service::dll::UnicastAddress())
        {
            isPCAtransmissionActive = true;

            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Beginning a PCA transmission for target " << ActivePCArx << " with maximum duration of " << timeUntilNextDRPReservation);

            if(PCABuffer.find(ActivePCArx) == PCABuffer.end())
            {
                MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Create a new PCA buffer for " << ActivePCArx <<" and started buffering ");
                PCABuffer[ActivePCArx] = new TempSendBuffer(DRPQueues,ActivePCArx,this,logger);
            }

            //Bit bits = DRPQueues->getHeadOfLinePDUbits(nextPCAReceiver);
            wns::simulator::Time txOPDuration = timeUntilNextDRPReservation;

            MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Set Timeout PCA | TxOpDuration " << txOPDuration);
            setTimeout(txOPDuration - 1E-9);

            PCABuffer[ActivePCArx]->SetTxopDuration(txOPDuration);
            if(PCABuffer[ActivePCArx]->StartBuffering())
                successfulStart = true;

        }
        else MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: There are no compounds for any target!");
     }
     else MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: There is not enough time until the next DRP reservation starts!");

     return successfulStart;
}

wns::simulator::Time
DRPScheduler::getTimeUntilNextDRPReservation()
{
    wns::simulator::Time timeUntilNextDRPReservation;
    wns::simulator::Time bpStartTime = friends.manager->getBPStartTime();
    int nextDRPMAS = 256;

    int thisMAS = friends.manager->getMASNumber(wns::simulator::getEventScheduler()->getTime());
    for(int i = thisMAS; i < currentDRPMap.size(); i++)
    {
        if(currentDRPMap[i])
        {
            nextDRPMAS = i;
            break;
        }
    }

    // Calculate time until the next MAS occupied by a DRP reservation begins
    timeUntilNextDRPReservation = bpStartTime + nextDRPMAS*256E-6 - wns::simulator::getEventScheduler()->getTime();

    nextDRPReservationTime = wns::simulator::getEventScheduler()->getTime() + timeUntilNextDRPReservation;

    if(timeUntilNextDRPReservation < 30E-6) return 0;
    else if(timeUntilNextDRPReservation > 2*1024E-6) return 2*1024E-6;
    else return timeUntilNextDRPReservation;
}

void
DRPScheduler::UpdateDRPMap(Vector DRPMap)
{
    currentDRPMap = DRPMap;
    currentRegisterDRPMap = DRPMap;

    // TODO In Case of Soft-DRP this needs to be adjusted

    // ThisMAS is the first MAS after the BP
    int thisMAS = friends.manager->getMASNumber(wns::simulator::getEventScheduler()->getTime());
    RegisterDRPReservations(thisMAS);
}

void
DRPScheduler::RegisterDRPReservations(int thisMAS)
{
    int adjacent = 0;
    double MASduration = 256E-6;
    int i = thisMAS;
    wns::simulator::Time ReservationStart;

    while(i < currentRegisterDRPMap.size())
    {

        if(currentRegisterDRPMap[i] == true)
        {
            if(adjacent == 0)
            {
                ReservationStart = (i - thisMAS) * MASduration;
            }

            currentRegisterDRPMap[i] = false;
            adjacent++;
        }
        else
        {
            if(adjacent != 0)
            {
                break;
            }
        }
        i++;
    }

    if(adjacent != 0)
    {
        wns::simulator::Time duration = adjacent * MASduration - 1E-12;
        adjacent = 0;

        wns::simulator::getEventScheduler()->scheduleDelay(
            boost::bind(&DRPScheduler::onDRPStart, this),
            ReservationStart);

        wns::simulator::getEventScheduler()->scheduleDelay(
            boost::bind(&DRPScheduler::onDRPStop, this),
            ReservationStart + duration);
    }

    if(i < currentRegisterDRPMap.size())
        RegisterDRPReservations(thisMAS);

}

void
DRPScheduler::onBPStart(wns::simulator::Time BPduration)
{
    wns::simulator::Time nextBPStart = friends.manager->getBPStartTime() + 256*256E-6;

    wns::simulator::getEventScheduler()->schedule(
            boost::bind(&DRPScheduler::onDRPStart, this),
            nextBPStart);

    wns::simulator::getEventScheduler()->schedule(
            boost::bind(&DRPScheduler::onDRPStop, this),
            nextBPStart + BPduration - 1E-12);
}

void
DRPScheduler::stopPCAtransmission()
{
    if(!friends.manager->getPCAchannelAccess()) return;

    if(!isPCAtransmissionActive)
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: PCA transmission is already stopped");
        return;
    }

    //doWakeup();
    PCABuffer[ActivePCArx]->StopBuffering();

    if(hasTimeoutSet())
    {
        cancelTimeout();
    }

    if(PCAcompoundsWereSent)
    {
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: StopPCATransmission after compounds were sent");
        DRPQueues->changePCAreceiver();
    }
    PCAcompoundsWereSent = false;
    isPCAtransmissionActive = false;

    if(DRPQueues->queueHasPDUs(ActivePCArx))
    {
        // Inform DCF about waiting transmissions
        MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Inform DCF about waiting transmissions with NumOfTransmissions : " << PCABuffer[ActivePCArx]->GetNumOfRetransmissions() +1);
        friends.dcf->waitingTransmissions(PCABuffer[ActivePCArx]->GetNumOfRetransmissions() +1);
    }

}

void
DRPScheduler::txOPCloseIn(wns::simulator::Time duration)
{
    MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Close TxOP in " << duration - 1E-6);
    wns::simulator::getEventScheduler()->scheduleDelay(
            boost::bind(&DRPScheduler::stopPCAtransmission, this),
            duration - 1E-6);
}

void
DRPScheduler::onDRPStart()
{
    friends.dcf->onDRPStart();
}

void
DRPScheduler::onDRPStop()
{
    friends.dcf->onDRPStop();
}
