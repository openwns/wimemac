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

#include <GLUE/reconfiguration/TransmittingManager.hpp>

#include <WNS/ldk/DelayedDeliveryInterface.hpp>

#include <WNS/ldk/Layer.hpp>
#include <WNS/pyconfig/Sequence.hpp>
#include <WNS/Assure.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(TransmittingManager,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.TransmittingManager",
				     FUNConfigCreator);

TransmittingManager::TransmittingManager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	Manager(fun, _config),
	CommandTypeSpecifier<TransmittingManagerCommand>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	wns::Cloneable<TransmittingManager>(),
	master(_config.get<bool>("master")),
	isReconfiguring(false),
	isSuspending(false),
	useOptimizedReconfigurationIfPossible(_config.get<bool>("useOptimizedReconfigurationIfPossible")),
	useOptimizedReconfiguration(false),
	proceedToReconfigureTimeout(false),
	cti(0),
	oldCTI(0),
	countReconfigurations(0),
	currentReconfigurationScheme(_config),
	reconfigurationRequest(),
	reconfigurationConfirm(),
	proceedToReconfigure(),
	reconfigurationFinished(),
	incomingPathReconfigured(false),
	outgoingPathReconfigured(false),
	reconfigurationFinishedFrameReceived(false),
	reconfigurationRequestFrameSize(_config.get<int>("reconfigurationRequestFrameSize")),
	frameSize(_config.get<int>("frameSize")),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	if (master)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Starting Reconfiguration Manager in master mode");
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "Starting Reconfiguration Manager in slave mode");
	}
} // TransmittingManager


TransmittingManager::~TransmittingManager()
{
	if (master)
	{
		MESSAGE_BEGIN(NORMAL, logger, m, "");
		m << "Processed " << countReconfigurations << " reconfigurations";
		MESSAGE_END();
	}
} // ~TransmittingManager


int
TransmittingManager::getCTIForSimpleAddFU()
{
	return cti;
} // getCTIForSimpleAddFU


void
TransmittingManager::onFUNCreated()
{
} // onFUNCreated


void
TransmittingManager::suspendedPath()
{
	MESSAGE_BEGIN(NORMAL, logger, m, "");
	m << "FU path to reconfigure in ";
	if (master)
		m << "master";
	else
		m << "slave";
	m << " FUN is suspended";
	MESSAGE_END();

	outgoingPathReconfigured = true;
	tryRemovingOldPath();

	MESSAGE_SINGLE(NORMAL, logger, "Creating ProceedToReconfigure frame");

	proceedToReconfigure = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

	TransmittingManagerCommand* command = activateCommand(proceedToReconfigure->getCommandPool());
	command->peer.type = TransmittingManagerCommand::ProceedToReconfigure;

	proceedToReconfigureTimeout = true;
	setTimeout(config.get<double>("retransmissionTimeoutOptimizedReconfiguration"));

	doWakeup();

	if (!useOptimizedReconfiguration)
	{
		setAccepting(cti);

		MESSAGE_SINGLE(NORMAL, logger, "Reconfiguration process for outgoing path finished.\nWaking up replaced FUs...");
		wakeupPath();
	}
} // suspendedPath


void
TransmittingManager::firstCompoundReceived()
{
	if (master)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Starting reconfiguration timer");

		startPeriodicTimeout(config.get<double>("reconfigurationInterval"),
				     static_cast<double>(rand()%100) / 100.0 * config.get<double>("maxReconfigurationDelay")+.05);
	}
} // firstCompoundReceived


void
TransmittingManager::periodically()
{
	if (!isReconfiguring)
	{
		isReconfiguring = true;

		++countReconfigurations;

		int numberReconfigurationScheme = countReconfigurations % config.len("reconfigurationSchemes");
		currentReconfigurationScheme = config.get<wns::pyconfig::View>("reconfigurationSchemes", numberReconfigurationScheme);

		MESSAGE_BEGIN(QUIET, logger, m, "Starting reconfiguration process...\n");
		m << "Using reconfiguration scheme no. " << numberReconfigurationScheme;
		MESSAGE_END();

		oldCTI = cti;
		cti = (cti + 1) % config.get<int>("ctiSequenceSize");

		createPath(cti, currentReconfigurationScheme);

		if (useOptimizedReconfigurationIfPossible)
		{
			MESSAGE_SINGLE(NORMAL, logger, "Using optimized reconfiguration mechanism if supported by reconfiguration scheme");
			useOptimizedReconfiguration = currentReconfigurationScheme.get<bool>("useOptimizedReconfiguration");

			if (useOptimizedReconfiguration)
			{
				MESSAGE_SINGLE(NORMAL, logger, "Delaying delivery of incoming compounds for the new FU path");

				getFUN()->findFriend<DelayedDeliveryInterface*>(currentReconfigurationScheme.get<std::string>("delayedDeliveryFU"))
					->delayDelivery();
			}
		}
		else
			useOptimizedReconfiguration = false;

		MESSAGE_SINGLE(NORMAL, logger, "Creating ReconfigurationRequest frame");

		reconfigurationRequest = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

		TransmittingManagerCommand* command = activateCommand(reconfigurationRequest->getCommandPool());
		command->peer.type = TransmittingManagerCommand::ReconfigurationRequest;
		command->peer.cti = cti;
		command->peer.reconfigurationScheme = numberReconfigurationScheme;

		setTimeout(config.get<double>("retransmissionTimeoutReconfigurationRequest"));

		doWakeup();
	}
} // periodically


