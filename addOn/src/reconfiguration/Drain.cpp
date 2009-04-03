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

#include <GLUE/reconfiguration/Drain.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Drain,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.Drain",
				     FUNConfigCreator);

Drain::	Drain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	wns::Cloneable<Drain>(),
	logger(config.get<wns::pyconfig::View>("logger"))
{
} // Drain


Drain::~Drain()
{
} // ~Drain


void
Drain::doSendData(const wns::ldk::CompoundPtr&)
{
	MESSAGE_SINGLE(NORMAL, logger, "Draining outgoing compound");
} // doSendData


void
Drain::doOnData(const wns::ldk::CompoundPtr&)
{
	MESSAGE_SINGLE(NORMAL, logger, "Draining incoming compound");
} // doOnData


bool
Drain::doIsAccepting(const wns::ldk::CompoundPtr&) const
{
	MESSAGE_SINGLE(NORMAL, logger, "Draining isAccepting call");
	return false;
} // doIsAccepting


void
Drain::doWakeup()
{
	MESSAGE_SINGLE(NORMAL, logger, "Draining wakeup call");
} // doWakeup



