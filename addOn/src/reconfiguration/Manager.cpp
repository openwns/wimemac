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

#include <GLUE/reconfiguration/Manager.hpp>
#include <GLUE/reconfiguration/SupportUpper.hpp>
#include <GLUE/reconfiguration/SupportUpperConnector.hpp>
#include <GLUE/reconfiguration/SupportLower.hpp>
#include <GLUE/reconfiguration/SupportLowerReceptor.hpp>
#include <GLUE/reconfiguration/utils.hpp>

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/utils.hpp>
#include <WNS/Assure.hpp>

#include <algorithm>

using namespace glue::reconfiguration;
using namespace wns::ldk;

Manager::RaceConditionSolver::RaceConditionSolver(Manager* _manager) :
	manager(_manager)
{} // RaceConditionSolver

void
Manager::RaceConditionSolver::solveRaceCondition()
{
	setTimeout(0.0);
} // solveRaceCondition

void
Manager::RaceConditionSolver::onTimeout()
{
	manager->raceConditionSolved();
} // onTimeout

Manager::Manager(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config) :
	fun(_fun),
	suspendingPath(false),
	suspendFUs(),
	suspendingFU(NULL),
	controlCTI(0),
	rcs(this),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	friends.supportUpperFU = NULL;
	friends.supportLowerFU = NULL;
} // Manager


Manager::~Manager()
{
} // ~Manager


void
Manager::setSupportUpper(SupportUpper* su)
{
	MESSAGE_SINGLE(VERBOSE, logger, "Registering SupportUpper FU at Reconfiguration Manager");

	assure(!friends.supportUpperFU, "SupportUpper FU is already registered at Reconfiguration Manager FU");
	friends.supportUpperFU = su;
} // setSupportUpper


void
Manager::setSupportLower(SupportLower* sl)
{
	MESSAGE_SINGLE(VERBOSE, logger, "Registering SupportLower FU at Reconfiguration Manager");

	assure(!friends.supportLowerFU, "SupportLower FU is already registered at Reconfiguration Manager FU");
	friends.supportLowerFU = sl;
} // setSupportLower


int
Manager::getControlCTIForSimpleAddFU() const
{
	return controlCTI;
} // getControlCTIForSimpleAddFU


void
Manager::setNotAccepting()
{
	MESSAGE_SINGLE(NORMAL, logger, "Setting SupportUpper FU not accepting");

	assureType(friends.supportUpperFU->getConnector(), SupportUpperConnector*);
	dynamic_cast<SupportUpperConnector*>(friends.supportUpperFU->getConnector())->setNotAccepting();
} // setNotAccepting


void
Manager::setAccepting(int cti)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Setting SupportUpper FU accepting");
	m << " (cti " << cti << ")";
	MESSAGE_END();

	assureType(friends.supportUpperFU->getConnector(), SupportUpperConnector*);
	dynamic_cast<SupportUpperConnector*>(friends.supportUpperFU->getConnector())->setAccepting(cti);
} // setAccepting


void
Manager::priorizeCTI(int priorizeCTI)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Setting SupportLower FU priorizing");
	m << " cti " << priorizeCTI;
	MESSAGE_END();

	assureType(friends.supportLowerFU->getReceptor(), SupportLowerReceptor*);
	dynamic_cast<SupportLowerReceptor*>(friends.supportLowerFU->getReceptor())->priorizeCTI(priorizeCTI);
} // priorizeCTI


