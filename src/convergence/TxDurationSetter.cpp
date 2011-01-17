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

#include <WIMEMAC/convergence/TxDurationSetter.hpp>
#include <DLL/Layer2.hpp>

using namespace wimemac::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::convergence::TxDurationSetter,
    wns::ldk::FunctionalUnit,
    "wimemac.convergence.TxDurationSetter",
    wns::ldk::FUNConfigCreator);

TxDurationSetter::TxDurationSetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_) :
    wns::ldk::fu::Plain<TxDurationSetter, TxDurationSetterCommand>(fun),
    managerName(config_.get<std::string>("managerName")),
    logger(config_.get("logger"))
{
    MESSAGE_SINGLE(NORMAL, this->logger, "created");
    friends.manager = NULL;
}


TxDurationSetter::~TxDurationSetter()
{
}

void TxDurationSetter::onFUNCreated()
{
    MESSAGE_SINGLE(NORMAL, this->logger, "onFUNCreated() started");
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::IManagerServices*>(managerName);
}

void
TxDurationSetter::processIncoming(const wns::ldk::CompoundPtr& /*compound*/)
{

}

void
TxDurationSetter::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
    TxDurationSetterCommand* command = activateCommand(compound->getCommandPool());
    wimemac::convergence::PhyMode phyMode = friends.manager->getPhyMode(compound->getCommandPool());

    // calculate tx duration
    wns::simulator::Time preambleTxDuration = friends.manager->getProtocolCalculator()->getDuration()->preamble(phyMode);

    if(friends.manager->isPreamble(compound->getCommandPool()))
    {
        command->local.txDuration = preambleTxDuration;

        MESSAGE_BEGIN(NORMAL, this->logger, m, "Preamble");
        m << ": duration " << command->local.txDuration;
        MESSAGE_END();
    }
    else
    {
        command->local.txDuration = friends.manager->getProtocolCalculator()->getDuration()->PSDU_PPDU(compound->getLengthInBits(), phyMode) - preambleTxDuration;
        //MESSAGE_BEGIN(VERBOSE, this->logger, m, "Outgoing Compound with size ");
        MESSAGE_BEGIN(NORMAL, this->logger, m, "Outgoing Compound with size ");
        m << compound->getLengthInBits();
        m << " with nIBP6S " << phyMode.getInfoBitsPer6Symbols();
        m << " --> duration " << friends.manager->getProtocolCalculator()->getDuration()->PSDU_PPDU(compound->getLengthInBits(), phyMode);
        m << " - " << preambleTxDuration;
        MESSAGE_END();

        MESSAGE_BEGIN(NORMAL, this->logger, m, "Command");
        m << " start " << wns::simulator::getEventScheduler()->getTime();
        m << " stop " << wns::simulator::getEventScheduler()->getTime() + command->local.txDuration;
        MESSAGE_END();

    }
}

void
TxDurationSetter::calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
    getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, dataSize, this);
}

