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
#include <WIMEMAC/frame/DataCollector.hpp>


using namespace wimemac::frame;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	wimemac::frame::DataCollector,
	wns::ldk::FunctionalUnit,
	"wimemac.frame.DataCollector",
	wns::ldk::FUNConfigCreator );


DataCollector::DataCollector( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config ) :
		wns::ldk::fcf::CompoundCollector( config ),
		wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>(fun),
		logger(config.get("logger")),
		duration(config.get<wns::simulator::Time>("duration")),
		_config(config),
		sending(false)
				
		
{
	MESSAGE_SINGLE(NORMAL, logger, "DataCollector ");
	
}

void 
DataCollector::doOnData( const wns::ldk::CompoundPtr& compound )
{
	assure(mode==CompoundCollector::Sending,"DataCollector: received compound although not in receiving mode!");
	getDeliverer()->getAcceptor( compound )->onData( compound );
}

bool 
DataCollector::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
	MESSAGE_SINGLE(NORMAL, logger, "DataCollector: doIsAccepting? ");	
	return (CompoundCollector::getMode() == CompoundCollector::Sending);
	
}

void 
DataCollector::doSendData( const wns::ldk::CompoundPtr& compound )
{
    assure(mode==CompoundCollector::Sending, "DataCollector: Got compound from above although not in sending mode!");
    if(!getConnector()->hasAcceptor(compound)){
        MESSAGE_SINGLE(NORMAL, logger, "DataCollector: can't send Data!");
        assure(false, "DataCollector: compound is not accepted from the lower FUs even though in sending mode! A transmission may already be in progress");
    }
    else{
        getConnector()->getAcceptor(compound)->sendData(compound);
        MESSAGE_SINGLE(NORMAL, logger, "DataCollector: Data sent, waiting for Phy to finish!");
    }
}


void 
DataCollector::doStart(int mode)
{
	switch (mode)
	{
	case CompoundCollector::Sending:
		
		MESSAGE_SINGLE(NORMAL, logger, "DataCollector: start: " << duration);
		setTimeout(duration);
		getReceptor()->wakeup();
		sending = true;
		break;

	case CompoundCollector::Receiving:
		break;
	default:
		throw wns::Exception("Unknown mode in CompoundCollector");
	}
}

void
DataCollector::onTimeout()
{

	MESSAGE_SINGLE(NORMAL, logger, "DataCollector: Data phase is over now! ");		
	getFrameBuilder()->finishedPhase(this);
	sending = false;
	CompoundCollector::mode = CompoundCollector::Pausing;
	
}


void
DataCollector::onFUNCreated()
{
    MESSAGE_SINGLE(NORMAL, logger, "DataCollector: created! duration: " << duration << " Mode: " << getMode());
    getFUN()->findFriend<wns::ldk::fcf::FrameBuilder*>("FrameBuilder")->start();
}

void
DataCollector::doWakeup()
{
    if(sending == true)
    {
        MESSAGE_SINGLE(NORMAL, logger, "DataCollector: Phy finished sending Data!");
        getReceptor()->wakeup();
    }
    else
        MESSAGE_SINGLE(NORMAL, logger, "DataCollector: doWakeup, but not in sending mode");
}
