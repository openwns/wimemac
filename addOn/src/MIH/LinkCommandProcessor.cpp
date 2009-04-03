#include "LinkCommandProcessor.hpp"

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>

using namespace glue::mih;

STATIC_FACTORY_REGISTER_WITH_CREATOR(LinkCommandProcessor,
				     wns::ldk::FunctionalUnit,
				     "glue.mih.LinkCommandProcessor",
				     wns::ldk::FUNConfigCreator);

LinkCommandProcessor::LinkCommandProcessor(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
		wns::ldk::CommandTypeSpecifier<>(fuNet),
		wns::ldk::HasReceptor<>(),
		wns::ldk::HasConnector<>(),
		wns::ldk::HasDeliverer<>(),
		wns::Cloneable<LinkCommandProcessor>(),
		logger(config.get<wns::pyconfig::View>("logger"))
{
	MESSAGE_SINGLE(NORMAL, logger, "LinkCommandProcessor FU created ");
} // LinkCommandProcessor

LinkCommandProcessor::~LinkCommandProcessor()
{
}

void
LinkCommandProcessor::linkConnect(const wns::service::dll::mih::ConnectionIdentifier&)
{
}//linkConnect

void
LinkCommandProcessor::linkDisconnect(const wns::service::dll::mih::ConnectionIdentifier&)
{
}// linkDisconnect

wns::service::dll::mih::ScanResponseSet
LinkCommandProcessor::scanAllLinks()
{
	wns::service::dll::mih::ScanResponseSet mySet;
	return mySet;
} //scanLink

wns::service::dll::mih::ScanResponseSet
LinkCommandProcessor::scanLink(const wns::service::dll::mih::LinkIdentifier&)
{
	wns::service::dll::mih::ScanResponseSet scanResponseSet;
	return scanResponseSet; 
} //scanLink

void
LinkCommandProcessor::configureThresholds(const wns::service::dll::mih::ConnectionIdentifier&)
{
}// configureThresholds

void
LinkCommandProcessor::onFUNCreated()
{
}  // onFUNCreated
void
LinkCommandProcessor::doSendData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("sendData(...) of FunctionalUnitLight must not be called.");
} // doSendData

void
LinkCommandProcessor::doOnData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("onData(...) of FunctionalUnitLight must not be called.");
} // doOnData

bool
LinkCommandProcessor::doIsAccepting(const wns::ldk::CompoundPtr& ) const
{
	throw wns::Exception("isAccepting(...) of FunctionalUnitLight must not be called.");
	return false;

} // doIsAccepting

void
LinkCommandProcessor::doWakeup()
{
	throw wns::Exception("wakeup(...) of FunctionalUnitLight must not be called.");
} // doWakeup
