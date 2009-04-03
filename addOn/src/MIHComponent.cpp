/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/MIHComponent.hpp>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	MIHComponent,
	wns::node::component::Interface,
	"glue.MIHComponent",
	wns::node::component::ConfigCreator
	);

MIHComponent::MIHComponent(
	wns::node::Interface* _node,
	const wns::pyconfig::View& _config) :

	glue::Component( _node, _config),
	measurementsMonitor(NULL),
	linkCommandProcessor(NULL)
{
}

void
MIHComponent::doStartup()
{
	// first call the startup of the the super class
	glue::Component::doStartup();

	wns::pyconfig::View _config = this->getConfig();

	measurementsMonitor =
		dynamic_cast<glue::mih::MeasurementsMonitor*>(
			fun->getFunctionalUnit(
				_config.get<std::string>("measurementsMonitor.commandName")));
	linkCommandProcessor =
		dynamic_cast<glue::mih::LinkCommandProcessor*>(
			fun->getFunctionalUnit(
				_config.get<std::string>("linkCommandProcessor.commandName")));

	capabilityDiscoveryProvider =
		dynamic_cast<glue::mih::CapabilityDiscoveryProvider*>(
			fun->getFunctionalUnit(
				_config.get<std::string>("capabilityDiscoveryProvider.commandName")));

	addService(getConfig().get<std::string>("linkEventNotification"), measurementsMonitor);
	addService(getConfig().get<std::string>("linkCommandProcessorService"), linkCommandProcessor);
	addService(getConfig().get<std::string>("capabilityDiscoveryService"), capabilityDiscoveryProvider);
}
