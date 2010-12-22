/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2010
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

#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/ldk/concatenation/Concatenation.hpp>

using namespace wimemac::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::convergence::PhyUser,
    wns::ldk::FunctionalUnit,
    "wimemac.convergence.PhyUser",
    wns::ldk::FUNConfigCreator);

PhyUser::PhyUser(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
    wns::ldk::fu::Plain<PhyUser, PhyUserCommand>(fun),
    config(_config),
    logger(config.get<wns::pyconfig::View>("logger")),
    tune(),
    dataTransmission(NULL),
    notificationService(NULL),
    phyModes(config.getView("phyModesDeliverer")),
    managerName(config.get<std::string>("managerName")),
    txDurationCommandName(config.get<std::string>("txDurationCommandName")),
    txrxTurnaroundDelay(config.get<wns::simulator::Time>("txrxTurnaroundDelay")),
    phyUserStatus(receiving),
    currentTxCompound(),
    rxProbe_(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),
                              "wimemac.phyuser.rxPower")),
    interferenceProbe_(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),
                              "wimemac.phyuser.interferencePower")),
    SINRProbe_(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),
                            "wimemac.phyuser.SINR")),
    SINRweightedProbe_(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()),
                            "wimemac.phyuser.SINRweighted"))
{
    tune.frequency = config.get<double>("initFrequency");
    tune.bandwidth = config.get<double>("initBandwidthMHz");
    tune.numberOfSubCarrier = 1;

    MESSAGE_SINGLE(NORMAL, logger, "Signal frequency is set to " << tune.frequency);
}
// PhyUser


PhyUser::~PhyUser()
{

}

void PhyUser::onFUNCreated()
{
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);
} // onFUNCreated

bool PhyUser::doIsAccepting(const wns::ldk::CompoundPtr& /* compound */) const
{
    // we only accept if we are not currently transmitting
    return (phyUserStatus != transmitting);
} // isAccepting



void PhyUser::doSendData(const wns::ldk::CompoundPtr& compound)
{
    assure(compound, "sendData called with an invalid compound.");
    assure(phyUserStatus != transmitting, "Cannot send data during transmission");

    MESSAGE_SINGLE(NORMAL, logger, "Rx Frequency is set to: " <<  (dataTransmission->getRxTune()).frequency);
    MESSAGE_SINGLE(NORMAL, logger, "Tx Frequency is set to: " <<  (dataTransmission->getTxTune()).frequency);

    if(!friends.manager->isPreamble(compound->getCommandPool()))
    {
        // signal tx start to the MAC
        this->wns::Subject<ITxStartEnd>::forEachObserver(OnTxStartEnd(compound, start));
    }

    wns::simulator::Time frameTxDuration = getFUN()->getCommandReader(txDurationCommandName)->
        readCommand<wimemac::convergence::TxDurationSetterCommand>(compound->getCommandPool())->getDuration();

    PhyUserCommand* command = activateCommand(compound->getCommandPool());

    // generate functor
    wimemac::convergence::BroadcastOFDMAAccessFunc* func = new wimemac::convergence::BroadcastOFDMAAccessFunc;

    // transmit now
    func->transmissionStart = wns::simulator::getEventScheduler()->getTime();
    func->transmissionStop = wns::simulator::getEventScheduler()->getTime() + frameTxDuration;
    func->subBand = 1;

    command->local.pAFunc.reset(func);
    (*command->local.pAFunc.get())(this, compound);

    MESSAGE_SINGLE(NORMAL, logger, "Transmission, rx disabled for " << frameTxDuration);
    if(phyUserStatus == txrxTurnaround)
    {
        setNewTimeout(frameTxDuration);
    }
    else
    {
        setTimeout(frameTxDuration);
    }
    // we are transmitting!
    phyUserStatus = transmitting;

    this->currentTxCompound = compound;

    MESSAGE_SINGLE(NORMAL, logger, "onSendData: Compound  " << *(compound));

} // doSendData

