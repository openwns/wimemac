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
#ifndef WIMEMAC_HELPER_TARGETKEYS_H
#define WIMEMAC_HELPER_TARGETKEYS_H


#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>

#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/convergence/Upper.hpp>

namespace wimemac {namespace helper {



class MACIDKeyBuilder;

class TargetMACKey:
	public wns::ldk::Key
{
	public:
		TargetMACKey(const MACIDKeyBuilder* factory, const wns::ldk::CompoundPtr& compound, int);
		
		bool operator<(const wns::ldk::Key& other) const;
		std::string str() const;

		wns::service::dll::UnicastAddress rx;
		wns::service::dll::UnicastAddress tx;
};

class MACIDKeyBuilder:
	public wns::ldk::KeyBuilder
{
	public:
		MACIDKeyBuilder(const wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual void onFUNCreated();
		virtual wns::ldk::ConstKeyPtr operator() (const wns::ldk::CompoundPtr& compound, int direction) const;

		const wns::ldk::fun::FUN* fun;
		struct Friends {
			wns::ldk::CommandReaderInterface* keyReader;
		} friends;
};
}
}


#endif
