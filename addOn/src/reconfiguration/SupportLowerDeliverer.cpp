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

#include <GLUE/reconfiguration/SupportLowerDeliverer.hpp>
#include <GLUE/reconfiguration/SupportLower.hpp>
#include <GLUE/reconfiguration/Manager.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

SupportLowerDeliverer::SupportLowerDeliverer() :
	fus(),
	controlCTIs(),
	simpleAddSealed(false),
	simpleAddFU(NULL),
	drainFU(NULL),
	firstCompound(true),
	notifyCTI(-1),
	logger("GLUE", "noLogger")
{
	friends.supportLowerFU = NULL;
} // SupportLowerDeliverer


void
SupportLowerDeliverer::add(FunctionalUnit* fu)
{
	assure(!simpleAddSealed, "SupportLowerDeliverer::add(FunctionalUnit* it) may only be called on initial FUN setup.");
	simpleAddSealed = true;
	simpleAddFU = fu;
} // add


FunctionalUnit*
SupportLowerDeliverer::getSimpleAddFU() const
{
	return simpleAddFU;
} // getSimpleAddFU


void
SupportLowerDeliverer::add(FunctionalUnit* it, int cti, int controlCTI)
{
	assure(fus.size() < 3, "Not more than 2 FUs may be connected to SupportUpperFU the same time.");

	MESSAGE_BEGIN(VERBOSE, logger, m, "Deliverer: Adding FU path");
	m << " with cti " << cti
	  << " and control cti " << controlCTI;
	MESSAGE_END();

	fus.insert(cti, it);
	controlCTIs.insert(cti, controlCTI);
} // add


wns::ldk::FunctionalUnit*
SupportLowerDeliverer::getFU(int cti)
{
	assure(fus.knows(cti), "Unknown cti");

	return fus.find(cti);
} // getFU


void
SupportLowerDeliverer::remove(int cti)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "Deliverer: Removing FU path");
	m << " with cti " << cti;
	MESSAGE_END();

	fus.erase(cti);
	controlCTIs.erase(cti);
} // remove


void
SupportLowerDeliverer::clear()
{
	assure(false, "Clear not supported");
} // clear


uint32_t
SupportLowerDeliverer::size() const
{
	return fus.size();
} // size


const Link::ExchangeContainer
SupportLowerDeliverer::get() const
{
	assure(false, "Exchange not supported.");
	return wns::ldk::Link::ExchangeContainer();
} // get


void
SupportLowerDeliverer::set(const wns::ldk::Link::ExchangeContainer&)
{
	assure(false, "Exchange not supported.");
} // set


CompoundHandlerInterface*
SupportLowerDeliverer::getAcceptor(const CompoundPtr& compound)
{
	assureType(friends.supportLowerFU, SupportLower*);
	SupportLowerCommand* command = dynamic_cast<SupportLowerCommand*>(friends.supportLowerFU->getCommand(compound->getCommandPool()));

	if (!fus.knows(command->peer.cti))
	{
		MESSAGE_BEGIN(QUIET, logger, m, "Deliverer: Received compound with invalid cti ");
		m << command->peer.cti
		  << "\nForwarding compound to Drain FU";
		MESSAGE_END();

		return drainFU;
	}

	if (controlCTIs.find(command->peer.cti) != command->magic.controlCTI)
		throw wns::Exception("Can't process compound due to CTI ambiguity.");

	if (firstCompound)
	{
		firstCompound = false;
		friends.supportLowerFU->getManager()->firstCompoundReceived();
	}

	if (command->peer.cti == notifyCTI)
	{
		friends.supportLowerFU->getManager()->receivedCTI(notifyCTI);
		notifyCTI = -1;
	}

	return fus.find(command->peer.cti);
} // getAcceptor


void
SupportLowerDeliverer::setDrainFU(FunctionalUnit* _drainFU)
{
	drainFU = _drainFU;
} // setCompoundCatcher


void
SupportLowerDeliverer::setCTICommandProvider(SupportLower* sl)
{
	friends.supportLowerFU = sl;
} // setCTICommandProvider


void
SupportLowerDeliverer::notifyWhenReceived(int cti)
{
	notifyCTI = cti;
} // notifyWhenReceived


void
SupportLowerDeliverer::setLogger(const wns::logger::Logger& _logger)
{
	logger = _logger;
} // setLogger