void PhyUser::doOnData(const wns::ldk::CompoundPtr& compound)
{
    assure(compound, "onData called with an invalid compound.");

    getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData

void PhyUser::onData(wns::osi::PDUPtr pdu, wns::service::phy::power::PowerMeasurementPtr rxPowerMeasurement)
{
    assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");

    if(phyUserStatus != receiving)
    {
        // During transmission, the receiver is off
        return;
    }

    // FIRST: create a copy instead of working on the real compound
    wns::ldk::CompoundPtr compound = wns::staticCast<wns::ldk::Compound>(pdu)->copy();

    PhyUserCommand* phyCommand = getCommand(compound->getCommandPool());

    // store measured signal into Command
    phyCommand->local.rxPower      = rxPowerMeasurement->getRxPower();
    phyCommand->local.interference = rxPowerMeasurement->getInterferencePower();


//     MESSAGE_SINGLE(NORMAL, logger, "onData: rxPower  " << phyCommand->local.rxPower);
//     MESSAGE_SINGLE(NORMAL, logger, "onData: interference  " << phyCommand->local.interference);
//     MESSAGE_SINGLE(NORMAL, logger, "onData: Compound  " << *(compound));

    // set probe values
    if(friends.manager->isForMe(compound->getCommandPool() ) && !friends.manager->isBeacon(compound->getCommandPool() ))
    {
        wns::Ratio sinr_ = wns::Ratio::from_dB(phyCommand->local.rxPower.get_dBm() - phyCommand->local.interference.get_dBm());

        rxProbe_->put(phyCommand->local.rxPower.get_dBm());
        interferenceProbe_->put(phyCommand->local.interference.get_dBm());
        SINRProbe_->put(sinr_.get_dB() );

        // Write weighted SINR probe; probe is weighted according to the tx duration in µs
        wns::simulator::Time txDuration_ = getFUN()->getCommandReader(txDurationCommandName)->
        readCommand<wimemac::convergence::TxDurationSetterCommand>(compound->getCommandPool())->getDuration();

        int weightingFactor_ = ceil(txDuration_*1E6); // weightingFactor = number of µs of tx duration
        for (int i = 0; i < weightingFactor_; i++)
        {
            SINRweightedProbe_->put(sinr_.get_dB() );
        }
    }

    this->wns::ldk::FunctionalUnit::onData(compound);
} // onData

void PhyUser::setDataTransmissionService(wns::service::Service* phy)
{
    assure(phy, "must be non-NULL");
    assureType(phy, wns::service::phy::ofdma::DataTransmission*);
    dataTransmission = dynamic_cast<wns::service::phy::ofdma::DataTransmission*>(phy);
    dataTransmission->setRxTune(tune);
    dataTransmission->setTxTune(tune);
} // setDataTransmissionService

wns::service::phy::ofdma::DataTransmission* PhyUser::getDataTransmissionService() const
{
    assure(dataTransmission, "no ofdma::DataTransmission set. Did you call setDataTransmission()?");
    return dataTransmission;
} // getDataTransmissionService

void PhyUser::setNotificationService(wns::service::Service* phy)
{
    assure(phy, "must be non-NULL");
    assureType(phy, wns::service::phy::ofdma::Notification*);
    notificationService = dynamic_cast<wns::service::phy::ofdma::Notification*>(phy);

    // attach handler (there can be only one)
    notificationService->registerHandler(this);

} // setNotificationService

wns::service::phy::ofdma::Notification* PhyUser::getNotificationService() const
{
    assure(notificationService, "no ofdma::Notification set. Did you call setNotificationService()?");
    return notificationService;
} // getNotificationService

void PhyUser::doWakeup()
{
    assure(false, "PhyUser doWakeup will never be called -- nobody is below");
} // doWakeup

void PhyUser::onTimeout()
{
    assure(phyUserStatus != receiving, "Timeout although not transmitting");
    if(phyUserStatus == transmitting)
    {
        // finished transmission, start turnaround
        MESSAGE_SINGLE(NORMAL, logger, "Timeout, finished transmission");
        phyUserStatus = txrxTurnaround;
        setTimeout(txrxTurnaroundDelay);

        assure(this->currentTxCompound, "currentTxCompound is NULL");
        if(!friends.manager->isPreamble(this->currentTxCompound->getCommandPool()))
        {
            // signal tx end to MAC
            MESSAGE_SINGLE(NORMAL, logger, "No preamble -> signal tx end");
            this->wns::Subject<ITxStartEnd>::forEachObserver(OnTxStartEnd(this->currentTxCompound, end));
        }
        this->currentTxCompound = wns::ldk::CompoundPtr();

        // wakeup: we are ready to transmit another compound
        this->getReceptor()->wakeup();
        return;
    }

    if(phyUserStatus == txrxTurnaround)
    {
        // finished turnaround, ready to receive
        phyUserStatus = receiving;
        return;
    }

    assure(false, "Unknown phyUserStatus");
}

void PhyUser::setFrequency(double frequency)
{
    if(dataTransmission == NULL)
    {
        MESSAGE_SINGLE(NORMAL, logger, "cannot yet set RxFrequency, save for after initialisation");
        tune.frequency = frequency;
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "setRxFrequency to f: " << frequency << " MHz");
        tune.frequency = frequency;
        dataTransmission->setRxTune(tune);
        dataTransmission->setTxTune(tune);
    }
}

PhyModeProvider* PhyUser::getPhyModeProvider()
{
    return &phyModes;
}

wns::Power
PhyUser::getRxPower(const wns::ldk::CommandPool* commandPool)
{
    PhyUserCommand* phyCommand = getCommand(commandPool);
    return phyCommand->local.rxPower;
}

wns::Power
PhyUser::getInterference(const wns::ldk::CommandPool* commandPool)
{
    PhyUserCommand* phyCommand = getCommand(commandPool);
    return phyCommand->local.interference;
}

