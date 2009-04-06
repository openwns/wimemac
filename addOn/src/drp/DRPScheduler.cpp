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

#include <WIMEMAC/drp/DRPScheduler.hpp>




STATIC_FACTORY_REGISTER_WITH_CREATOR(
	wimemac::drp::DRPScheduler,
	wns::ldk::FunctionalUnit,
	"wimemac.drp.DRPScheduler",
	wns::ldk::FUNConfigCreator );

using namespace wimemac::drp;

DRPScheduler::DRPScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ ) :
		config(config_),
		wns::ldk::fu::Plain<DRPScheduler, wns::ldk::EmptyCommand>(fun),
		logger(config_.get("logger"))
					
		
{
	DRPQueues = new helper::Queues(config, fun);

}

void 
DRPScheduler::doOnData( const wns::ldk::CompoundPtr& compound )
{	

	MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Receive Beacon");
	
}

bool 
DRPScheduler::doIsAccepting( const wns::ldk::CompoundPtr& compound) const
{
	MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: doIsAccepting is called");	
	return (DRPQueues->isAccepting(compound));
	
}

void 
DRPScheduler::doSendData( const wns::ldk::CompoundPtr& compound )
{
	wns::service::dll::UnicastAddress iam 
	= getFUN()->findFriend<wimemac::convergence::UnicastUpper*>("unicastUpperConvergence")->getMACAddress();

	

	MESSAGE_SINGLE(NORMAL, logger, "put and I am " << iam);
	DRPQueues->put(compound);
	//this->currentBeacon = friends.manager->createCompound(friends.manager->getMACAddrress(), wns::service::dll::UnicastAddress(),BEACON, 0.0);

	//assure(mode==CompoundCollector::Sending, "DRPScheduler: Got compound from above although not in sending mode!");
	/*if(!getConnector()->hasAcceptor(compound)){
		MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: can't send Beacon!");
	}
	else{
		getConnector()->getAcceptor(compound)->sendData(compound);
		MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Beacon sent, waiting for Phy to finish!");
	}*/
}



void 
DRPScheduler::onFUNCreated()
{	

	
		

	
	//MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: created! duration: " << duration << " Mode: " << getMode());
}

void 
DRPScheduler::doWakeup()
{
	MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Phy finished sending beacon!");
	//CreateBeacon();
	//doSendData(CreateBeacon());
	

}
void
DRPScheduler::TimeToTransmit(wns::service::dll::UnicastAddress macaddress, wns::simulator::Time duration) const
{
	wns::service::dll::UnicastAddress iam 
	= getFUN()->findFriend<wimemac::convergence::UnicastUpper*>("unicastUpperConvergence")->getMACAddress();

	MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: TimeToTransmit is called for target " << macaddress <<" and I am " 
	<< iam);
	
	
	if(DRPQueues->queueHasPDUs(macaddress))
	{
		
		if(!getConnector()->hasAcceptor(DRPQueues->getHeadOfLinePDU(macaddress)))
		{	
			MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: Error, can't send compound in a regular DRP reservation!");
		}
		else
		{
			getConnector()->getAcceptor(DRPQueues->getHeadOfLinePDU(macaddress))
			->sendData(DRPQueues->getHeadOfLinePDU(macaddress));
			MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler, send compound with DRP channel access");
		}
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "DRPScheduler: TimeToTransmit, but there are no compounds!");
	}


	
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
