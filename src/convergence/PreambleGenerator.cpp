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

#include <WIMEMAC/convergence/PreambleGenerator.hpp>
#include <DLL/Layer2.hpp>

using namespace wimemac::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::convergence::PreambleGenerator,
    wns::ldk::FunctionalUnit,
    "wimemac.convergence.PreambleGenerator",
    wns::ldk::FUNConfigCreator);

PreambleGenerator::PreambleGenerator(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_) :
    wns::ldk::fu::Plain<PreambleGenerator, PreambleGeneratorCommand>(fun),
    phyUserName(config_.get<std::string>("phyUserName")),
    managerName(config_.get<std::string>("managerName")),
    logger(config_.get("logger")),
    pendingCompound(),
    pendingPreamble()
{
    MESSAGE_SINGLE(NORMAL, this->logger, "created");
    friends.manager = NULL;
    friends.phyUser = NULL;
}

PreambleGenerator::~PreambleGenerator()
{

}

void
PreambleGenerator::onFUNCreated()
{
    MESSAGE_SINGLE(NORMAL, this->logger, "onFUNCreated() started");

    friends.phyUser = getFUN()->findFriend<wimemac::convergence::PhyUser*>(phyUserName);
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);
}

void
PreambleGenerator::processIncoming(const wns::ldk::CompoundPtr& compound)
{
    if((friends.manager->isPreamble(compound->getCommandPool())) && (friends.manager->hasPayload(compound->getCommandPool())))
    {
        MESSAGE_SINGLE(NORMAL, this->logger, "Received PREAMBLE -> drop");
        // Save preambletype for following incoming compound with type payload
        lastReceivedCompoundType = friends.manager->getCompoundType(compound->getCommandPool());
    }
    else if((friends.manager->isPreamble(compound->getCommandPool())) && (!friends.manager->hasPayload(compound->getCommandPool())))
    {
        wimemac::CompoundType compoundType_ = friends.manager->getCompoundType(compound->getCommandPool());
        MESSAGE_SINGLE(NORMAL, this->logger, "Received PREAMBLE without PAYLOAD of type : " << compoundType_);
        // Change compoundtype from preamble to the actual type and deliver it
        if (compoundType_ == ACK_PREAMBLE)
        {
            friends.manager->setCompoundType(compound->getCommandPool(), ACK);
            getDeliverer()->getAcceptor(compound)->onData(compound);
        }
        else assure(false, "CompoundType : " << compoundType_ << "should have a payload!");
    }
    else
    {
        if (lastReceivedCompoundType == BEACON_PREAMBLE)
        {
            friends.manager->setCompoundType(compound->getCommandPool(), BEACON);
        }
        else if (lastReceivedCompoundType == DATA_PREAMBLE)
        {
            friends.manager->setCompoundType(compound->getCommandPool(), DATA);
        }
        else assure(false, "Last received compound type was : " << lastReceivedCompoundType << ". This compound is of type payload and should not follow such a compound!");
        // deliver frame
        getDeliverer()->getAcceptor(compound)->onData(compound);
        lastReceivedCompoundType == PAYLOAD;
    }
}