void
TransmittingManager::onTimeout()
{
	if (proceedToReconfigureTimeout)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Creating ProceedToReconfigure frame");

		proceedToReconfigure = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

		TransmittingManagerCommand* command = activateCommand(proceedToReconfigure->getCommandPool());
		command->peer.type = TransmittingManagerCommand::ProceedToReconfigure;

		setTimeout(config.get<double>("retransmissionTimeoutOptimizedReconfiguration"));

		doWakeup();
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "Creating ReconfigurationRequest frame");

		reconfigurationRequest = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

		TransmittingManagerCommand* command = activateCommand(reconfigurationRequest->getCommandPool());
		command->peer.type = TransmittingManagerCommand::ReconfigurationRequest;
		command->peer.cti = cti;
		command->peer.reconfigurationScheme = countReconfigurations % config.len("reconfigurationSchemes");

		setTimeout(config.get<double>("retransmissionTimeoutReconfigurationRequest"));

		doWakeup();
	}
} // onTimeout


void
TransmittingManager::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
	getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

	TransmittingManagerCommand* command = getCommand(commandPool);

	if (command->peer.type == TransmittingManagerCommand::ReconfigurationRequest)
		commandPoolSize += reconfigurationRequestFrameSize;
	else
		commandPoolSize += frameSize;
} // calculateSizes


void
TransmittingManager::doOnData(const wns::ldk::CompoundPtr& compound)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "Compound backtrace");
	m << compound->dumpJourney();
	MESSAGE_END();

	TransmittingManagerCommand* command = getCommand(compound->getCommandPool());

	switch (command->peer.type)
	{
	case TransmittingManagerCommand::ReconfigurationRequest :
	{
		MESSAGE_SINGLE(NORMAL, logger, "Received ReconfigurationRequest frame");

		if (master)
			throw wns::Exception("Reconfiguration Manager in master mode received reconfiguration request!");

		if (!isReconfiguring)
		{
			isReconfiguring = true;

			oldCTI = cti;
			cti = command->peer.cti;

			currentReconfigurationScheme = config.get<wns::pyconfig::View>("reconfigurationSchemes", command->peer.reconfigurationScheme);

			if (useOptimizedReconfigurationIfPossible)
				useOptimizedReconfiguration = currentReconfigurationScheme.get<bool>("useOptimizedReconfiguration");
			else
				useOptimizedReconfiguration = false;

			createPath(cti, currentReconfigurationScheme);

			if (useOptimizedReconfiguration)
			{
				MESSAGE_SINGLE(NORMAL, logger, "Delaying delivery of incoming compounds for the new FU path");

				getFUN()->findFriend<DelayedDeliveryInterface*>(currentReconfigurationScheme.get<std::string>("delayedDeliveryFU"))
					->delayDelivery();
				setAccepting(cti);
			}
			else
				setNotAccepting();

			MESSAGE_SINGLE(NORMAL, logger, "Creating ReconfigurationConfirm frame");

			reconfigurationConfirm = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

			TransmittingManagerCommand* command = activateCommand(reconfigurationConfirm->getCommandPool());
			command->peer.type = TransmittingManagerCommand::ReconfigurationConfirm;

			doWakeup();

			MESSAGE_SINGLE(NORMAL, logger, "Suspending FU path to reconfigure in slave FUN");
			isSuspending = true;
			suspendPath(oldCTI);
		}
		else
		{
			MESSAGE_SINGLE(NORMAL, logger, "Creating ReconfigurationConfirm frame");

			reconfigurationConfirm = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

			TransmittingManagerCommand* command = activateCommand(reconfigurationConfirm->getCommandPool());
			command->peer.type = TransmittingManagerCommand::ReconfigurationConfirm;

			doWakeup();
		}

		break;
	}
	case TransmittingManagerCommand::ReconfigurationConfirm :
	{
		MESSAGE_SINGLE(NORMAL, logger, "Received ReconfigurationConfirm frame");

		if (!master)
			throw wns::Exception("Reconfiguration Manager in slave mode received reconfiguration confirm!");

		if (isReconfiguring && !isSuspending)
		{
			if (useOptimizedReconfiguration)
				setAccepting(cti);
			else
				setNotAccepting();

			MESSAGE_SINGLE(NORMAL, logger, "Suspending FU path to reconfigure in master FUN");
			isSuspending = true;
			suspendPath(oldCTI);
		}

		if (hasTimeoutSet() && !proceedToReconfigureTimeout)
			cancelTimeout();

		break;
	}
	case TransmittingManagerCommand::ProceedToReconfigure :
	{
		MESSAGE_SINGLE(NORMAL, logger, "Received ProceedToReconfigure frame");

		if (isReconfiguring)
		{
			if (!incomingPathReconfigured)
			{
				incomingPathReconfigured = true;

				if (useOptimizedReconfiguration)
				{
					MESSAGE_SINGLE(NORMAL, logger, "Starting delayed delivery");
					getFUN()->findFriend<DelayedDeliveryInterface*>(currentReconfigurationScheme.get<std::string>("delayedDeliveryFU"))
						->deliver();
				}

				MESSAGE_SINGLE(NORMAL, logger, "Reconfiguration process for incoming path finished.");

				if (!master)
					tryRemovingOldPath();
			}
			if (!isSuspending && master)
			{
				isSuspending = true;

				if (useOptimizedReconfiguration)
					setAccepting(cti);
				else
					setNotAccepting();

				MESSAGE_SINGLE(NORMAL, logger, "Suspending FU path to reconfigure in master FUN");
				suspendPath(oldCTI);

				if (hasTimeoutSet())
				{
					if (!proceedToReconfigureTimeout)
						cancelTimeout();
					else
						assure(false, "Invalid timeout set");
				}
			}
		}

		MESSAGE_SINGLE(NORMAL, logger, "Creating ReconfigurationFinished frame");

		reconfigurationFinished = CompoundPtr(new Compound(getFUN()->getProxy()->createCommandPool()));

		TransmittingManagerCommand* command = activateCommand(reconfigurationFinished->getCommandPool());
		command->peer.type = TransmittingManagerCommand::ReconfigurationFinished;

		doWakeup();

		break;
	}
	case TransmittingManagerCommand::ReconfigurationFinished :
	{
		MESSAGE_SINGLE(NORMAL, logger, "Received ReconfigurationFinished frame");

		if (isReconfiguring)
			reconfigurationFinishedFrameReceived = true;

		tryRemovingOldPath();

		if (hasTimeoutSet())
			if (proceedToReconfigureTimeout)
				cancelTimeout();
			else
				assure(false, "Invalid timeout set");

		proceedToReconfigureTimeout = false;

		break;
	}
	default :
	{
		throw wns::Exception("Received command with unknown frame type.");
	}
	}

} // doOnData


