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

#include <GLUE/reconfiguration/CompoundBacktracker.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(CompoundBacktracker,
				     wns::ldk::FunctionalUnit,
				     "glue.CompoundBacktracker",
				     wns::ldk::FUNConfigCreator);

CompoundBacktracker::CompoundBacktracker(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	wns::ldk::Processor<CompoundBacktracker>(),
	wns::ldk::CommandTypeSpecifier<>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<CompoundBacktracker>(),

	backtraceIncoming(_config.get<bool>("backtraceIncoming")),
	backtraceOutgoing(_config.get<bool>("backtraceOutgoing")),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
} // CompoundBacktracker


CompoundBacktracker::~CompoundBacktracker()
{
} // ~CompoundBacktracker


void
CompoundBacktracker::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
	if (backtraceIncoming)
	{
		MESSAGE_BEGIN(VERBOSE, logger, m, "vvv Compound backtrace (outgoing)");
		m << compound->dumpJourney();
		MESSAGE_END();
	}
} // doSendData


void
CompoundBacktracker::processIncoming(const wns::ldk::CompoundPtr& compound)
{
	if (backtraceOutgoing)
	{
		MESSAGE_BEGIN(VERBOSE, logger, m, "^^^ Compound backtrace (incoming)");
		m << compound->dumpJourney();
		MESSAGE_END();
	}
} // doOnData

bool
CompoundBacktracker::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
	if (backtraceIncoming)
	{
		MESSAGE_BEGIN(VERBOSE, logger, m, "vvv Compound backtrace (isAccepting)");
		m << compound->dumpJourney();
		MESSAGE_END();
	}
	return getConnector()->hasAcceptor(compound);
} // doIsAccepting