void
PreambleGenerator::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
    // compute transmission duration of the frame, dependent on the mcs
    wimemac::convergence::PhyMode phyMode =
        friends.manager->getPhyMode(compound->getCommandPool());
    wns::simulator::Time psduDuration =
        friends.manager->protocolCalculator->getDuration()->MSDU_PSDU(compound->getLengthInBits(), phyMode); // FCS and pad bits were not added yet

    // First we generate a preamble
    this->pendingPreamble = compound->copy();
    // set preamble compoundType according to outgoing compoundType
    wimemac::CompoundType outgoingCompoundType_ = friends.manager->getCompoundType(compound->getCommandPool());

    if (outgoingCompoundType_ == ACK)
    {
        friends.manager->setCompoundType(this->pendingPreamble->getCommandPool(),
                                  ACK_PREAMBLE);
    }
    else if (outgoingCompoundType_ == BEACON)
    {
        friends.manager->setCompoundType(this->pendingPreamble->getCommandPool(),
                                  BEACON_PREAMBLE);
    }
    else if (outgoingCompoundType_ == DATA or outgoingCompoundType_ == DATA_TXOP)
    {
        friends.manager->setCompoundType(this->pendingPreamble->getCommandPool(),
                                  DATA_PREAMBLE);
    }
    else assure(false, "Outgoing compound has no valid compoundType : " << outgoingCompoundType_);

    friends.manager->setPhyMode(this->pendingPreamble->getCommandPool(),
                                friends.phyUser->getPhyModeProvider()->getPreamblePhyMode(phyMode));
    if (friends.manager->hasPayload(compound->getCommandPool()))
    {
        wns::simulator::Time frameExchangeDuration = friends.manager->getFrameExchangeDuration(compound->getCommandPool());

        if(frameExchangeDuration == 0)
        {
            // Special compounds that may have not been initialized with a frameExchangeDuration
            friends.manager->setFrameExchangeDuration(this->pendingPreamble->getCommandPool(), psduDuration);
        }
        else friends.manager->setFrameExchangeDuration(this->pendingPreamble->getCommandPool(), frameExchangeDuration + psduDuration);

        friends.manager->setpsduDuration(this->pendingPreamble->getCommandPool(), psduDuration);
    }
    else
    {
        friends.manager->setFrameExchangeDuration(this->pendingPreamble->getCommandPool(), friends.manager->getFrameExchangeDuration(compound->getCommandPool()));

        friends.manager->setpsduDuration(this->pendingPreamble->getCommandPool(), 0);
    }

    PreambleGeneratorCommand* preambleCommand = activateCommand(this->pendingPreamble->getCommandPool());
    preambleCommand->peer.frameId = compound->getBirthmark();

    MESSAGE_SINGLE(NORMAL, logger, "Outgoing preamble with frame tx duration " << friends.manager->protocolCalculator->getDuration()->preamble(phyMode));

    PreambleGeneratorCommand* compoundCommand = activateCommand(compound->getCommandPool());

    friends.manager->setCompoundType(compound->getCommandPool(),
                                  PAYLOAD);
    this->pendingCompound = compound;
}

const wns::ldk::CompoundPtr
PreambleGenerator::hasSomethingToSend() const
{
    if(this->pendingPreamble)
    {
        assure(this->pendingCompound, "Pending preamble but no pending compound");
        return(this->pendingPreamble);
    }
    return(this->pendingCompound);
}

wns::ldk::CompoundPtr
PreambleGenerator::getSomethingToSend()
{
    assure(this->pendingCompound, "Called getSomethingToSend without pending compound");

    wns::ldk::CompoundPtr myFrame;
    if(this->pendingPreamble)
    {
        assure(this->pendingCompound, "Pending preamble but no pending compound");
        myFrame = this->pendingPreamble;
        this->pendingPreamble = wns::ldk::CompoundPtr();
        MESSAGE_SINGLE(NORMAL, logger, "Sending preamble");

        // If compound has no payload delete pending compound
        if (!friends.manager->hasPayload(this->pendingCompound->getCommandPool()))
        {
            this->pendingCompound = wns::ldk::CompoundPtr();
            MESSAGE_SINGLE(NORMAL, logger, "Compound without payload was dropped");
        }
    }
    else
    {
        myFrame = this->pendingCompound;
        this->pendingCompound = wns::ldk::CompoundPtr();
        MESSAGE_SINGLE(NORMAL, logger, "Sending psdu");
    }
    return(myFrame);
}

bool
PreambleGenerator::hasCapacity() const
{
    return(this->pendingCompound == wns::ldk::CompoundPtr());
}

void
PreambleGenerator::calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
    if(friends.manager->isPreamble(commandPool))
    {
        dataSize = 0;
        if (friends.manager->getPreambleMode(commandPool) == "STANDARD") commandPoolSize = 700;
        else if (friends.manager->getPreambleMode(commandPool) == "BURST") commandPoolSize = 500;

    }
    else
    {
        getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

        Bit psdusize_ = friends.manager->protocolCalculator->getFrameLength()->getPSDU(commandPoolSize + dataSize, friends.manager->getPhyMode(commandPool).getInfoBitsPer6Symbols());

        commandPoolSize = 0;
        dataSize = psdusize_;
    }
}
