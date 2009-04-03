/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WIMEMAC/WIMEMAC.hpp>

using namespace wimemac;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	WiMeMAC,
	wns::module::Base,
	"WIMEMAC",
	wns::PyConfigViewCreator);


WiMeMAC::WiMeMAC(const wns::pyconfig::View& _pyco) :
	wns::module::Module<WiMeMAC>(_pyco)
{
}

WiMeMAC::~WiMeMAC()
{
}

void WiMeMAC::configure()
{
}

void WiMeMAC::startUp()
{
}

void WiMeMAC::shutDown()
{
}


