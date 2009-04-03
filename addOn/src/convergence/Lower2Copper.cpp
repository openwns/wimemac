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

#include <GLUE/convergence/Lower2Copper.hpp>
#include <GLUE/convergence/Upper.hpp>
#include <GLUE/Component.hpp>

#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/module/Base.hpp>

#include <cstdlib>


using namespace glue::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Lower2Copper,
	wns::ldk::FunctionalUnit,
	"glue.convergence.Lower2Copper",
	wns::ldk::FUNConfigCreator);

Lower2Copper::Lower2Copper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
	wns::ldk::CommandTypeSpecifier<LowerCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Lower2Copper>(),

	config(_config),
	logger(_config.get<wns::pyconfig::View>("logger")),
	dataTransmission(NULL),
	notificationService(NULL),
	isBlocking(_config.get<bool>("blocking"))
{
} // Lower2Copper

Lower2Copper::~Lower2Copper()
{
} // ~Lower2Copper

void Lower2Copper::onFUNCreated()
{
	friends.unicastRouting =
		getFUN()->findFriend<glue::convergence::UnicastUpper*>(
			config.get<std::string>("unicastRouting"));

	friends.broadcastRouting =
		getFUN()->findFriend<glue::convergence::BroadcastUpper*>(
			config.get<std::string>("broadcastRouting"));
} // onFUNCreated

bool Lower2Copper::doIsAccepting(const wns::ldk::CompoundPtr& /* compound */) const
{
	if (this->isBlocking == true)
	{
		return getDataTransmissionService()->isFree();
	}
	else
	{
		return true;
	}
} // isAccepting

void Lower2Copper::onCarrierIdle()
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": carrier idle, accepting new transmissions";
	MESSAGE_END();

	getReceptor()->wakeup();
}

void Lower2Copper::onCarrierBusy()
{
	// currently, we do nothing.
}

void Lower2Copper::onCollision()
{
	// currently, we do nothing. In future we might stop ongoing transmissions
} // onCollision

void
Lower2Copper::doSendData(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "sendData called with an invalid compound.");

	if (hasCommandOf(friends.unicastRouting, compound)) {
		UnicastUpperCommand* command = friends.unicastRouting->getCommand(compound->getCommandPool());
		getDataTransmissionService()->sendData(command->peer.targetMACAddress, compound);
	}
 	else if (hasCommandOf(friends.broadcastRouting, compound)) {
		BroadcastUpperCommand* command = friends.broadcastRouting->getCommand(compound->getCommandPool());
		getDataTransmissionService()->sendData(command->peer.targetMACAddress, compound);
	}
	else assure(false, "Not a routing Compound!");
} // doSendData

void Lower2Copper::doOnData(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "onData called with an invalid compound.");

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": doOnData(), forwading to higher FU";
	MESSAGE_END();

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData

void
Lower2Copper::doWakeup()
{
	// This will never be called ...
} // doWakeup

void
Lower2Copper::onData(const wns::osi::PDUPtr& pdu, double ber, bool collision)
{
	assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");

	// In case of a collision -> throw away ... packet can't be decoded
	if (collision)
	{
		return;
	}

	// FIRST: create a copy instead of working on the real compound
	wns::ldk::CompoundPtr compound = wns::staticCast<wns::ldk::Compound>(pdu)->copy();

	if (hasCommandOf(friends.unicastRouting, compound)) {
		UnicastUpperCommand* uc = friends.unicastRouting->getCommand(compound->getCommandPool());
		if (uc->peer.targetMACAddress == address) {
			pushUp(compound, ber, pdu);
		}
	}
	else if (hasCommandOf(friends.broadcastRouting, compound)) {
		pushUp(compound, ber, pdu);
	}
	// else throw away
	// Data was not for us
	// should not happen with current copper implementation
} // onData

void
Lower2Copper::setDataTransmissionService(wns::service::Service* phy)
{
	assure(phy, "must be non-NULL");
	assureType(phy, wns::service::phy::copper::DataTransmission*);
	dataTransmission = dynamic_cast<wns::service::phy::copper::DataTransmission*>(phy);
} // setDataTransmissionService

wns::service::phy::copper::DataTransmission*
Lower2Copper::getDataTransmissionService() const
{
	assure(dataTransmission, "no copper::DataTransmission set. Did you call setDataTransmission()?");
	return dataTransmission;
} // getDataTransmissionService

void
Lower2Copper::setNotificationService(wns::service::Service* phy)
{
	assure(phy, "must be non-NULL");
	assureType(phy, wns::service::phy::copper::Notification*);
	notificationService = dynamic_cast<wns::service::phy::copper::Notification*>(phy);
	// attach for both, data handling an carrier sensing
	this->wns::Observer<wns::service::phy::copper::Handler>::startObserving(notificationService);
	this->wns::Observer<wns::service::phy::copper::CarrierSensing>::startObserving(notificationService);
	notificationService->setDLLUnicastAddress(address);
} // setNotificationService

wns::service::phy::copper::Notification*
Lower2Copper::getNotificationService() const
{
	assure(notificationService, "no copper::Notification set. Did you call setNotificationService()?");
	return notificationService;
} // getNotificationService

void
Lower2Copper::setMACAddress(const wns::service::dll::UnicastAddress& _address)
{
	address = _address;
	MESSAGE_SINGLE(NORMAL, logger, "setting MAC address of lowerConvergence to: " << address);
} // setMACAddress

void
Lower2Copper::pushUp(const wns::ldk::CompoundPtr& compound, double ber, const wns::osi::PDUPtr& pdu)
{
	LowerCommand* lc = activateCommand(compound->getCommandPool());
	lc->local.per = 1.0 - pow(1.0 - ber, pdu->getLengthInBits());
	notifyBERConsumers(ber, pdu->getLengthInBits());
	this->wns::ldk::FunctionalUnit::onData(compound);
} // pushUp


