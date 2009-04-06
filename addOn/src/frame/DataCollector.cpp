/******************************************************************************
 * WiMeMAC                                                                    *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2009                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                   *
 * email: hor@comnets.de, www: http://wns.comnets.rwth-aachen.de/             *
 *****************************************************************************/
#include <WIMEMAC/frame/DataCollector.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	wimemac::frame::DataCollector,
	wns::ldk::FunctionalUnit,
	"wimemac.frame.DataCollector",
	wns::ldk::FUNConfigCreator );

using namespace wimemac::frame;

DataCollector::DataCollector( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config ) :
		wns::ldk::fcf::CompoundCollector( config ),
		wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>(fun),
		logger(config.get("logger")),
		duration(config.get<wns::simulator::Time>("duration"))	
		
{
	MESSAGE_SINGLE(NORMAL, logger, "DataCollector ");
	
	
}

void 
DataCollector::doOnData( const wns::ldk::CompoundPtr& compound )
{
	/*assure(mode==CompoundCollector::Sending,"DataCollector: received compound although not in receiving mode!");
	getDeliverer()->getAcceptor( compound )->onData( compound );*/
}

bool 
DataCollector::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
	/*MESSAGE_SINGLE(NORMAL, logger, "DataCollector: BeaconMode: " << CompoundCollector::getMode());	
	return (CompoundCollector::getMode() == CompoundCollector::Sending && sending == false);*/
	return false;
}

void 
DataCollector::doSendData( const wns::ldk::CompoundPtr& compound )
{
	/*assure(mode==CompoundCollector::Sending, "DataCollector: Got compound from above although not in sending mode!");
	if(!getConnector()->hasAcceptor(compound)){
		MESSAGE_SINGLE(NORMAL, logger, "DataCollector: can't send Beacon!");
		gotWakeup = false;
	}
	else{
		getConnector()->getAcceptor(compound)->sendData(compound);
		sending = true;
		MESSAGE_SINGLE(NORMAL, logger, "DataCollector: Beacon sent, waiting for Phy to finish!");
	}*/
}


void 
DataCollector::doStart(int mode)
{
	switch (mode)
	{
	case CompoundCollector::Sending:
		
		MESSAGE_SINGLE(NORMAL, logger, "DataCollector: start: " << duration);
		setTimeout(duration);
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
	CompoundCollector::mode = CompoundCollector::Pausing;
	
}


void 
DataCollector::onFUNCreated()
{	
	MESSAGE_SINGLE(NORMAL, logger, "DataCollector: created! duration: " << duration << " Mode: " << getMode());
	getFUN()->findFriend<wns::ldk::fcf::FrameBuilder*>("FrameBuilder")->start();


	/*friends.beaconbuilder = 
	getFUN()->findFriend<glue::wimemac::frame::BeaconBuilder*>("BeaconBuilder");*/
}

void 
DataCollector::doWakeup()
{
	/*if(sending)
	{
		MESSAGE_SINGLE(NORMAL, logger, "DataCollector: Phy finished sending beacon!");
		sending = false;
	}*/
}




/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
