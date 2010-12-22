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
#ifndef WIMEMAC_MANAGEMENT_BEACONCOMMAND_H
#define WIMEMAC_MANAGEMENT_BEACONCOMMAND_H
#include <WNS/ldk/Command.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/PowerRatio.hpp>
#include <vector>
#include <queue>

namespace wimemac {

namespace convergence
{
    class MCS;
}

namespace management {


/** @brief BeaconCommand builds a evaluates beacon compounds
    * - # ReservationType: Hard or Soft DRP reservation
    * - # ReasonCode: DRP reservation Denied, Accept, Conflict or Pending
    * - # DRPAlloc: the DRP allocation vector for own reservation
    * - # DeviceType: Reservation Target or Reservation Owner
    */

typedef std::vector<bool> Vector;

    class BeaconCommand:
        public wns::ldk::Command
    {
    public:

            BeaconCommand()
            {
                peer.HasDRPIE = false;
                peer.HasAvailabilityIE = false;
                //default value is 1, as at least one device is a member of a beacon group
                peer.BPOIE.BPsize = 0; 
                
            }


            enum ReservationType {
                Hard, Soft
            };

            enum ReasonCode{
            Accept, Conflict, Pending, Denied, Modified
            };

            enum DeviceType {
            Target, Owner
            };

            enum ProbeElementID {
            DRPAvailability
            };
            
                      
            struct DRP
            {
                enum ReservationType reservationtype;
                enum ReasonCode reasoncode;
                enum DeviceType devicetype;
                bool status;
                wns::service::dll::UnicastAddress address;
                Vector DRPAlloc;
                bool isAdditionalDRPIE;
            };
            
            struct BPO
            {
                int BPsize;
                std::vector<wns::service::dll::UnicastAddress> IE;
                
            };


            struct{} local;

            struct{
                BPO BPOIE;
                bool HasDRPIE;
                bool HasAvailabilityIE;
                
                std::queue<DRP> drp;
                Vector availabilityBitmap;
                wns::container::Registry<wns::service::dll::UnicastAddress, std::queue<ProbeElementID> > probe;
            } peer;

            struct{} magic;

            bool HasDRPIE()
            {
                return(peer.drp.size()!=0);
            }

            bool HasAvailabilityIE()
            {
                return(peer.HasAvailabilityIE);
            }

            bool HasProbeIE(wns::service::dll::UnicastAddress address)
            {
                return(peer.probe.knows(address));
            }

            bool HasProbeIE()
            {
                return(peer.probe.size() != 0);
            }
            DRP GetDRPIE()
            {
                DRP DRPIECommand;
                DRPIECommand = peer.drp.front();
                peer.drp.pop();
                return(DRPIECommand);
            }
            void PutDRPIE(DRP drp)
            {
                peer.drp.push(drp);
            }

            int GetDRPIESize()
            {
                return(peer.drp.size());
            }

        private:

    };
}

}
#endif
