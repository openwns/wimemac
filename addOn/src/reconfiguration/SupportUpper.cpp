/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/reconfiguration/SupportUpper.hpp>
#include <GLUE/reconfiguration/Manager.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SupportUpper,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.SupportUpper",
				     wns::ldk::FUNConfigCreator);

SupportUpper::SupportUpper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<SupportUpperConnector>(),
	HasDeliverer<>(),
	wns::Cloneable<SupportUpper>(),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	friends.manager = NULL;
	getConnector()->setLogger(logger);
} // SupportUpper


SupportUpper::~SupportUpper()
{
} // ~SupportUpper


void
SupportUpper::onFUNCreated()
{
	friends.manager = getFUN()->findFriend<Manager*>(config.get<std::string>("reconfigurationManager"));
	friends.manager->setSupportUpper(this);
	getConnector()->add(getConnector()->getSimpleAddFU(), friends.manager->getCTIForSimpleAddFU());
	getConnector()->setAccepting(friends.manager->getCTIForSimpleAddFU());
} // onFUNCreated


void
SupportUpper::processOutgoing(const CompoundPtr&)
{
	MESSAGE_SINGLE(VERBOSE, logger, "Compound outgoing...");
} // processOutgoing


void
SupportUpper::processIncoming(const CompoundPtr&)
{
	MESSAGE_SINGLE(VERBOSE, logger, "Compound incoming...");
} // processIncoming



