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

#ifndef WIMEMAC_MANAGEMENT_BEACONEVALUATOR_H
#define WIMEMAC_MANAGEMENT_BEACONEVALUATOR_H

#include <WNS/service/dll/Address.hpp>
#include <WNS/container/Registry.hpp>
#include <WIMEMAC/management/BeaconCommand.hpp>
#include <WIMEMAC/drp/DRPmap.hpp>
#include <WIMEMAC/drp/DRPManager.hpp>
#include <WNS/logger/Logger.hpp>

#include <WIMEMAC/helper/IDRPQueueInterface.hpp>
#include <vector>

namespace wimemac { namespace drp {
    class DRPManager;
}}

namespace wimemac { namespace lowerMAC {
        class Manager;
}}

namespace wimemac { namespace management {
typedef std::vector<bool> Vector;

        /** @brief BeaconEvaluator evaluates beacon compounds. For each new DRP connection a DRPManager is created. 
    * DRPManager holds the necessary information to identify a common drp connection.
    * The queue interface represents the connection between "beacon plane (BeaconCollector, BeaconEvaluator)"
    * and "data plane (DataCollector)". This interface is used by each DRPManager to notify the data queues about
    * the beginning drp reservation for a specific drp connection.
    */

    class BeaconEvaluator
        {
        public:
            BeaconEvaluator();
            /** @brief After receiving a beacon, compound will be examined*/
            void BeaconExamination(wns::service::dll::UnicastAddress tx, wns::service::dll::UnicastAddress iam, BeaconCommand* BeaconCommand,
            wns::logger::Logger _logger);

            /** @brief register specific queue interface*/
            void setFriend(helper::IDRPQueueInterface* QueueInterface);

            /** @brief register manager*/
            void setManagerFriend(wimemac::lowerMAC::Manager* manager_);

            /** @brief register drp connection timer for each drp connection after BP has finished*/
            int CollectDRPmaps(wns::simulator::Time BPDuration);

            void CreateDRPMaps();
            void EvaluateConnection();

            void CreateDRPManager(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize, wimemac::convergence::PhyMode DefPhyMode);
            void UpdateDRPManager(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize);

            bool CreateDRPIE(BeaconCommand* BeaconCommand);
            bool CreateBPOIE(BeaconCommand* BeaconCommand);
            bool CreateProbeIE(BeaconCommand* BeaconCommand);

            /** @brief call to request an IE from the specified address */
            void RequestIE(wns::service::dll::UnicastAddress rx, BeaconCommand::ProbeElementID elementID);

            void UpdateGlobalHardDRPMap(Vector SoftDRPMap);
            void UpdateGlobalSoftDRPMap(Vector HardDRPMap);
            void UpdateGlobalDRPMap(Vector DRPMap);
            void SetLogger(wns::logger::Logger _logger);

            bool UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap);

            /** @brief Get the PhyMode for a transmission partner*/
            wimemac::convergence::PhyMode
            getPhyMode(wns::service::dll::UnicastAddress rx, int masNumber);

            void SetBPDuration(wns::simulator::Time duration);
            void evaluatePERforConnections();
            Vector getReservedMASs();



        protected:
            drp::DRPmap* DRPmapManager;
            wns::simulator::Time BPDuration;
            int NumberOfBPSlots;

            /** @brief Respond to probes with the requested IEs in the next beacons */
            struct Probes
            {
                int drpAvailability;
            } requestedProbes;

            Vector getAllocatedMASs();
            
            wns::simulator::Time BPStartTime;

        private:
          
          int calp;
            typedef wns::container::Registry<wns::service::dll::UnicastAddress, drp::DRPManager*> ContainerType;
            /** @brief DRPconnection holds all DRPManager, mac address represents a key*/
            ContainerType DRPIncomingConnections;
            ContainerType DRPOutgoingConnections;
            ContainerType::const_iterator it;

            drp::DRPManager* DRPmanager;
            Vector tmpAllocatedMAS; // For allocated MAS probe in beaconbuilder destructor
            
            
            int getBeaconSlot();
            
            void ExamineBeaconPeriodOccupancy(wns::service::dll::UnicastAddress tx, BeaconCommand* BeaconCommand);
            void InsertInNeighoursBPoccupancy(wns::service::dll::UnicastAddress tx, BeaconCommand* BeaconCommand);
            
            /** @brief Topology discovering through Beacon Period Occupancy, BPoccupancy is the device's BPOIE*/
            std::vector<wns::service::dll::UnicastAddress> BPoccupancy;
            
            struct BPallocation
            {
              BPallocation():
              source(wns::service::dll::UnicastAddress::UnicastAddress())
              {}
              
              wns::service::dll::UnicastAddress source;
              std::vector<wns::service::dll::UnicastAddress> BPoccupancy;
            };
            
            /** @brief NeighboursBPoccupancy is used as a container for all received BPOIEs of the neighbours*/
            std::vector<BPallocation> NeighboursBPoccupancy;

            wns::container::Registry<wns::service::dll::UnicastAddress, Vector> rxAvailabilityBitmap;
            /** @brief ProbeIEs */
            bool hasPendingProbe;
            bool FirstEval;
            wns::container::Registry<wns::service::dll::UnicastAddress, std::queue<BeaconCommand::ProbeElementID> > pendingProbe;
            wns::logger::Logger logger;
            helper::IDRPQueueInterface* queueInterface;

            struct Friends
            {
                helper::IDRPQueueInterface* QueueInterface;
                wimemac::lowerMAC::Manager* manager;
            } friends;

        };
}//management
}//wimemac
#endif
