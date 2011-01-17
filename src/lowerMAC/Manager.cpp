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

#include <WIMEMAC/lowerMAC/Manager.hpp>
#include <WIMEMAC/convergence/ChannelState.hpp>
#include <WIMEMAC/Component.hpp>
#include <WNS/service/dll/StationTypes.hpp>
#include <WNS/ldk/FlowGate.hpp>



using namespace wimemac::lowerMAC;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::lowerMAC::Manager,
    wns::ldk::FunctionalUnit,
    "wimemac.lowerMAC.Manager",
    wns::ldk::FUNConfigCreator);

Manager::Manager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    wns::ldk::fu::Plain<Manager, ManagerCommand>(fun),
    config_(config),
    logger_(config_.get("logger")),
    expectedACKDuration(config.get<wns::simulator::Time>("myConfig.expectedACKDuration")),
    sifsDuration(config.get<wns::simulator::Time>("myConfig.sifsDuration")),
    myMACAddress_(config.get<wns::service::dll::UnicastAddress>("myMACAddress")),
    ucName_(config_.get<std::string>("upperConvergenceCommandName")),
    protocolCalculatorName(config_.get<std::string>("protocolCalculatorName")),
    mcsProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),"wimemac.manager.mcs")),
    reservationBlocks(config.get<int>("myConfig.reservationBlocks")),
    useRandomPattern(config_.get<bool>("myConfig.useRandomPattern") ),
    useRateAdaptation(config_.get<bool>("myConfig.useRateAdaptation")),
    useDRPchannelAccess(config_.get<bool>("myConfig.useDRPchannelAccess")),
    usePCAchannelAccess(config_.get<bool>("myConfig.usePCAchannelAccess")),
    msduLifetimeLimit(config_.get<wns::simulator::Time>("myConfig.msduLifetimeLimit"))
    

{
    MESSAGE_SINGLE(NORMAL, logger_, "created");
    friends.phyUser = NULL;
    friends.upperConvergence = NULL;
    friends.drpScheduler = NULL;

}

Manager::~Manager()
{

}

void
Manager::onFUNCreated()
{
    // set services in my PhyUser to communicate with lower layer
    friends.phyUser = getFUN()->findFriend<wimemac::convergence::PhyUser*>(config_.get<std::string>("phyUserName"));

    friends.phyUser->setDataTransmissionService(
        this->getFUN()->getLayer<wimemac::Component*>()
        ->getService<wns::service::phy::ofdma::DataTransmission*>( config_.get<std::string>("phyDataTransmission") ) );
    friends.phyUser->setNotificationService(
        this->getFUN()->getLayer<wimemac::Component*>()
        ->getService<wns::service::phy::ofdma::Notification*>( config_.get<std::string>("phyNotification") ) );

    friends.drpScheduler = getFUN()->findFriend<wimemac::drp::DRPScheduler*>(config_.get<std::string>("drpSchedulerName"));

    friends.errorModelling = getFUN()->findFriend<wimemac::convergence::ErrorModelling*>(config_.get<std::string>("errorModellingName"));

    protocolCalculator = getFUN()->getLayer<dll::Layer2*>()->getManagementService<wimemac::management::ProtocolCalculator>(protocolCalculatorName);

    // set service in ChannelState to observe the CarrierSense
    wimemac::convergence::ChannelState* cs = getFUN()->findFriend<wimemac::convergence::ChannelState*>(config_.get<std::string>("channelStateName"));
    assure(cs, "Could not get ChannelState from my FUN");

    cs->setCarrierSensingService(
        this->getFUN()->getLayer<dll::Layer2*>()
        ->getService<wns::service::phy::ofdma::Notification*>(config_.get<std::string>("phyCarrierSense")) );

    // find upper convergence
    friends.upperConvergence = getFUN()->findFriend<dll::UpperConvergence*>(ucName_);

}


