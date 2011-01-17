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

#ifndef WIMEMAC_MANAGEMENT_PROTOCOLCALCULATORPLUGINS_FRAMELENGTH_HPP
#define WIMEMAC_MANAGEMENT_PROTOCOLCALCULATORPLUGINS_FRAMELENGTH_HPP

// must be the first include!
#include <WIMEMAC/FrameType.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/simulator/Bit.hpp>

namespace wimemac { namespace management { namespace protocolCalculatorPlugins {
	/** 
     * @brief plugin to determine PSDU sizes for different MAC data units
     *
     * this class calculates the resulting PSDU length in bits for
     * MSDUs, A-MPDUs, A-MSDUs
     */
    class FrameLength
    {
    public:
        /// @brief Constructor with python config
        FrameLength( const wns::pyconfig::View& config );

        virtual ~FrameLength() {};

        /// @brief returns size of the resulting PSDU
        Bit getPSDU(Bit msduFrameSize, int nIBP6S) const;

        /// @brief returns size of the Preamble
        Bit getPreamble(std::string pm) const;

		/// @brief calculates Pad Bits number depending on nIBP6S and msduFrameSize
        Bit pad(Bit msduFrameSize, int nIBP6S) const;

        const Bit FCS;
        const Bit tail;

    };
} // protocolCalculatorPlugins
} // management
} // wimemac

#endif
