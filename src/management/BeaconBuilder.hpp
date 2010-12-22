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
#ifndef WIMEMAC_MANAGEMENT_BEACONBUILDER_H
#define WIMEMAC_MANAGEMENT_BEACONBUILDER_H

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/lowerMAC/Manager.hpp>
#include <WIMEMAC/management/BeaconCommand.hpp>
#include <WIMEMAC/management/BeaconEvaluator.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WIMEMAC/helper/IDRPQueueInterface.hpp>
#include <vector>
#include <DLL/UpperConvergence.hpp>

namespace wimemac {
namespace management {

        /** @brief BeaconBuilder builds a evaluates beacon compounds
        * - # Building Beacons: create a new compound, set broadcast address and create beaconcommand
        * - # On reception: evaluate the received beacon compound. By envaluation a new DRPManager 
        *	is created for each new drp reservation. This DRPManager holds all necessary information about the reservation and sets the reservation timer.
        * - # BP has finished: Ask each DRPManager to set the reservation timer if drp negotation has finished successfully
        */

typedef std::vector<bool> Vector;

    class BeaconBuilder :

        public wns::ldk::fu::Plain<BeaconBuilder, BeaconCommand>,
        public BeaconEvaluator
        {
        public:

            BeaconBuilder(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ );
            ~BeaconBuilder();
            void doOnData(const wns::ldk::CompoundPtr& );
            void doSendData(const wns::ldk::CompoundPtr& );

            /** @brief If BeaconPhase is over, all received beacons could be evaluated and the upcoming reservations could be set. This is done by the BeaconEvaluator */

            void BuildDTPmap();
            bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
            void doWakeup();

            bool isBeacon(const wns::ldk::CommandPool* commandPool) const;

            void prepareDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize);
            void updateDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize);


        protected:

            virtual void onFUNCreated();

        private:
                    
            void calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;
            wns::ldk::CompoundPtr CreateBeacon();
            wns::ldk::CompoundPtr currentBeacon;
            wns::logger::Logger logger;
            wimemac::convergence::PhyMode beaconPhyMode;
            const std::string managerName;

            bool isDRPconnection;
            wns::service::dll::UnicastAddress tmpID; // For allocatedMAS probe in destructor

            wns::probe::bus::ContextCollectorPtr MASProbe;          // Number of allocated MAS
            wns::probe::bus::ContextCollectorPtr allocatedMASProbe; // Allocate MAS in text probe

            struct Friends
            {
                dll::UpperConvergence * Upper;
                wimemac::helper::IDRPQueueInterface* queueInterface;
                wns::ldk::CommandReaderInterface* keyReader;
                wimemac::lowerMAC::Manager* manager;
                
            } friends;	
    };
}
}
#endif
