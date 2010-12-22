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
#include <WIMEMAC/arq/ARQ.hpp>
#include <DLL/Layer2.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::arq::ARQ,
    wns::ldk::FunctionalUnit,
    "wimemac.arq.ARQ",
    wns::ldk::FUNConfigCreator );

using namespace wimemac::arq;

ARQ::ARQ( wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& config_ ) :
        wns::ldk::fu::Plain<ARQ, ARQCommand>(_fun),
        logger(config_.get("logger")),
        managerName(config_.get<std::string>("managerName")),
        fun(_fun),
        retrans(0),
        SIFS(10E-6),
        AckDuration(13.125E-6)
{
    scheduler = wns::simulator::getEventScheduler();
}

void 
ARQ::doOnData( const wns::ldk::CompoundPtr& compound )
{
    dll::UpperCommand* command = fun->getCommandReader("upperConvergence")
        ->readCommand<dll::UpperCommand>(compound->getCommandPool());

    if(command->peer.targetMACAddress != ownaddress)
    {
        MESSAGE_SINGLE(NORMAL, this->logger, "Received compound that is not for me -> drop");
    }

    else
    {
        wns::service::dll::UnicastAddress rx = command->peer.sourceMACAddress;
        wns::service::dll::UnicastAddress currentPartner = friends.manager->getDRPScheduler()->getCurrentTransmissionTarget();

        // only process compound if it's from the currently active transmission partner or if no transmission is active
        if((currentPartner == rx) || (currentPartner == wns::service::dll::UnicastAddress()))
        {
            if(hasCommandOf(compound) && getCommand(compound->getCommandPool())->peer.Frametype == ARQCommand::Ack)
            {
            MESSAGE_SINGLE(NORMAL, logger, "ARQ: received an ACK compound ");

                if(getCommand(compound->getCommandPool())->peer.ACKpolicy == ARQCommand::ImmACK)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "ARQ: an immediate ack is received from station : " << rx
                        <<"  inform the drp buffer about the acknowledged compound ");
                    getFUN()->findFriend<wimemac::drp::DRPScheduler*>("DRPScheduler")->Acknowledgment(rx);

                }
            }
            else
            {
                MESSAGE_SINGLE(NORMAL, logger, "ARQ: receive a data frame from " << rx << ", create an ACK and forward data frame ");
                getDeliverer()->getAcceptor( compound )->onData( compound );
                CreateACK(compound);
            }
        }
        else MESSAGE_SINGLE(NORMAL, logger, "ARQ: Received a compound from " << rx << ", but the current partner is " << currentPartner << ". Dropping the compound.");
    }
}

bool 
ARQ::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
    return true;
}

void 
ARQ::doSendData( const wns::ldk::CompoundPtr& compound )
{
    if(!hasCommandOf(compound))
    {
        ARQCommand* ac = activateCommand(compound->getCommandPool());
        ac->peer.ACKpolicy  = ARQCommand::ImmACK;
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "ARQ: Command is already activated, ACKPolicy is set to: "
        <<getCommand(compound->getCommandPool())->peer.ACKpolicy
        <<" Target is : "<< (fun->getCommandReader("upperConvergence")
        ->readCommand<dll::UpperCommand>(compound->getCommandPool()))->peer.targetMACAddress);
    }

    if(!getConnector()->hasAcceptor(compound)){
        if(getCommand(compound->getCommandPool())->peer.Frametype == ARQCommand::Ack)
        {
            MESSAGE_SINGLE(NORMAL, logger, "ARQ: can't send ACK");
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "ARQ: can't send Data");
        }
    }
    else{

        if(getCommand(compound->getCommandPool())->peer.Frametype == ARQCommand::Ack)
        {
            MESSAGE_SINGLE(NORMAL, logger, "ARQ: send ACK");
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "ARQ: send Data");
        }
        getConnector()->getAcceptor(compound)->sendData(compound);
    }
}


void 
ARQ::onFUNCreated()
{
    ownaddress = getFUN()->findFriend<dll::UpperConvergence*>("upperConvergence")->getMACAddress();
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);
}

void 
ARQ::doWakeup()
{
}

bool
ARQ::hasCommandOf(const wns::ldk::CompoundPtr& compound) const
{
    return (getFUN()->getProxy()->commandIsActivated( compound->getCommandPool(), this ));
}

void
ARQ::CreateACK( const wns::ldk::CompoundPtr& compound)
{
    wns::ldk::CommandPool* AckCommand = friends.manager->createReply(compound->getCommandPool(), ACK);
    wns::ldk::CompoundPtr AckCompound(new wns::ldk::Compound(AckCommand));

    wns::simulator::Time fxDur = friends.manager->getFrameExchangeDuration(compound->getCommandPool()) - SIFS - AckDuration;
    if (fxDur < SIFS)
    {
        fxDur = 0;
    }
    friends.manager->setFrameExchangeDuration(AckCommand, fxDur);
    MESSAGE_SINGLE(NORMAL, logger, "ARQ: FrameExchangeDuration set to : " << fxDur);

    ARQCommand* ac = activateCommand(AckCompound->getCommandPool());
    ac->peer.Frametype = ARQCommand::Ack;
    ac->peer.ACKpolicy = ARQCommand::ImmACK;

    wns::events::scheduler::Callable call = boost::bind(&ARQ::TransmitAck, this, AckCompound);
    scheduler->scheduleDelay(call, SIFS);
}


void 
ARQ::calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, sduSize,this);
}
void
ARQ::TransmitAck(const wns::ldk::CompoundPtr& compound)
{
    MESSAGE_SINGLE(NORMAL, logger, "ARQ: send Ack after SIFS");
    doSendData(compound);
}

