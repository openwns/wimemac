/******************************************************************************
 * Capability Discovery Provider                                              *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include "CapabilityDiscoveryProvider.hpp"

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/module/Base.hpp>

using namespace glue::mih;

STATIC_FACTORY_REGISTER_WITH_CREATOR(CapabilityDiscoveryProvider,
				     wns::ldk::FunctionalUnit,
				     "glue.mih.CapabilityDiscoveryProvider",
				     wns::ldk::FUNConfigCreator);
CapabilityDiscoveryProvider::CapabilityDiscoveryProvider(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<>(fuNet),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<CapabilityDiscoveryProvider>(),
	capabilityDiscoveryHandler(NULL),
	logger(config.get<wns::pyconfig::View>("logger")),
	myConfig(config),
	address(wns::service::dll::UnicastAddress(config.get<int>("address"))),
	dllTechnologyName(config.get<std::string>("dllTechnologyName"))

{
	MESSAGE_SINGLE(NORMAL, logger, "CapabilityDiscoveryProvider FU created ");
	linkIdentifier.technologyName = "Glue";
	linkIdentifier.source = wns::service::dll::UnicastAddress(address);
} // CapabilityDiscoveryProvider

CapabilityDiscoveryProvider::~CapabilityDiscoveryProvider()
{
}

void
CapabilityDiscoveryProvider::registerHandler(wns::service::dll::mih::CapabilityDiscoveryHandler* _capabilityDiscoveryHandler)
{
	assureNotNull(_capabilityDiscoveryHandler);
	assure(this->capabilityDiscoveryHandler == NULL, " Capability Discovery handler already set");
	this->capabilityDiscoveryHandler = _capabilityDiscoveryHandler;

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " CapabilityDiscoveryProvider got a Capability Discovery Handler in MIHF";
	MESSAGE_END();

} // registerHandler


void
CapabilityDiscoveryProvider::provideLinkEventDiscover()
{
	wns::service::dll::mih::LinkEventType linkEventType;
	std::vector<wns::service::dll::mih::LinkEventType> supportedEventsList;
	bool type;

	if(type=myConfig.get<bool>("linkUp"))
	{
		linkEventType = wns::service::dll::mih::LinkUp;

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " Type put in SupportedEvents set:";
		m << wns::service::dll::mih::LinkEventTypeToString(linkEventType);
		MESSAGE_END();

		supportedEventsList.push_back(linkEventType);
	}
	if(type=myConfig.get<bool>("linkDown"))
	{
		linkEventType = wns::service::dll::mih::LinkDown;

		MESSAGE_BEGIN(NORMAL, logger, m,getFUN()->getName());
		m << " Type put in SupportedEvents set:";
		m << wns::service::dll::mih::LinkEventTypeToString(linkEventType);
		MESSAGE_END();

		supportedEventsList.push_back(linkEventType);
	}
	if(type=myConfig.get<bool>("linkDetected"))
	{
		linkEventType = wns::service::dll::mih::LinkDetected;

		MESSAGE_BEGIN(NORMAL, logger, m,getFUN()->getName());
		m << " Type put in SupportedEvents set:";
		m << wns::service::dll::mih::LinkEventTypeToString(linkEventType);
		MESSAGE_END();

		supportedEventsList.push_back(linkEventType);
	}
	if(type=myConfig.get<bool>("linkGoingDown"))
	{
		linkEventType = wns::service::dll::mih::LinkGoingDown;

		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " Type put in SupportedEvents set:";
		m << wns::service::dll::mih::LinkEventTypeToString(linkEventType);
		MESSAGE_END();

		supportedEventsList.push_back(linkEventType);
	}
	wns::service::dll::mih::LinkEventDiscover linkEventDiscover(this->linkIdentifier, supportedEventsList);

	assure(capabilityDiscoveryHandler, "No Capability Discovery handler set");

	capabilityDiscoveryHandler->onLinkEventDiscover(linkEventDiscover);
} // provideLinkEventDiscover

wns::service::dll::UnicastAddress
CapabilityDiscoveryProvider::getAddress()
{
	return this->address;
} // getAddress

std::string
CapabilityDiscoveryProvider::getTechnologyName()
{
	return this->dllTechnologyName;
} // getTechnologyName

std::vector<std::string>
CapabilityDiscoveryProvider::getModeNames()
{
	std::vector<std::string> empty;
	return empty;
} // getModeNames

void
CapabilityDiscoveryProvider::doSendData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("sendData(...) of FunctionalUnitLight must not be called.");
} // doSendData

void
CapabilityDiscoveryProvider::doOnData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("onData(...) of FunctionalUnitLight must not be called.");
} // doOnData

bool
CapabilityDiscoveryProvider::doIsAccepting(const wns::ldk::CompoundPtr& ) const
{
	throw wns::Exception("isAccepting(...) of FunctionalUnitLight must not be called.");
	return false;

} // doIsAccepting

void
CapabilityDiscoveryProvider::doWakeup()
{
	throw wns::Exception("wakeup(...) of FunctionalUnitLight must not be called.");
} // doWakeup
