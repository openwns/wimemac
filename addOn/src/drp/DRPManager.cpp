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

#include <WIMEMAC/drp/DRPManager.hpp>

using namespace wimemac::drp;
using wimemac::frame::BeaconCommand;


DRPManager::DRPManager(wns::service::dll::UnicastAddress TargetAddress,BeaconCommand::ReservationType InitialType, 
			BeaconCommand::ReasonCode InitialReason, BeaconCommand::DeviceType InitialDevice,Vector DRPSlotMap, 
			helper::IDRPQueueInterface* _QueueInterface):
			
			reasoncode(InitialReason),
			reservationtype(InitialType),
			devicetype(InitialDevice),
			peerAddress(TargetAddress),
			DRPAllocMap(DRPSlotMap),
			QueueInterface(_QueueInterface)			
{	
	scheduler = wns::simulator::getEventScheduler();
}


void DRPManager::SetAddress(wns::service::dll::UnicastAddress address)
{
	//peerAddress = address;
}

wns::service::dll::UnicastAddress DRPManager::GetAddress()
{
	return peerAddress;	
}

void 
DRPManager::SetReservationType(BeaconCommand::ReservationType DRPResType)
{
	reservationtype = DRPResType;

}

BeaconCommand::ReservationType
DRPManager::GetReservationType()
{
	return reservationtype;
}

void 
DRPManager::SetReasonCode(BeaconCommand::ReasonCode reason)
{
	reasoncode = reason;

}

BeaconCommand::ReasonCode
DRPManager::GetReasonCode()
{
	return reasoncode;
}

void 
DRPManager::FindNewPattern()
{
}
void 
DRPManager::SetPattern(Vector DRPreservation)
{
	DRPAllocMap = DRPreservation;
}
Vector
DRPManager::GetPattern()
{
	return DRPAllocMap;
}


void 
DRPManager::StartRegisterReservation()
{
	//assure(false,"BeaconCollector: received compound although not in receiving mode!");
	tmpDRPAllocMap = DRPAllocMap;
	RegisterReservation();
}

void
DRPManager::RegisterReservation()
{
  wns::logger::Logger log("tests", "SchedulerBestPractices");

			
	int adjacent = 0;
	double MASduration = 256E-6;
	int i = 0;

	while(i < tmpDRPAllocMap.size())
	{
		
		if(tmpDRPAllocMap[i] == true)
		{
			if(adjacent == 0)
			{
				ReservationStart = i * MASduration;
			}

			tmpDRPAllocMap[i] = false;
			adjacent++;
		}
		else
		{
			if(adjacent != 0)
			{
				break;	
			}
		}
		i++;		
	}

	if(adjacent != 0)
	{
		duration = adjacent * MASduration;
		adjacent = 0;
		
		wns::events::scheduler::Callable call = boost::bind(&DRPManager::Transmit, this, duration);
		scheduler->scheduleDelay(call, ReservationStart);


		MESSAGE_BEGIN(NORMAL, log, m, "");
		m << "Set reservation timer "
		<<" Start Time: " << ReservationStart
		<<" Duration: " << duration;
    		MESSAGE_END();
	
	}
	else
	{
		MESSAGE_BEGIN(NORMAL, log, m, "");
		m << "There are no more MAS reserved in this superframe";
    		MESSAGE_END();

	}

}


void
DRPManager::Transmit(wns::simulator::Time duration)
{ wns::logger::Logger log("tests", "SchedulerBestPractices");
	
		MESSAGE_BEGIN(NORMAL, log, m, "");
		m << "It's time to transmit for mac " << peerAddress;
    		MESSAGE_END();
	
	//its time to transmit, signal
	QueueInterface->TimeToTransmit(peerAddress, duration);
	//setNewTimeout(duration);
	
	
}

void
DRPManager::onTimeout()
{
	//reservation has finished now
	RegisterReservation();
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