void
Manager::processIncoming(const wns::ldk::CompoundPtr& /*compound*/)
{

}

void
Manager::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
    assure(getFUN()->getCommandReader(ucName_)
           ->readCommand<dll::UpperCommand>(compound->getCommandPool())
           ->peer.sourceMACAddress == myMACAddress_,
           "Try to tx compound with source address " <<
           getFUN()->getCommandReader(ucName_)
           ->readCommand<dll::UpperCommand>(compound->getCommandPool())
           ->peer.sourceMACAddress <<
           " from transceiver with MAC address " << myMACAddress_ );

    // The command has to be activated to be considered in the createReply chain
    ManagerCommand* mc = activateCommand(compound->getCommandPool());
    mc->peer.CompoundType = DATA;
    mc->peer.hasPayload = true;
    mc->peer.frameExchangeDuration = this->sifsDuration + this->expectedACKDuration;

    // Sets the phymode according to the configuration of the drpscheduler
    wns::service::dll::UnicastAddress rx = getFUN()->getCommandReader(ucName_)
           ->readCommand<dll::UpperCommand>(compound->getCommandPool())
           ->peer.targetMACAddress;
    int masNumber_ = getMASNumber(wns::simulator::getEventScheduler()->getTime());
    mc->peer.phyMode = friends.drpScheduler->getPhyMode(rx, masNumber_);
    mc->peer.psduDuration = protocolCalculator->getDuration()->MSDU_PSDU(compound->getLengthInBits(), mc->peer.phyMode);

    mcsProbe->put(mc->peer.phyMode.getDataRate() );

    if(this->msduLifetimeLimit > 0)
    {
        mc->local.expirationTime =  wns::simulator::getEventScheduler()->getTime() + this->msduLifetimeLimit;
        MESSAGE_SINGLE(NORMAL, logger_, "Outgoing command will expire at " << mc->local.expirationTime);
    }
    else
    {
        mc->local.expirationTime = 0.0;
        MESSAGE_SINGLE(NORMAL, logger_, "Outgoing command, no expiration time");
    }
}

bool
Manager::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    wns::ldk::CompoundPtr compound2send = compound->copy();

    // The command has to be activated to be considered in the createReply chain
    ManagerCommand* mc = activateCommand(compound2send->getCommandPool());
    mc->peer.CompoundType = DATA;
    mc->peer.hasPayload = true;
    mc->peer.frameExchangeDuration = this->sifsDuration + this->expectedACKDuration;

    // Sets the phymode according to the configuration of the drpscheduler
    wns::service::dll::UnicastAddress rx = getFUN()->getCommandReader(ucName_)
        ->readCommand<dll::UpperCommand>(compound2send->getCommandPool())
        ->peer.targetMACAddress;
    //int masNumber_ = this->getMASNumber(wns::simulator::getEventScheduler()->getTime());
    //mc->peer.phyMode = friends.drpScheduler->getPhyMode(rx, masNumber_);
    mc->peer.phyMode = friends.drpScheduler->getPhyMode(rx, -1);

    //return getConnector()->hasAcceptor(compound2send);
    return wns::ldk::Processor<Manager>::doIsAccepting(compound2send);
}

bool
Manager::lifetimeExpired(const wns::ldk::CommandPool* commandPool) const
{
    wns::simulator::Time exp = getCommand(commandPool)->local.expirationTime;
    if(exp == 0)
    {
        return false;
    }
    else
    {
        return(wns::simulator::getEventScheduler()->getTime() > exp);
    }
}

wns::simulator::Time
Manager::getExpirationTime(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->local.expirationTime);
}

wimemac::convergence::PhyUser*
Manager::getPhyUser()
{
    if(friends.phyUser)
    {
        return(friends.phyUser);
    }
    else
    {
        return(getFUN()->findFriend<wimemac::convergence::PhyUser*>(config_.get<std::string>("phyUserName")));
    }
}