void
TransmittingManager::doWakeup()
{
	if (reconfigurationRequest && getConnector()->hasAcceptor(reconfigurationRequest))
	{
		MESSAGE_SINGLE(NORMAL, logger, "Transmitting ReconfigurationRequest frame...");

		CompoundPtr help = reconfigurationRequest;
		reconfigurationRequest = CompoundPtr();
		getConnector()->getAcceptor(help)->sendData(help);
	}
	if (reconfigurationConfirm && getConnector()->hasAcceptor(reconfigurationConfirm))
	{
		MESSAGE_SINGLE(NORMAL, logger, "Transmitting ReconfigurationConfirm frame...");

		CompoundPtr help = reconfigurationConfirm;
		reconfigurationConfirm = CompoundPtr();
		getConnector()->getAcceptor(help)->sendData(help);
	}
	if (proceedToReconfigure && getConnector()->hasAcceptor(proceedToReconfigure))
	{
		MESSAGE_SINGLE(NORMAL, logger, "Transmitting ProceedToReconfigure frame...");

		CompoundPtr help = proceedToReconfigure;
		proceedToReconfigure = CompoundPtr();
		getConnector()->getAcceptor(help)->sendData(help);
	}
	if (reconfigurationFinished && getConnector()->hasAcceptor(reconfigurationFinished))
	{
		MESSAGE_SINGLE(NORMAL, logger, "Transmitting ReconfigurationFinished frame...");

		CompoundPtr help = reconfigurationFinished;
		reconfigurationFinished = CompoundPtr();
		getConnector()->getAcceptor(help)->sendData(help);
	}
} // wakeup


void
TransmittingManager::tryRemovingOldPath()
{
	if (incomingPathReconfigured &&
	    outgoingPathReconfigured &&
	    reconfigurationFinishedFrameReceived)
	{
		removePath(oldCTI);

		incomingPathReconfigured = false;
		outgoingPathReconfigured = false;
		reconfigurationFinishedFrameReceived = false;

		isReconfiguring = false;
		isSuspending = false;
	}
} // tryRemovingOldPath



