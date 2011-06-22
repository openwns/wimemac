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

#ifndef WIMEMAC_LOWERMAC_IMANAGERSERVICES_HPP
#define WIMEMAC_LOWERMAC_IMANAGERSERVICES_HPP

#include <WIMEMAC/drp/IDRPSchedulerServices.hpp>
#include <WIMEMAC/management/IBeaconBuilderServices.hpp>
#include <WIMEMAC/management/ProtocolCalculator.hpp>

#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/CommandPool.hpp>
#include <WNS/service/dll/Address.hpp>

namespace wimemac { namespace lowerMAC {

    /** @brief Interface for the Manager Services */
    class IManagerServices:
        public wimemac::drp::IDRPSchedulerServices,
        public wimemac::management::IBeaconBuilderServices
    {
    public:
        virtual ~IManagerServices(){};

        /** @brief Handling of the services */
        
        /** @brief Get the compound type */
        virtual wimemac::CompoundType
        getCompoundType(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief Set the compound type */
        virtual void
        setCompoundType(const wns::ldk::CommandPool* commandPool, const CompoundType type) = 0;
        
        /** @brief Returns the MAC address of this transceiver. Do not use the
         * similar function from the dll::UpperConvergence - this will give you
         * the MAC address of the complete Layer2 */
        virtual wns::service::dll::UnicastAddress
        getMACAddress() const = 0;
        
        /** @brief Get the PhyMode from a compound*/
        virtual wimemac::convergence::PhyMode
        getPhyMode(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief Set the PhyMode using wimemac::convergence::PhyMode */
        virtual void
        setPhyMode(const wns::ldk::CommandPool* commandPool, const wimemac::convergence::PhyMode phyMode) = 0;
        
        /** @brief Getter for the receiver address of the compound*/
        virtual wns::service::dll::UnicastAddress
        getReceiverAddress(const wns::ldk::CommandPool* commandPool) const = 0;
        
        virtual wns::simulator::Time
        getBPStartTime() = 0;
        
        virtual void
        setBPStartTime(wns::simulator::Time bpStartTime_) = 0;
        
        /** @brief Get Preamble Mode */
        virtual std::string
        getPreambleMode(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief Check if compound type is PREAMBLE */
        virtual bool
        isPreamble(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief Check if compound is a Beacon */
        virtual bool
        isBeacon(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief True if a received frame is intended for me */
        virtual bool
        isForMe(const wns::ldk::CommandPool* commandPool) const = 0;
        
        /** @brief Check if Compound has payload */
        virtual bool
        hasPayload(const wns::ldk::CommandPool* commandPool) const = 0;
        
        virtual wns::simulator::Time
        getpsduDuration(const wns::ldk::CommandPool* commandPool) const = 0;

        virtual void
        setpsduDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration) = 0;
        
        /** @brief Getter for the transmitter address of the compound*/
        virtual wns::service::dll::UnicastAddress
        getTransmitterAddress(const wns::ldk::CommandPool* commandPool) const = 0;
        
        virtual bool getDRPchannelAccess() = 0;
        virtual bool getPCAchannelAccess() = 0;
        virtual bool getRandomUse() = 0;
                
        
        virtual wns::ldk::CommandPool* createReply(const wns::ldk::CommandPool* original) const = 0;
        virtual wns::ldk::CommandPool* createReply(const wns::ldk::CommandPool* original, wimemac::CompoundType compoundType) const = 0;
        virtual wns::ldk::CompoundPtr
        createCompound(const wns::service::dll::UnicastAddress transmitterAddress,
                       const wns::service::dll::UnicastAddress receiverAddress,
                       const CompoundType compoundType,
                       const bool hasPayload,
                       const wns::simulator::Time frameExchangeDuration,
                       const bool requiresDirectReply = false) = 0; 
        
        virtual wns::simulator::Time getFrameExchangeDuration(const wns::ldk::CommandPool* commandPool) const = 0;
        virtual void setFrameExchangeDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration) = 0;
        
        virtual wimemac::management::ProtocolCalculator* getProtocolCalculator() = 0;
        
    };
   

} // lowerMAC
} // wimemac

#endif // not defined WIMEMAC_LOWERMAC_IMANAGERSERVICES_HPP



