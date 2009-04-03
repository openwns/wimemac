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

#ifndef WIMEMAC_DRP_DRPMANAGER_H
#define WIMEMAC_DRP_DRPMANAGER_H
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/frame/BeaconCommand.hpp>

#include <boost/bind.hpp>

#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WIMEMAC/helper/IDRPQueueInterface.hpp>

#include <WNS/logger/Logger.hpp>

#include <vector>
#include <algorithm>

using wimemac::frame::BeaconCommand;

namespace wimemac {
namespace drp {
typedef std::vector<bool> Vector;

	class DRPManager:
		BeaconCommand::BeaconCommand,
		public wns::events::CanTimeout
		{
		public:
			DRPManager(wns::service::dll::UnicastAddress TargetAddress, 
			BeaconCommand::ReservationType InitialType, BeaconCommand::ReasonCode InitialReason, 
			BeaconCommand::DeviceType InitialDevice, Vector DRPSlotMap, helper::IDRPQueueInterface* QueueInterface);

			void SetAddress(wns::service::dll::UnicastAddress address);
			wns::service::dll::UnicastAddress GetAddress();

			void SetReservationType(BeaconCommand::ReservationType);
			BeaconCommand::ReservationType GetReservationType();

			void SetReasonCode(BeaconCommand::ReasonCode);
			BeaconCommand::ReasonCode GetReasonCode();

			void FindNewPattern();
			void SetPattern(Vector DRPreservation);
			Vector GetPattern();

			void RegisterReservation();
			void StartRegisterReservation();

			void Transmit(wns::simulator::Time duration);	
			
			virtual void onTimeout();
			


			
		private:
			
			
			enum BeaconCommand::ReservationType reservationtype;
			enum BeaconCommand::ReasonCode reasoncode;
			enum BeaconCommand::DeviceType devicetype;

			Vector DRPAllocMap;
			Vector tmpDRPAllocMap;
			Vector::iterator it;
			wns::service::dll::UnicastAddress peerAddress;
			wns::simulator::Time duration;
			wns::simulator::Time ReservationStart;

			wns::events::scheduler::Interface* scheduler;
			wns::events::scheduler::Callable call;
			
			wns::logger::Logger logger_;
			wns::service::dll::UnicastAddress test;

			helper::IDRPQueueInterface* QueueInterface;

			
		};
}
}
#endif

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
