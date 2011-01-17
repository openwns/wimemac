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

#ifndef WIMEMAC_MANAGEMENT_PROTOCOLCALCULATORPLUGINS_DURATION_HPP
#define WIMEMAC_MANAGEMENT_PROTOCOLCALCULATORPLUGINS_DURATION_HPP

#include <WIMEMAC/management/protocolCalculatorPlugins/FrameLength.hpp>
#include <WIMEMAC/convergence/PhyMode.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/simulator/Time.hpp>


namespace wimemac { namespace management {
        class ProtocolCalculator;
}}

namespace wimemac { namespace management { namespace protocolCalculatorPlugins {
    /**
     * @brief Class to determine transmission duration for different PDUs
     *
     * this class calculates the actual duration of different PDUs under given
     * phy parameters like data bits per symbol, number of streams etc
     * the (A)M(S/P)DU duration functions expect the proper size(s) of the PDU(s)
     * e.g. a vector with frame sizes for an aggregated MPDU
     */
    class Duration
    {
    public:
        Duration( wimemac::management::protocolCalculatorPlugins::FrameLength* fl_, const wns::pyconfig::View& config );

        virtual ~Duration() {};

        wns::simulator::Time
        PPDU(Bit psduLength, const wimemac::convergence::PhyMode& pm) const;

        wns::simulator::Time
        PSDU(Bit psduLength, const wimemac::convergence::PhyMode& pm) const;

        wns::simulator::Time
        preamble(const wimemac::convergence::PhyMode& pm) const;

        wns::simulator::Time
        MSDU_PPDU(Bit msduFrameSize, const wimemac::convergence::PhyMode& pm) const;

        wns::simulator::Time
        MSDU_PSDU(Bit msduFrameSize, const wimemac::convergence::PhyMode& pm) const;

        wns::simulator::Time
        PSDU_PPDU(Bit psduFrameSize, const wimemac::convergence::PhyMode& pm) const;

        const wns::simulator::Time tsym;

    private:
        const wimemac::management::protocolCalculatorPlugins::FrameLength* fl;


    };
} // protocolCalculatorPlugins
} // management
} // wimemac

#endif
