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
 
#include <WIMEMAC/helper/TargetKeys.hpp>

using namespace wimemac::helper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	wimemac::helper::MACIDKeyBuilder,
	wns::ldk::KeyBuilder,	
	"wimemac.helper.MACIDKeyBuilder",
	wns::ldk::FUNConfigCreator );

TargetMACKey::TargetMACKey(const MACIDKeyBuilder* factory, const wns::ldk::CompoundPtr& compound, int)
{
	wns::ldk::CommandPool* commandPool = compound->getCommandPool();
	wimemac::convergence::UnicastUpperCommand* unicastcommand =
	factory->friends.keyReader->readCommand<wimemac::convergence::UnicastUpperCommand>(commandPool);
	
	
	rx = unicastcommand->peer.targetMACAddress;
}


bool
TargetMACKey::operator<(const wns::ldk::Key& _other) const
{
	
	assureType(&_other, const TargetMACKey*);
        const TargetMACKey* other = static_cast<const TargetMACKey*>(&_other);
	if(rx == other->rx)
		return rx < other->rx;
}

std::string
TargetMACKey::str() const
{
	std::stringstream ss;
	ss << "MAC Address: " << rx;
	return ss.str();
}


MACIDKeyBuilder::MACIDKeyBuilder(const wns::ldk::fun::FUN* _fun, const wns::pyconfig::View&):
	fun(_fun)
{
}
void
MACIDKeyBuilder::onFUNCreated()
{
	friends.keyReader = fun->getProxy()->getCommandReader("unicastUpperConvergence");
}


wns::ldk::ConstKeyPtr
MACIDKeyBuilder::operator() (const wns::ldk::CompoundPtr& compound, int direction) const
{
	return wns::ldk::ConstKeyPtr(new TargetMACKey(this, compound, direction));
}
