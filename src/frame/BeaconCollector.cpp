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
 
#include <WIMEMAC/frame/BeaconCollector.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::frame::BeaconCollector,
    wns::ldk::FunctionalUnit,
    "wimemac.frame.BeaconCollector",
    wns::ldk::FUNConfigCreator );

using namespace wimemac::frame;

BeaconCollector::BeaconCollector( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config ) :
        wns::ldk::fcf::CompoundCollector( config ),
        wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>(fun),
        logger(config.get("logger")),
        duration(config.get<wns::simulator::Time>("duration")),
        gotWakeup(false),
        sending(false),
        managerName(config.get<std::string>("managerName")),
        BeaconSlot(config.get<int>("BeaconSlot")),
        BeaconSlotDuration(config.get<wns::simulator::Time>("BeaconSlotDuration"))

{
    

}

void
BeaconCollector::doOnData( const wns::ldk::CompoundPtr& compound )
{
    assure(mode==CompoundCollector::Sending,"BeaconCollector: received compound although not in receiving mode!");
    getDeliverer()->getAcceptor( compound )->onData( compound );
}

bool
BeaconCollector::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: BeaconMode: " << CompoundCollector::getMode());	
    return (CompoundCollector::getMode() == CompoundCollector::Sending && sending == false);
}

void
BeaconCollector::doSendData( const wns::ldk::CompoundPtr& compound )
{
    assure(mode==CompoundCollector::Sending, "BeaconCollector: Got compound from above although not in sending mode!");
    if(!getConnector()->hasAcceptor(compound)){
        MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: can't send Beacon!");
        gotWakeup = false;
    }
    else{
        getConnector()->getAcceptor(compound)->sendData(compound);
        sending = true;
        MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: Beacon sent, waiting for Phy to finish!");
    }
}


void
BeaconCollector::doStart(int mode)
{   
    switch (mode)
    {
    case CompoundCollector::Sending:
            
        MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: start: " << duration <<" BeaconSlot " << BeaconSlot);
        wns::simulator::getEventScheduler()->scheduleDelay(boost::bind(&BeaconCollector::TimeToTransmit, this), BeaconSlotDuration * BeaconSlot);
  
        setTimeout(duration);
        BPStart = wns::simulator::getEventScheduler()->getTime();
        friends.manager->setBPStartTime(BPStart);

        break;
    case CompoundCollector::Receiving:

        break;
    default:
        throw wns::Exception("Unknown mode in CompoundCollector");
    }
}

void
BeaconCollector::onTimeout()
{

    if(sending)
        assure(gotWakeup, "Phy Layer is yet not done sending! Check frame config!");

    getFrameBuilder()->finishedPhase(this);
    CompoundCollector::mode = CompoundCollector::Pausing;
    sending = false;
    friends.beaconbuilder->BuildDTPmap();
    MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: Beacon phase is over now!");
}

void
BeaconCollector::onFUNCreated()
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: created! duration: " << duration << " Mode: " << getMode());
    getFUN()->findFriend<wns::ldk::fcf::FrameBuilder*>("FrameBuilder")->start();


    friends.beaconbuilder = 
    getFUN()->findFriend<wimemac::management::IBeaconBuilderServices*>("BeaconBuilder");

    friends.manager = 
    getFUN()->findFriend<wimemac::lowerMAC::IManagerServices*>(managerName);

    friends.beaconbuilder->SetBPDuration(duration);
}

void
BeaconCollector::doWakeup()
{
    if(sending)
    {
        MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: Phy finished sending beacon!");
        sending = false;
    }
}

void
BeaconCollector::TimeToTransmit()
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: Time to transmit Beacon!");
    this->getReceptor()->wakeup();
}
