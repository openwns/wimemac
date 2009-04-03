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
#include "<WIMEMAC/BeaconBuilder.hpp>
#include <WIMEMAC/convergence/Upper.hpp>
#include <DLL/Layer2.hpp>



STATIC_FACTORY_REGISTER_WITH_CREATOR(
	wimemac::frame::BeaconBuilder,
	wns::ldk::FunctionalUnit,
	"wimemac.frame.BeaconBuilder",
	wns::ldk::FUNConfigCreator );

using namespace wimemac::frame;

BeaconBuilder::BeaconBuilder( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ ) :
		wns::ldk::fu::Plain<BeaconBuilder, BeaconCommand>(fun),
		logger(config_.get("logger")),
		currentBeacon(),
		isDRPconnection(true)

				
		
{

}

void 
BeaconBuilder::doOnData( const wns::ldk::CompoundPtr& compound )
{	

	//assure(friends.manager->getFrameType(compound->getCommandPool()) == BEACON, "Received frame is not a Beacon");
	//if(friends.manager->getTransmitterAddress(compound->getCommandPool())==friends.manager->getMACAddress())
	
	//assure(mode==CompoundCollector::Sending,"BeaconBuilder: received compound although not in receiving mode!");
	//getDeliverer()->getAcceptor( compound )->onData( compound );
	
	/*if(getFUN()->getLayer<dll::Layer2*>()->getDLLAddress() == (getCommand(compound->getCommandPool()))->peer.address)
		isDRPconnection = true;*/
	//friends.queueInterface->TimeToTransmit(getCommand(compound->getCommandPool())->peer.address,1);
	BeaconEvaluator::BeaconExamination(getCommand(compound->getCommandPool()), isDRPconnection, logger);
	MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Receive Beacon from " << (getCommand(compound->getCommandPool()))->peer.address);



	
}

bool 
BeaconBuilder::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
	//MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: BeaconMode: " << CompoundCollector::getMode());	
	//return (true);
}

void 
BeaconBuilder::doSendData( const wns::ldk::CompoundPtr& compound )
{
	//this->currentBeacon = friends.manager->createCompound(friends.manager->getMACAddrress(), wns::service::dll::UnicastAddress(),BEACON, 0.0);

	//assure(mode==CompoundCollector::Sending, "BeaconBuilder: Got compound from above although not in sending mode!");
	if(!getConnector()->hasAcceptor(compound)){
		MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: can't send Beacon!");
	}
	else{
		getConnector()->getAcceptor(compound)->sendData(compound);
		MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Beacon sent, waiting for Phy to finish!");
	}
}



void 
BeaconBuilder::onFUNCreated()
{	

	
	friends.queueInterface=
	getFUN()->findFriend<wimemac::helper::IDRPQueueInterface*>("DRPScheduler");

	friends.broadcastUpper = 
	getFUN()->findFriend<wimemac::convergence::BroadcastUpper*>("broadcastUpperConvergence");//(config.get<std::string>("broadcastRouting"));

	BeaconEvaluator::setFriend(friends.queueInterface);

	
	//MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: created! duration: " << duration << " Mode: " << getMode());
}

void 
BeaconBuilder::doWakeup()
{
	MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Phy finished sending beacon!");
	//CreateBeacon();
	doSendData(CreateBeacon());
	

}
void 
BeaconBuilder::calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{	
	getFUN()->getProxy()->calculateSizes(commandPool, commandPoolSize, sduSize,this);
	Bit MacHead = 32;
	commandPoolSize = MacHead;
	sduSize+= MacHead;
 
}
wns::ldk::CompoundPtr
BeaconBuilder::CreateBeacon()
{
	
	wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->getProxy()->createCommandPool()));
	
	BeaconCommand* bc = activateCommand(compound->getCommandPool());
	bc->peer.reservationtype = BeaconCommand::Hard;
	
	
	wimemac::convergence::BroadcastUpperCommand* buc = friends.broadcastUpper->activateCommand(compound->getCommandPool());
	buc->peer.targetMACAddress=wns::service::dll::BroadcastAddress();

	bc->peer.address = (getFUN()->findFriend<wimemac::convergence::UnicastUpper*>("unicastUpperConvergence"))->getMACAddress();

	return compound;
	
}

void
BeaconBuilder::BuildDTPmap()
{
	MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: BP is over and now build a new DTP map!");
	BeaconEvaluator::CollectDRPmaps();
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
