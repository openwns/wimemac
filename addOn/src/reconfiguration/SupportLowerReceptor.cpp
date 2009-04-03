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

#include <GLUE/reconfiguration/SupportLowerReceptor.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

SupportLowerReceptor::SupportLowerReceptor() :
	fus(),
	simpleAddSealed(false),
	simpleAddFU(NULL),
	priorizedCTI(-1),
	logger("GLUE", "noLogger")
{
} // SupportLowerReceptor


void
SupportLowerReceptor::add(FunctionalUnit* fu)
{
	assure(!simpleAddSealed, "SupportLowerReceptor::add(FunctionalUnit* it) may only be called on initial FUN setup.");
	simpleAddSealed = true;
	simpleAddFU = fu;
} // add


FunctionalUnit*
SupportLowerReceptor::getSimpleAddFU() const
{
	return simpleAddFU;
} // getSimpleAddFU


void
SupportLowerReceptor::add(FunctionalUnit* it, int cti)
{
	assure(fus.size() < 3, "Not more than 2 FU paths may be connected to SupportUpperFU the same time.");

	MESSAGE_BEGIN(VERBOSE, logger, m, "Receptor: Adding FU path");
	m << " with cti " << cti;
	MESSAGE_END();

	fus.insert(cti, it);
} // add


void
SupportLowerReceptor::remove(int cti)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "Receptor: Removing FU path");
	m << " with cti " << cti;
	MESSAGE_END();

	if (priorizedCTI == cti)
		priorizedCTI = -1;

	fus.erase(cti);
} // remove


void
SupportLowerReceptor::clear()
{
	assure(false, "Clear not supported");
} // clear


uint32_t
SupportLowerReceptor::size() const
{
	return fus.size();
} // size


const Link::ExchangeContainer
SupportLowerReceptor::get() const
{
	assure(false, "Exchange not supported.");
	return wns::ldk::Link::ExchangeContainer();
} // get


void
SupportLowerReceptor::set(const wns::ldk::Link::ExchangeContainer&)
{
	assure(false, "Exchange not supported.");
} // set


void
SupportLowerReceptor::wakeup()
{
	if (fus.knows(priorizedCTI))
		fus.find(priorizedCTI)->wakeup();

	for (FunctionalUnitContainer::const_iterator it = fus.begin();
	     it != fus.end();
	     ++it)
	{
		if (it->first != priorizedCTI)
			it->second->wakeup();
	}
} // wakeup


void
SupportLowerReceptor::priorizeCTI(int _cti)
{
	assure(fus.knows(_cti), "Can't priorize cti: cti not known.");

	priorizedCTI = _cti;
} // priorizeCTI


void
SupportLowerReceptor::setLogger(const wns::logger::Logger& _logger)
{
	logger = _logger;
} // setLogger