void
Manager::createPath(int cti, const wns::pyconfig::View& config)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Creating new path for");
	m << " cti " << cti;
	MESSAGE_END();

	configureFUN(fun, config.get<wns::pyconfig::View>("fun"));
	initNewFUNPart(fun, config.get<wns::pyconfig::View>("fun"));

	FunctionalUnit* pathTopFU = fun->findFriend<FunctionalUnit*>(config.get<std::string>("pathTopFU"));
	friends.supportUpperFU->getConnector()->add(pathTopFU, cti);
	pathTopFU->getDeliverer()->add(friends.supportUpperFU);
	pathTopFU->getReceptor()->add(friends.supportUpperFU);

	++controlCTI;
	FunctionalUnit* pathBottomFU = fun->findFriend<FunctionalUnit*>(config.get<std::string>("pathBottomFU"));
	FunctionalUnit* ctiSetter = friends.supportLowerFU->createCTISetter(cti, controlCTI);
	pathBottomFU->getConnector()->add(ctiSetter);
	ctiSetter->getDeliverer()->add(pathBottomFU);
	ctiSetter->getReceptor()->add(pathBottomFU);
} // createPath


void
Manager::removePath(int cti)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Removing path with");
	m << " cti " << cti;
	MESSAGE_END();

	deleteFUNPart(fun,
		      friends.supportLowerFU->getDeliverer()->getFU(cti),
		      friends.supportUpperFU);

	friends.supportUpperFU->getConnector()->remove(cti);
	friends.supportLowerFU->getDeliverer()->remove(cti);
	friends.supportLowerFU->getReceptor()->remove(cti);
} // removePath


void
Manager::suspended(const SuspendableInterface* si)
{
	if (suspendingPath)
	{
		if (si != suspendingFU && si)
			return;

		if (!suspendFUs.empty())
		{
			wns::ldk::FunctionalUnit* nextSuspendFU = suspendFUs[0];
			suspendFUs.erase(suspendFUs.begin());

			FUContainer nextSuspendFUs = nextSuspendFU->getConnector()->get();

			nextSuspendFUs.erase(std::remove(nextSuspendFUs.begin(),
							 nextSuspendFUs.end(),
							 friends.supportLowerFU),
					     nextSuspendFUs.end());

			suspendFUs.insert(suspendFUs.begin(),
					  nextSuspendFUs.begin(),
					  nextSuspendFUs.end());

			MESSAGE_BEGIN(NORMAL, logger, m, "Suspending FU ");
			m << nextSuspendFU->getName();
			MESSAGE_END();

			assureType(nextSuspendFU, wns::ldk::SuspendableInterface*);
			suspendingFU = dynamic_cast<wns::ldk::SuspendableInterface*>(nextSuspendFU);
			suspendingFU->suspend(this);
		}
		else
		{
			suspendingPath = false;
			// to resolve race conditions
			rcs.solveRaceCondition();
		}
	}
} // suspended


void
Manager::raceConditionSolved()
{
	suspendedPath();
} // raceConditionSolved


void
Manager::suspendPath(int cti)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Suspending path with");
	m << " cti " << cti;
	MESSAGE_END();

	assure(!suspendingPath, "Already suspending a path.");
	assure(friends.supportUpperFU, "Support Upper FU not set");
	assureType(friends.supportUpperFU->getConnector(), SupportUpperConnector*);

	SupportUpperConnector* suc = dynamic_cast<SupportUpperConnector*>(friends.supportUpperFU->getConnector());
	suspendFUs.push_back(suc->getFU(cti));
	suspendingPath = true;

	suspended(NULL);
} // suspendPath


void
Manager::cancelSuspendPath()
{
	MESSAGE_SINGLE(NORMAL, logger, "Canceling suspending path");

	suspendingPath = false;
	suspendFUs.clear();
} // cancelSuspendPath


void
Manager::wakeupPath()
{
	MESSAGE_SINGLE(NORMAL, logger, "Waking up upper FUs");

	friends.supportUpperFU->wakeup();
} // wakeupPath


void
Manager::notifyWhenReceived(int cti)
{
	friends.supportLowerFU->getDeliverer()->notifyWhenReceived(cti);
} // notifyWhenReceived


void
Manager::firstCompoundReceived()
{
	MESSAGE_SINGLE(NORMAL, logger, "SupportLower received first incoming compound");
} // firstCompoundReceived


void
Manager::receivedCTI(int cti)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Notification:");
	m << " Received compound with cti " << cti;
	MESSAGE_END();
} // receivedCTI



