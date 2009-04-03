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

#include <GLUE/reconfiguration/SupportLowerCTISetter.hpp>
#include <GLUE/reconfiguration/SupportLower.hpp>

#include <WNS/pyconfig/Parser.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SupportLowerCTISetter,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.SupportLowerCTISetter",
				     wns::ldk::FUNConfigCreator);

SupportLowerCTISetter::SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Processor<SupportLowerCTISetter>(),
	SuspendSupport(fun, _config),
	wns::Cloneable<SupportLowerCTISetter>(),

	cti(config.get<int>("cti")),
	controlCTI(config.get<int>("controlCTI")),
	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger"))
{
	friends.supportLower = NULL;
} // SupportLowerCTISetter


SupportLowerCTISetter::SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config, SupportLower* supportLower) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Processor<SupportLowerCTISetter>(),
	SuspendSupport(fun, _config),
	wns::Cloneable<SupportLowerCTISetter>(),

	cti(-1),
	controlCTI(-1),
	config(_config),
	logger(_config.get("ctiLogger"))
{
	friends.supportLower = supportLower;
} // SupportLowerCTISetter


SupportLowerCTISetter::SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config, SupportLower* supportLower, int _cti, int _controlCTI) :
	CommandTypeSpecifier<>(fun),
	HasReceptor<>(),
	HasConnector<>(),
	HasDeliverer<>(),
	Processor<SupportLowerCTISetter>(),
	SuspendSupport(fun, _config),
	wns::Cloneable<SupportLowerCTISetter>(),

	cti(_cti),
	controlCTI(_controlCTI),
	config(_config),
	logger(_config.get("ctiLogger"))
{
	friends.supportLower = supportLower;
} // SupportLowerCTISetter


void
SupportLowerCTISetter::onFUNCreated()
{
	if (friends.supportLower)
		return;

	std::string name = config.get<std::string>("supportLower");
	friends.supportLower = getFUN()->findFriend<SupportLower*>(name);
} // onFUNCreated


void
SupportLowerCTISetter::processOutgoing(const CompoundPtr& compound)
{
	assure(cti != -1, "CTI not set");

	SupportLowerCommand* command =
		dynamic_cast<SupportLowerCommand*>(friends.supportLower->activateCommand(compound->getCommandPool()));
	command->peer.cti = cti;
	command->magic.controlCTI = controlCTI;

	MESSAGE_BEGIN(NORMAL, logger, m, "This is SupportLowerCTISetter");
	m << " writing cti " << cti;
	MESSAGE_END();
} // processOutgoing


void
SupportLowerCTISetter::processIncoming(const CompoundPtr& /* compound */)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "This is SupportLowerCTISetter");
	m << " receiving cti " << cti;
	MESSAGE_END();
} // processIncoming


void
SupportLowerCTISetter::setCTI(int _cti, int _controlCTI)
{
	MESSAGE_BEGIN(VERBOSE, logger, m, "This is SupportLowerCTISetter");
	m << " setting cti to " << _cti
	  << " and control cti to " << _controlCTI;
	MESSAGE_END();

	cti = _cti;
	controlCTI = _controlCTI;
} // setCTI


bool
SupportLowerCTISetter::onSuspend() const
{
	return true;
} // onSuspend



