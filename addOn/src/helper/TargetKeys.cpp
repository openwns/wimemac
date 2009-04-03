/******************************************************************************
 * WiMeMAC                                                                    *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2009                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                   *
 * email: hor@comnets.de, www: http://wns.comnets.rwth-aachen.de/             *
 *****************************************************************************/
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
