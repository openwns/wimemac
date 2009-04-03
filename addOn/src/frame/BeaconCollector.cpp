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
#include <WIMEMAC/BeaconCollector.hpp>

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
		BeaconSlot(config.get<int>("BeaconSlot")),
		beaconscheduler(this)
		
		
{
	MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: BeaconSlot: " << BeaconSlot);
	
	
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
		beaconscheduler.SetStartOffsetTime(BeaconSlot);
		setTimeout(duration);

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
	cancelPeriodicTimeout();
	BeaconSlotCounter = 0;
	friends.beaconbuilder->BuildDTPmap();
	MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: Beacon phase is over now!");
}


void
BeaconCollector::periodically()
{
	
	
}		




void 
BeaconCollector::onFUNCreated()
{	
	MESSAGE_SINGLE(NORMAL, logger, "BeaconCollector: created! duration: " << duration << " Mode: " << getMode());
	getFUN()->findFriend<wns::ldk::fcf::FrameBuilder*>("FrameBuilder")->start();


	friends.beaconbuilder = 
	getFUN()->findFriend<wimemac::frame::BeaconBuilder*>("BeaconBuilder");
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
