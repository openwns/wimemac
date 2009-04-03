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

#include <GLUE/reconfiguration/SupportUpperConnector.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

SupportUpperConnector::SupportUpperConnector() :
	fus(),
	simpleAddSealed(false),
	simpleAddFU(NULL),
	currentConnectorFU(NULL),
	logger("GLUE", "noLogger")
{
}


void
SupportUpperConnector::add(FunctionalUnit* it)
{
	assure(!simpleAddSealed, "SupportUpperConnector::add(FunctionalUnit* it) may only be called on initial FUN setup.");
	simpleAddSealed = true;
	simpleAddFU = it;
} // add


FunctionalUnit*
SupportUpperConnector::getSimpleAddFU() const
{
	return simpleAddFU;
} // getSimpleAddFU


void
SupportUpperConnector::add(FunctionalUnit* it, int cti)
{
	assure(fus.size() < 3, "Not more than 2 FU paths may be connected to SupportUpperFU the same time.");

	MESSAGE_BEGIN(VERBOSE, logger, m, "Connector: Adding FU path");
	m << " with cti " << cti;
	MESSAGE_END();

	fus.insert(cti, it);
} // add


wns::ldk::FunctionalUnit*
SupportUpperConnector::getFU(int cti)
{
	assure(fus.knows(cti), "Unknown cti");

	return fus.find(cti);
} // getFU


void
SupportUpperConnector::remove(int cti)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "Connector: Removing FU path");
	m << " with cti " << cti;
	MESSAGE_END();

	fus.erase(cti);
} // remove


void
SupportUpperConnector::setAccepting(int cti)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "Connector: Setting FU path");
	m << " with cti " << cti << " accepting";
	MESSAGE_END();

	currentConnectorFU = fus.find(cti);
} // setAccepting


void
SupportUpperConnector::setNotAccepting()
{
	MESSAGE_SINGLE(VERBOSE, logger, "Connector: Setting all FU paths not accepting");

	currentConnectorFU = NULL;
} // setNotAccepting


void
SupportUpperConnector::clear()
{
	assure(false, "Clear not supported");
} // clear


uint32_t
SupportUpperConnector::size() const
{
	return fus.size();
} // size


const Link::ExchangeContainer
SupportUpperConnector::get() const
{
	assure(false, "Exchange not supported.");
	return wns::ldk::Link::ExchangeContainer();
} // get


void
SupportUpperConnector::set(const Link::ExchangeContainer&)
{
	assure(false, "Exchange not supported.");
} // set


bool
SupportUpperConnector::hasAcceptor(const CompoundPtr& compound)
{
	if (!currentConnectorFU)
		return false;

	assure(currentConnectorFU, "hasAcceptor called, but current path has not been set yet.");
	return currentConnectorFU->isAccepting(compound);
} // hasAcceptor


FunctionalUnit*
SupportUpperConnector::getAcceptor(const CompoundPtr& compound)
{
	assure(hasAcceptor(compound), "getAcceptor called, but there is no valid candidate.");
	return currentConnectorFU;
} // getAcceptor


void
SupportUpperConnector::setLogger(const wns::logger::Logger& _logger)
{
	logger = _logger;
} // setLogger



