/*******************************************************************************
 * WiMeMAC                                                                     *
 * __________________________________________________________________________  *
 *                                                                             *
 * Copyright (C) 2009                                                          *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                                *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                   *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                    *
 * email: <hor@comnets.rwth-aachen.de>, www: http://wns.comnets.rwth-aachen.de/*
 ******************************************************************************/




#include <WIMEMAC/BeaconEvaluator.hpp>



using namespace wimemac::frame;

BeaconEvaluator::BeaconEvaluator()
		
{
	DRPmapManager = new drp::DRPmap(256);
}

void
BeaconEvaluator::BeaconExamination(BeaconCommand* BeaconCommand, bool isDRPconnection,wns::logger::Logger _logger)
{
	logger =_logger;
	MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Receive Beacon");

	if(isDRPconnection)
	{
		if(DRPConnections.knows(BeaconCommand->peer.address))
		{
			DRPmanager = DRPConnections.find(BeaconCommand->peer.address);
			MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: address is already known");
		
		}
		else
		{	
			Vector test(256,false);	
			for(int ii = 80; ii < 100 ; ii++)
			{
				test[ii] = true;
			}
			
			
			DRPConnections.insert(BeaconCommand->peer.address, new drp::DRPManager(BeaconCommand->peer.address,
				 BeaconCommand->peer.reservationtype,BeaconCommand->peer.reasoncode, BeaconCommand->peer.devicetype,
				 test/*BeaconCommand->peer.DRPAlloc*/, queueInterface));
			MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator has built a new drp connection manager for target " << BeaconCommand->peer.address);
			DRPmanager = DRPConnections.find(BeaconCommand->peer.address);
		}
	}
	else
	{
		if(BeaconCommand->peer.reasoncode == BeaconCommand::Accept && BeaconCommand->peer.reservationtype == BeaconCommand::Hard)
		{
			MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Hard");
			DRPmapManager->UpdateHardDRPmap(BeaconCommand->peer.DRPAlloc, logger);
		}
		else if(BeaconCommand->peer.reasoncode == BeaconCommand::Accept && BeaconCommand->peer.reservationtype == BeaconCommand::Soft)
		{
			MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Soft");
			Vector::iterator test = BeaconCommand->peer.DRPAlloc.begin();
			DRPmapManager->UpdateSoftDRPmap(BeaconCommand->peer.DRPAlloc, logger);
			
		}
	}
}

void
BeaconEvaluator::CollectDRPmaps()
{
	for(it = DRPConnections.begin(); it != DRPConnections.end() ;++it)
	{
		
		if((*it).second->GetReasonCode() == BeaconCommand::Accept)
		{
			(*it).second->StartRegisterReservation();
			MESSAGE_SINGLE(NORMAL, logger, "StartRegister");
				
		}			
					
		
	
	}
}

void
BeaconEvaluator::setFriend(helper::IDRPQueueInterface* QueueInterface)
{
	queueInterface = QueueInterface;
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