wimemac::management::ProtocolCalculator*
Manager::getProtocolCalculator()
{
    return protocolCalculator;
}

dll::Layer2::StationType
Manager::getStationType() const
{
    // simply relayed to Layer2
    return(this->getFUN()->getLayer<dll::Layer2*>()->getStationType());
}

wns::service::dll::UnicastAddress
Manager::getMACAddress() const
{
    return(this->myMACAddress_);
}

wns::ldk::CommandPool*
Manager::createReply(const wns::ldk::CommandPool* original) const
{
    wns::ldk::CommandPool* reply = this->getFUN()->getProxy()->createReply(original, this);

    dll::UpperCommand* ucReply = getFUN()->getProxy()->getCommand<dll::UpperCommand>(reply, ucName_);
    dll::UpperCommand* ucOriginal = getFUN()->getCommandReader(ucName_)->readCommand<dll::UpperCommand>(original);

    // this cannot be set to this->myAddress_, because it is not defined which
    // Manager-entity will be asked for createReply
    ucReply->peer.sourceMACAddress = ucOriginal->peer.targetMACAddress;

    ManagerCommand* mc = activateCommand(reply);
    // Sets the phymode to the same as the original compounds phymode
    wimemac::lowerMAC::ManagerCommand* mcOriginal = this->getCommand(original);
    mc->peer.phyMode = mcOriginal->getPhyMode();

    mc->peer.CompoundType = mcOriginal->getCompoundType();

    if(this->msduLifetimeLimit > 0)
    {
        mc->local.expirationTime =  wns::simulator::getEventScheduler()->getTime() + this->msduLifetimeLimit;
    }
    else
    {
        mc->local.expirationTime = 0;
    }


    MESSAGE_SINGLE(NORMAL, logger_, "create reply done, set sourceMACAddress to " << ucReply->peer.sourceMACAddress);

    return(reply);
}

wns::ldk::CommandPool*
Manager::createReply(const wns::ldk::CommandPool* original, wimemac::CompoundType compoundType) const
{
    wns::ldk::CommandPool* reply = this->getFUN()->getProxy()->createReply(original, this);

    dll::UpperCommand* ucReply = getFUN()->getProxy()->getCommand<dll::UpperCommand>(reply, ucName_);
    dll::UpperCommand* ucOriginal = getFUN()->getCommandReader(ucName_)->readCommand<dll::UpperCommand>(original);

    // this cannot be set to this->myAddress_, because it is not defined which
    // Manager-entity will be asked for createReply
    ucReply->peer.sourceMACAddress = ucOriginal->peer.targetMACAddress;

    ManagerCommand* mc = activateCommand(reply);
    mc->peer.CompoundType = compoundType;
    if (compoundType == ACK)
    {
        mc->peer.hasPayload = false;
    }
    // Sets the phymode to the same as the original compounds phymode
    wimemac::lowerMAC::ManagerCommand* mcOriginal = this->getCommand(original);
    mc->peer.phyMode = mcOriginal->getPhyMode();

    if(this->msduLifetimeLimit > 0)
    {
        mc->local.expirationTime =  wns::simulator::getEventScheduler()->getTime() + this->msduLifetimeLimit;
    }
    else
    {
        mc->local.expirationTime = 0;
    }


    MESSAGE_SINGLE(NORMAL, logger_, "create reply done, set sourceMACAddress to " << ucReply->peer.sourceMACAddress);

    return(reply);
}

wns::service::dll::UnicastAddress
Manager::getTransmitterAddress(const wns::ldk::CommandPool* commandPool) const
{
    return (getFUN()->getCommandReader(ucName_)->readCommand<dll::UpperCommand>(commandPool)->peer.sourceMACAddress);
}

wns::service::dll::UnicastAddress
Manager::getReceiverAddress(const wns::ldk::CommandPool* commandPool) const
{
    return (getFUN()->getCommandReader(ucName_)->readCommand<dll::UpperCommand>(commandPool)->peer.targetMACAddress);
}

