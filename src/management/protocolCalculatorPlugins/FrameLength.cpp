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

#include <WIMEMAC/management/protocolCalculatorPlugins/FrameLength.hpp>
#include <math.h>

using namespace wimemac::management::protocolCalculatorPlugins;

FrameLength::FrameLength(const wns::pyconfig::View& config):
	tail(config.get<Bit>("tail")),
	FCS(config.get<Bit>("FCS"))
{

}

Bit
FrameLength::getPSDU(Bit msduFrameSize, int nIBP6S) const
{
    return (msduFrameSize + this->FCS + this->tail + pad(msduFrameSize, nIBP6S));
}

Bit
FrameLength::getPreamble(std::string pm) const
{
    assure((pm == "STANDARD" || pm == "BURST"), "got an undefined plcpMode");
    if (pm == "STANDARD") return 700;
    else if (pm == "BURST") return 500;
}

Bit
FrameLength::pad(Bit msduFrameSize, int nIBP6S) const
{
	double nFrame = 6 * ceil( double(msduFrameSize + this->FCS + this->tail)  / nIBP6S);
	double nPad = nFrame / 6 * nIBP6S - msduFrameSize - this->FCS - this->tail;
	return (nPad);
}
