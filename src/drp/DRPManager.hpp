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

#ifndef WIMEMAC_DRP_DRPMANAGER_H
#define WIMEMAC_DRP_DRPMANAGER_H

#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/management/BeaconCommand.hpp>

#include <boost/bind.hpp>

#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WIMEMAC/helper/IDRPQueueInterface.hpp>
#include <WIMEMAC/drp/DRPPatternCreator.hpp>

#include <WNS/logger/Logger.hpp>

#include <vector>
#include <algorithm>

namespace wimemac { namespace drp {

typedef std::vector<bool> Vector;

    class DRPManager:

        public DRPPatternCreator
        {
        public:
            DRPManager(wns::service::dll::UnicastAddress TargetAddress,
            wimemac::management::BeaconCommand::ReservationType InitialType, wimemac::management::BeaconCommand::ReasonCode InitialReason,
            wimemac::management::BeaconCommand::DeviceType InitialDevice, Vector DRPSlotMap, 
            helper::IDRPQueueInterface* QueueInterface, wns::logger::Logger logger_, wimemac::lowerMAC::Manager* manager_, 
            int numberOfBPSlots_, bool useRateAdaptation_, double patternPEROffset_, wimemac::convergence::PhyMode DefPhyMode);

            DRPManager(wns::service::dll::UnicastAddress TargetAddress,
            wimemac::management::BeaconCommand::ReservationType InitialType, wimemac::management::BeaconCommand::ReasonCode InitialReason,
            wimemac::management::BeaconCommand::DeviceType InitialDevice, Vector DRPSlotMap, helper::IDRPQueueInterface* QueueInterface, wns::logger::Logger logger_, wimemac::lowerMAC::Manager* manager_, int numberOfBPSlots_, bool useRateAdaptation_, double patternPEROffset_);

            wns::service::dll::UnicastAddress GetAddress();

            void SetReservationType(wimemac::management::BeaconCommand::ReservationType);
            wimemac::management::BeaconCommand::ReservationType GetReservationType();

            void SetReasonCode(wimemac::management::BeaconCommand::ReasonCode);
            wimemac::management::BeaconCommand::ReasonCode GetReasonCode();

            void SetMergeReasonCode(wimemac::management::BeaconCommand::ReasonCode);
            wimemac::management::BeaconCommand::ReasonCode GetMergeReasonCode();

            void SetDeviceType(wimemac::management::BeaconCommand::DeviceType deviceType);
            wimemac::management::BeaconCommand::DeviceType GetDeviceType();

            bool areMASsAvailable(Vector DRPGlobal);
            
            void FindNewPattern(Vector DRPGlobal);
            void CreateAdditionalPattern(Vector DRPGlobal);
            
            void SetPattern(Vector DRPreservation);
            Vector GetPattern();
            void SetMergePattern(Vector DRPreservation);
            Vector GetMergePattern();
            
            void ResolveConflict();
            void ResolveMergeConflict();

            void RegisterReservation(wns::simulator::Time BPDuration);
            void StartRegisterReservation(wns::simulator::Time BPDuration);

            void SetStatus(bool ConnectionStatus);
            bool GetStatus();
            void SetMergeStatus(bool ConnectionStatus);
            bool GetMergeStatus();

            void AddReservation(wimemac::management::BeaconCommand::ReasonCode mergeReasonCode_, Vector mergeMap_);
            void MergePatterns();

            bool IsMapCreated();
            bool NeedsAdditionalPattern();
            bool HasPendingDRPMerge();
            void Transmit(wns::simulator::Time duration);

            void UpdatePhyModeDown(wimemac::convergence::PhyMode phyMode_);

        private:

            enum wimemac::management::BeaconCommand::ReservationType reservationtype;
            enum wimemac::management::BeaconCommand::ReasonCode reasoncode;
            enum wimemac::management::BeaconCommand::ReasonCode mergereasoncode;
            enum wimemac::management::BeaconCommand::DeviceType devicetype;

            bool connectionstatus;
            bool mergeconnectionstatus;
            bool needsAdditionalPattern;
            bool hasPendingDRPMerge;
            int waitSFsForNewAvailIE;

            const bool useRateAdaptation;

            Vector DRPAllocMap;
            Vector tmpDRPAllocMap;
            Vector tmpDRPMergeMap;
            Vector::iterator it;
            wns::service::dll::UnicastAddress peerAddress;
            wns::simulator::Time duration;
            wns::simulator::Time ReservationStart;

            wns::events::scheduler::Interface* scheduler;
            wns::events::scheduler::Callable call;

            wns::logger::Logger logger;
            wns::service::dll::UnicastAddress test;

            helper::IDRPQueueInterface* QueueInterface;

        };
}//drp
}//wimemac
#endif