bool
Manager::isForMe(const wns::ldk::CommandPool* commandPool) const
{
    if(getFUN()->getCommandReader(ucName_)->commandIsActivated(commandPool))
    {
        return(getFUN()->getCommandReader(ucName_)->readCommand<dll::UpperCommand>(commandPool)->peer.targetMACAddress == this->myMACAddress_);
    }
    else
    {
        // broadcast -> is for me
        return true;
    }
}

wimemac::CompoundType
Manager::getCompoundType(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->peer.CompoundType);
}

std::string
Manager::getPreambleMode(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->peer.phyMode.getPreambleMode());
}

bool
Manager::hasPayload(const wns::ldk::CommandPool* commandPool) const
{
    return (getCommand(commandPool)->peer.hasPayload);
}


void
Manager::setHasPayload(const wns::ldk::CommandPool* commandPool, bool setPayloadTo)
{
    getCommand(commandPool)->peer.hasPayload = setPayloadTo;
}


bool
Manager::isPreamble(const wns::ldk::CommandPool* commandPool) const
{
    return (getCommand(commandPool)->peer.CompoundType == ACK_PREAMBLE || getCommand(commandPool)->peer.CompoundType == BEACON_PREAMBLE || getCommand(commandPool)->peer.CompoundType == DATA_PREAMBLE);
}

bool
Manager::isBeacon(const wns::ldk::CommandPool* commandPool) const
{
    return friends.drpScheduler->isBeacon(commandPool);
}

wns::ldk::CompoundPtr
Manager::createCompound(const wns::service::dll::UnicastAddress transmitterAddress,
                                const wns::service::dll::UnicastAddress receiverAddress,
                                const CompoundType compoundType,
                                const bool hasPayload,
                                //const FrameType frameType,
                                const wns::simulator::Time frameExchangeDuration,
                                const bool requiresDirectReply)
{
    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool()));
    ManagerCommand* mc = activateCommand(compound->getCommandPool());
    dll::UpperCommand* uc = friends.upperConvergence->activateCommand(compound->getCommandPool());

    uc->peer.sourceMACAddress = transmitterAddress;
    uc->peer.targetMACAddress = receiverAddress;
    mc->peer.CompoundType = compoundType;
    mc->peer.hasPayload = hasPayload;
    mc->peer.frameExchangeDuration = frameExchangeDuration;
    mc->peer.psduDuration = 0.0;
    mc->peer.requiresDirectReply = requiresDirectReply;

    // phymode is set by the compounds creating instance

    return(compound);
}

void
Manager::setCompoundType(const wns::ldk::CommandPool* commandPool, const CompoundType type)
{
    getCommand(commandPool)->peer.CompoundType = type;
}

wimemac::convergence::PhyMode
Manager::getPhyMode(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->getPhyMode());
}

void
Manager::setPhyMode(const wns::ldk::CommandPool* commandPool, const wimemac::convergence::PhyMode phyMode)
{
    getCommand(commandPool)->peer.phyMode = phyMode;
}

wimemac::convergence::PhyMode
Manager::getDefaultPhyMode() const
{
    return(friends.phyUser->getPhyModeProvider()->getDefaultPhyMode());
}

wns::Power
Manager::getRxPower(const wns::ldk::CommandPool* commandPool)
{
    return friends.phyUser->getRxPower(commandPool);
}

wns::Power
Manager::getInterference(const wns::ldk::CommandPool* commandPool)
{
    return friends.phyUser->getInterference(commandPool);
}

wimemac::convergence::MCS
Manager::getMaxPosMCS(wns::Ratio sinr_, Bit maxCompoundSize_, double per_)
{
    return friends.errorModelling->getMaxPosMCS(sinr_, maxCompoundSize_, per_);
}


