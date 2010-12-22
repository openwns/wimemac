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
#ifndef WIMEMAC_HELPER_IQUEUEINTERFACE_H
#define WIMEMAC_HELPER_IQUEUEINTERFACE_H

#include <WNS/service/dll/Address.hpp>
#include <WNS/simulator/Time.hpp>
#include <WIMEMAC/management/BeaconBuilder.hpp>

namespace wimemac { namespace management {
         class BeaconBuilder;
}}

namespace wimemac {namespace helper {

class IQueueInterface
{
	public:
	virtual	~IQueueInterface() {};
	
	virtual bool 
	isAccepting(const wns::ldk::CompoundPtr& compound) const = 0;

    virtual
    void put(const wns::ldk::CompoundPtr& compound) = 0;

	virtual 
	wns::ldk::CompoundPtr getHeadOfLinePDU(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	int getHeadOfLinePDUbits(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	bool hasQueue(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	bool queueHasPDUs(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	void setFriend(wimemac::management::BeaconBuilder* bb) = 0;

	virtual 
	std::string printAllQueues() = 0;

};


}//helper
}//wimemac


#endif
