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

#include <GLUE/reconfiguration/SimpleManager.hpp>
#include <GLUE/Component.hpp>

#include <WNS/node/component/FQSN.hpp>
#include <WNS/pyconfig/Sequence.hpp>
#include <WNS/Assure.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SimpleManager,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.SimpleManager",
				     FUNConfigCreator);

SimpleManager::SimpleManager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	Manager(fun, _config),
	glue::trigger::FunctionalUnitLight(fun, _config),
	master(_config.get<bool>("master")),
	peerRM(NULL),
	isSuspending(false),
	pathSuspended(false),
	peerPathSuspended(false),
	cti(0),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	if (master)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Starting Reconfiguration Manager in master mode");

		startPeriodicTimeout(config.get<double>("reconfigurationInterval"),
				     static_cast<double>(rand()%100) / 100.0 * config.get<double>("maxReconfigurationDelay")+.01);
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "Starting Reconfiguration Manager in slave mode");
	}

	getFUN()->getLayer()->addService(config.get<std::string>("name"), this);
} // SimpleManager


SimpleManager::~SimpleManager()
{
} // ~SimpleManager


int
SimpleManager::getCTIForSimpleAddFU()
{
	return cti;
} // getCTIForSimpleAddFU


void
SimpleManager::onFUNCreated()
{
	std::string nodeName = config.get<std::string>("peerNodeName");
	std::string serviceName = config.get<std::string>("name");

	MESSAGE_BEGIN(NORMAL, logger, m, "Connecting to peer Reconfiguration Manager at node ");
	m << nodeName;
	MESSAGE_END();

	wns::node::component::FQSN fqsn(nodeName, serviceName);
	peerRM = getFUN()->getLayer()->getRemoteService<glue::reconfiguration::SimpleManager*>(fqsn);
} // onFUNCreated


void
SimpleManager::suspendedPath()
{
	if (master)
	{
		MESSAGE_SINGLE(NORMAL, logger, "FU path to reconfigure in master FUN is suspended");

		pathSuspended = true;
		tryReconfiguration();
	}
	else
		peerRM->peerSuspendedPath();
} // suspendedPath


void
SimpleManager::peerSuspendedPath()
{
	MESSAGE_SINGLE(NORMAL, logger, "FU path to reconfigure in slave FUN is suspended");

	peerPathSuspended = true;
	tryReconfiguration();
} // peerSuspendedPath


void
SimpleManager::periodically()
{
	int numberReconfigurationScheme = cti % config.len("reconfigurationSchemes");
	MESSAGE_BEGIN(QUIET, logger, m, "Starting reconfiguration process...\n");
	m << "Using reconfiguration scheme no. " << numberReconfigurationScheme;
	MESSAGE_END();

	setNotAccepting();
	suspendPath(cti);

	assure(!peerRM->isMaster(), "Peer Reconfiguration Manager must not be a master RM!");
	peerRM->setNotAccepting();
	peerRM->suspendPath(cti);
} // periodically


bool
SimpleManager::isMaster() const
{
	return master;
} // isMaster


void
SimpleManager::tryReconfiguration()
{
	if (pathSuspended && peerPathSuspended)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Processing reconfiguration...");

		int numberReconfigurationScheme = cti % config.len("reconfigurationSchemes");
		wns::pyconfig::View currentReconfigurationScheme = config.get<wns::pyconfig::View>("reconfigurationSchemes", numberReconfigurationScheme);

		removePath(cti);
		peerRM->removePath(cti);

		++cti;

		createPath(cti, currentReconfigurationScheme);
		peerRM->createPath(cti, currentReconfigurationScheme);

		setAccepting(cti);
		peerRM->setAccepting(cti);

		MESSAGE_SINGLE(NORMAL, logger, "Reconfiguration process finished.\nWaking up replaced FUs...");

		wakeupPath();
		peerRM->wakeupPath();

		pathSuspended = false;
		peerPathSuspended = false;
	}
} // tryReconfiguration



