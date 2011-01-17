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

#ifndef WIMEMAC_CONVERGENCE_IPHYSERVICES_HPP
#define WIMEMAC_CONVERGENCE_IPHYSERVICES_HPP

#include <WNS/service/phy/ofdma/Handler.hpp>
#include <WNS/service/phy/ofdma/Notification.hpp>
#include <WNS/service/phy/ofdma/DataTransmission.hpp>

#include <WIMEMAC/convergence/PhyModeProvider.hpp>

namespace wimemac { namespace convergence {

    /** @brief Interface for the PhyUser Services */
    class IPhyServices
    {
    public:
        virtual ~IPhyServices(){};

        /** @brief Handling of the services */
        virtual void setNotificationService(wns::service::Service* phy) = 0;
        virtual wns::service::phy::ofdma::Notification* getNotificationService() const = 0;
        virtual void setDataTransmissionService(wns::service::Service* phy) = 0;
        virtual wns::service::phy::ofdma::DataTransmission* getDataTransmissionService() const = 0;
        
        /** @brief Handling of PhyModes */
        virtual PhyModeProvider* getPhyModeProvider() = 0;
        
        /** @brief Get signal & noise power from compounds */
        virtual wns::Power getRxPower(const wns::ldk::CommandPool* commandPool) = 0;
        virtual wns::Power getInterference(const wns::ldk::CommandPool* commandPool) = 0;
    };
   

} // convergence
} // wimemac

#endif // not defined WIMEMAC_CONVERGENCE_IPHYSERVICES_HPP
