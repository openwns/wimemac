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

#include <WIMEMAC--MAIN--0.1/WiMeMAC--main--0.1Module.hpp>

using namespace wimemac--main--0.1;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	WiMeMAC--main--0.1Module,
	wns::module::Base,
	"wimemac--main--0.1",
	wns::PyConfigViewCreator);

WiMeMAC--main--0.1Module::WiMeMAC--main--0.1Module(const wns::pyconfig::View& _pyco) :
	wns::module::Module<WiMeMAC--main--0.1Module>(_pyco)
{
}

WiMeMAC--main--0.1Module::~WiMeMAC--main--0.1Module()
{
}

void WiMeMAC--main--0.1Module::configure()
{
}

void WiMeMAC--main--0.1Module::startUp()
{
}

void WiMeMAC--main--0.1Module::shutDown()
{
}
