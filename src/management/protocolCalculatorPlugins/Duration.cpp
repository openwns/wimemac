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

#include <WIMEMAC/management/protocolCalculatorPlugins/Duration.hpp>
#include <cmath>

using namespace wimemac::management::protocolCalculatorPlugins;

Duration::Duration( FrameLength* fl_, const wns::pyconfig::View& config):
    tsym(config.get<wns::simulator::Time>("tsym")),
    fl(fl_)
{

}

wns::simulator::Time
Duration::PPDU(Bit psduLength, const wimemac::convergence::PhyMode& pm) const
{
    return (preamble(pm) + psduLength / pm.getInfoBitsPer6Symbols() * 6 * tsym);
}

wns::simulator::Time
Duration::PSDU(Bit psduLength, const wimemac::convergence::PhyMode& pm) const
{
    return (psduLength / pm.getInfoBitsPer6Symbols() * 6 * tsym);
}

wns::simulator::Time
Duration::preamble(const wimemac::convergence::PhyMode& pm) const
{
    if(pm.getPreambleMode() == "STANDARD")
    {
        // Standard Preamble transmission time + PCLP Header transmission time
        return (9.375e-6 + 3.75e-6);
    }

    if(pm.getPreambleMode() == "BURST")
    {
        // Burst Preamble transmission time + PCLP Header transmission time
        return (5.625e-6 + 3.75e-6);
    }

    assure(pm.getPreambleMode() == "STANDARD" or pm.getPreambleMode() == "BURST", "Unknown plcpMode : " << pm.getPreambleMode());

    return(1);
}

wns::simulator::Time
Duration::MSDU_PPDU(Bit msduFrameSize, const wimemac::convergence::PhyMode& pm) const
{
    return(PPDU(fl->getPSDU(msduFrameSize, pm.getInfoBitsPer6Symbols()), pm));
}

wns::simulator::Time
Duration::MSDU_PSDU(Bit msduFrameSize, const wimemac::convergence::PhyMode& pm) const
{
    return(PSDU(fl->getPSDU(msduFrameSize, pm.getInfoBitsPer6Symbols()), pm));
}

wns::simulator::Time
Duration::PSDU_PPDU(Bit psduFrameSize, const wimemac::convergence::PhyMode& pm) const
{
    return(PPDU(psduFrameSize, pm));
}
