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

#ifndef WIMEMAC_HELPER_FILTERTXOPTYPE_HPP
#define WIMEMAC_HELPER_FILTERTXOPTYPE_HPP

#include <WIMEMAC/lowerMAC/Manager.hpp>
#include <DLL/compoundSwitch/Filter.hpp>

namespace wimemac { namespace helper {

    /** @brief Filters compounds by the frame type (which is configurable), to
     * be used by the compound switch */
    class FilterTxOPType:
        public dll::compoundSwitch::Filter
    {
    public:
        FilterTxOPType(dll::compoundSwitch::CompoundSwitch* compoundSwitch, wns::pyconfig::View& config);
        ~FilterTxOPType();

        virtual void
        onFUNCreated();

        virtual bool
        filter(const wns::ldk::CompoundPtr& compound) const;

    private:
        wimemac::CompoundType acceptingTxOPType;
        wns::ldk::CommandReaderInterface* commandReader;
    };
} // Helper
} // WiMeMac

#endif