double
Manager::getErrorRateForCommandFrames(wns::Ratio sinr_, Bit maxCompoundSize_)
{
    return friends.errorModelling->getErrorRateForCommandFrames(sinr_,maxCompoundSize_);
}

int
Manager::getMASNumber(wns::simulator::Time time_)
{
    // +10E-6 offset to avoid rounding errors
    // The shortest possible TxDuration is 18E-6s. Thus even with the offset the time is definitely within the TxTime
    wns::simulator::Time timeInSF_ = time_ - BPStartTime + 10E-6;
    double masNumberdouble_ = timeInSF_ / 256E-6;
    int masNumber_ = masNumberdouble_;

    return masNumber_;
}

double
Manager::getDesiredPER()
{
    return friends.drpScheduler->getDesiredPER();
}

double
Manager::getPatternPEROffset()
{
    return friends.drpScheduler->getPatternPEROffset();
}

wns::simulator::Time
Manager::getFrameExchangeDuration(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->peer.frameExchangeDuration);
}

void
Manager::setFrameExchangeDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration)
{
    getCommand(commandPool)->peer.frameExchangeDuration = duration;
}

wns::simulator::Time
Manager::getpsduDuration(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->peer.psduDuration);
}

void
Manager::setpsduDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration)
{
    getCommand(commandPool)->peer.psduDuration = duration;
}

bool
Manager::getRequiresDirectReply(const wns::ldk::CommandPool* commandPool) const
{
    return(getCommand(commandPool)->peer.requiresDirectReply);
}

void
Manager::setRequiresDirectReply(const wns::ldk::CommandPool* commandPool, bool requiresDirectReply)
{
    getCommand(commandPool)->peer.requiresDirectReply = requiresDirectReply;
}

// For BeaconBuilder Services
/** @brief Forwardes the call to the BeaconBuilder */
void
Manager::prepareDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    friends.drpScheduler->getBeaconBuilder()->prepareDRPConnection(rx, CompoundspSF, BitspSF, MaxCompoundSize);
}

void
Manager::updateDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    friends.drpScheduler->getBeaconBuilder()->updateDRPConnection(rx, CompoundspSF, BitspSF, MaxCompoundSize);
}

void
Manager::BuildDTPmap()
{
    friends.drpScheduler->getBeaconBuilder()->BuildDTPmap();
}
        
void
Manager::SetBPDuration(wns::simulator::Time duration)
{
    friends.drpScheduler->getBeaconBuilder()->SetBPDuration(duration);
}

// For DRPScheduler Services
bool 
Manager::startPCAtransmission()
{
    friends.drpScheduler->startPCAtransmission();
}
 
void
Manager::stopPCAtransmission()
{
    friends.drpScheduler->stopPCAtransmission();
}

void
Manager::txOPCloseIn(wns::simulator::Time duration)
{
    friends.drpScheduler->txOPCloseIn(duration);
}

int
Manager::getNumOfRetransmissions(const wns::ldk::CompoundPtr& compound)
{
    friends.drpScheduler->getNumOfRetransmissions(compound);
}

wns::service::dll::UnicastAddress
Manager::getCurrentTransmissionTarget()
{
    friends.drpScheduler->getCurrentTransmissionTarget();
}

bool
Manager::UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap)
{
    friends.drpScheduler->UpdateMapWithPeerAvailabilityMap(rx, DRPMap);
}

bool
Manager::adjustMCSdown(wns::service::dll::UnicastAddress rx)
{
    friends.drpScheduler->adjustMCSdown(rx);
}

void
Manager::UpdateDRPMap(Vector DRPMap)
{
    friends.drpScheduler->UpdateDRPMap(DRPMap);
}

void
Manager::onBPStart(wns::simulator::Time BPduration)
{
    friends.drpScheduler->onBPStart(BPduration);
}

void 
Manager::Acknowledgment(wns::service::dll::UnicastAddress tx)
{
    friends.drpScheduler->Acknowledgment(tx);
}
