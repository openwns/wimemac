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

#include <GLUE/reconfiguration/SupportLower.hpp>
#include <GLUE/reconfiguration/SupportLowerCTISetter.hpp>
#include <GLUE/reconfiguration/Manager.hpp>
#include <GLUE/reconfiguration/Drain.hpp>

using namespace glue::reconfiguration;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SupportLower,
				     wns::ldk::FunctionalUnit,
				     "glue.reconfiguration.SupportLower",
				     wns::ldk::FUNConfigCreator);

SupportLower::SupportLower(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& _config) :
	CommandTypeSpecifier<SupportLowerCommand>(fuNet),
	HasReceptor<SupportLowerReceptor>(),
	HasConnector<>(),
	HasDeliverer<SupportLowerDeliverer>(),
	Processor<SupportLower>(),
	wns::Cloneable<SupportLower>(),
	ctiSize(_config.get<int>("ctiSize")),
	config(_config),
	logger(_config.get("logger"))
{
	friends.manager = NULL;
	friends.drain = NULL;
	getDeliverer()->setLogger(logger);
	getReceptor()->setLogger(logger);
} // SupportLower


FunctionalUnit*
SupportLower::whenConnecting()
{
	MESSAGE_SINGLE(NORMAL, logger, "Creating initial CTI Setter. Has to be post-configured!");

	SupportLowerCTISetter *ctiSetter = new SupportLowerCTISetter(getFUN(), config, this);

	ctiSetter->getConnector()->add(this);
	this->getReceptor()->add(ctiSetter);

	this->getDeliverer()->add(ctiSetter);

	return ctiSetter;
} // whenConnecting


FunctionalUnit*
SupportLower::createCTISetter(int cti, int controlCTI)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Creating CTI Setter");
	m << " for cti " << cti;
	MESSAGE_END();

	SupportLowerCTISetter* ctiSetter = new SupportLowerCTISetter(getFUN(), config, this, cti, controlCTI);

	ctiSetter->getConnector()->add(this);
	this->getReceptor()->add(ctiSetter, cti);
	this->getDeliverer()->add(ctiSetter, cti, controlCTI);

	return ctiSetter;
} // createCTISetter


void
SupportLower::onFUNCreated()
{
	friends.manager = getFUN()->findFriend<Manager*>(config.get<std::string>("reconfigurationManager"));
	friends.manager->setSupportLower(this);
	getDeliverer()->setCTICommandProvider(this);
	getDeliverer()->add(getDeliverer()->getSimpleAddFU(),
			    friends.manager->getCTIForSimpleAddFU(),
			    friends.manager->getControlCTIForSimpleAddFU());
	friends.drain = getFUN()->findFriend<Drain*>(config.get<std::string>("drainFU"));
	getDeliverer()->setDrainFU(friends.drain);
	assureType(getDeliverer()->getSimpleAddFU(), SupportLowerCTISetter*);
	dynamic_cast<SupportLowerCTISetter*>(getDeliverer()->getSimpleAddFU())->setCTI(friends.manager->getCTIForSimpleAddFU(),
										       friends.manager->getControlCTIForSimpleAddFU());
	getReceptor()->add(getReceptor()->getSimpleAddFU(), friends.manager->getCTIForSimpleAddFU());
} // onFUNCreated


void
SupportLower::processOutgoing(const CompoundPtr& compound)
{
	SupportLowerCommand* command = getCommand(compound->getCommandPool());

	MESSAGE_BEGIN(NORMAL, logger, m, "Sending cti ");
	m << command->peer.cti;
	MESSAGE_END();
} // processOutgoing


void
SupportLower::processIncoming(const CompoundPtr& compound)
{
	SupportLowerCommand* command = getCommand(compound->getCommandPool());

	MESSAGE_BEGIN(NORMAL, logger, m, "Delivering cti ");
	m << command->peer.cti;
	MESSAGE_END();
} // processIncoming


void
SupportLower::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
	getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

	commandPoolSize += ctiSize;
} // calculateSizes


Manager*
SupportLower::getManager() const
{
	return friends.manager;
} // getManager



