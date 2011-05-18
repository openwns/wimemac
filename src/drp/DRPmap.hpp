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

#ifndef WIMEMAC_DRPmap_H
#define WIMEMAC_DRPmap_H
#include <WNS/logger/Logger.hpp>
#include <vector>
#include <WNS/service/dll/Address.hpp>

#include <WNS/container/Registry.hpp>



namespace wimemac {
namespace drp {
typedef std::vector<bool> Vector;
typedef wns::container::Registry<wns::service::dll::UnicastAddress, Vector > MASsperStation;

    class DRPmap
    {
        public:
            DRPmap(int gMASpSF);
            void UpdateHardDRPmap(Vector, wns::logger::Logger _logger);
            void UpdateSoftDRPmap(Vector, wns::logger::Logger _logger);
            void UpdateDRPmap(Vector UpdateMap, Vector& UpdatedMap);
            
            void UpdatePendingDRPMap(wns::service::dll::UnicastAddress, wns::service::dll::UnicastAddress, Vector);
            void ReleasePendingDRPMap(wns::service::dll::UnicastAddress, wns::service::dll::UnicastAddress);
            
            bool PossiblePattern(Vector CompareDRPMap);
            void GetPattern(wns::logger::Logger _logger);
            void GetGlobalPattern(Vector& GlobalPattern);
            Vector GetGlobalHardDRPmap();
            void SetLogger(wns::logger::Logger _logger);
            
            void setBPSlots(int numberOfBPSlots_);

            // Start of a new BP -> clear globalDRPMaps from slots not announced during the last mMaxLostBeacons beacons
            void onBPStarted();

        private:
            Vector GetPendingDRPmap();
            
            std::deque<Vector> globalHardDRPmapVec;
            std::deque<Vector> globalSoftDRPmapVec;
            std::deque<Vector> globalEraseDRPmapVec;
            std::deque<wns::container::Registry<wns::service::dll::UnicastAddress, MASsperStation > > PendingDRPMapVec;
            

            int mMaxLostBeacons;
            int numberOfBPSlots;
            bool isPatternValid;
          
            Vector globalHardDRPmap;
            Vector globalSoftDRPmap;
            Vector globalEraseDRPmap;
            wns::container::Registry<wns::service::dll::UnicastAddress, MASsperStation > PendingDRPMap;
            
            wns::logger::Logger logger;
        };
}
}
#endif

