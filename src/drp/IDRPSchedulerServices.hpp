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

#ifndef WIMEMAC_DRP_IDRPSCHEDULERSERVICES_HPP
#define WIMEMAC_DRP_IDRPSCHEDULERSERVICES_HPP

#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/simulator/Time.hpp>

namespace wimemac { namespace drp {

    typedef std::vector<bool> Vector;
  
    /** @brief Interface for the DRPScheduler Services */
    class IDRPSchedulerServices
    {
    public:
        virtual ~IDRPSchedulerServices(){};

        /** @brief Handling of the services */
        
        /** @brief invokes a PCA transmission */
        virtual bool startPCAtransmission() = 0;
        virtual void stopPCAtransmission() = 0;
        
        virtual void txOPCloseIn(wns::simulator::Time duration) = 0;
        
        virtual wns::service::dll::UnicastAddress getCurrentTransmissionTarget() = 0;
        
         /** @brief Updates a DRPMap with the non available slots for a specified address */
        virtual bool UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap) = 0;
        
        /** @brief Returns true if the PER is above the limit and the PhyMode should be set down */
        virtual bool adjustMCSdown(wns::service::dll::UnicastAddress rx) = 0;
        
        /** @brief Updates the map of DRP reservations */
        virtual void UpdateDRPMap(Vector DRPMap) = 0;
        
        virtual void onBPStart(wns::simulator::Time BPduration) = 0;
        
        virtual void Acknowledgment(wns::service::dll::UnicastAddress tx) = 0;
        
        /** @brief get the number of retransmissions for the specified compound */
        virtual int getNumOfRetransmissions(const wns::ldk::CompoundPtr& compound) = 0;
                      
    };
   

} // drp
} // wimemac

#endif // not defined WIMEMAC_DRP_IDRPSCHEDULERSERVICES_HPP



