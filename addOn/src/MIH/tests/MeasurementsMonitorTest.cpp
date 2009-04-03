/******************************************************************************
 * MeasurementsMonitor Unit Test                                              *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include "GLUE/MIH/MeasurementsMonitor.hpp"
#include <GLUE/BERProvider.hpp>
#include <WNS/service/dll/mih/LinkEventHandler.hpp>
#include <WNS/service/dll/mih/LinkEventService.hpp>
#include <WNS/service/dll/mih/LinkIdentifier.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

using namespace glue::mih;

namespace glue { namespace mih { namespace tests{

	using namespace wns::ldk;

	/**
	* @brief In this we set up a BER provider a Measurements
	* Monitor FU and a Link Event Handler Stub. First we test
	* a Link Up Event which contains the BER, provided to the
	* Measurements Monitor and delivered to the Link Event
	* Handler(testEvent)
	* @TODO dbn: Show nig how to write this test using the eventscheduler
	*/
	class MeasurementsMonitorTest :
		public wns::TestFixture

	{
		CPPUNIT_TEST_SUITE(MeasurementsMonitorTest);
		CPPUNIT_TEST( testEvent );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void setUpMeasurementsMonitor();
		void setUpBERProvider();
		void cleanup();

		void testEvent();

		class LinkEventNotificationStub:
			virtual public wns::service::dll::mih::LinkEventHandler,
			virtual public wns::ldk::FunctionalUnit,
			public wns::ldk::CommandTypeSpecifier<>,
			public wns::ldk::HasReceptor<>,
			public wns::ldk::HasConnector<>,
			public wns::ldk::HasDeliverer<>,
			public wns::Cloneable<LinkEventNotificationStub>
		{
		 public:
			LinkEventNotificationStub(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View&):
				wns::ldk::CommandTypeSpecifier<>(fuNet),
				wns::ldk::HasReceptor<>(),
				wns::ldk::HasConnector<>(),
				wns::ldk::HasDeliverer<>(),
				wns::Cloneable<LinkEventNotificationStub>()
			{};

			~LinkEventNotificationStub(){};

			wns::service::dll::mih::LinkEventType type;

			void
			onLinkUpEvent(const wns::service::dll::mih::LinkUpEvent& linkEvent) {
				this->type=linkEvent.type;
			}
			void
			onLinkDetectedEvent(const wns::service::dll::mih::LinkDetectedEvent& linkEvent) {
				this->type=linkEvent.type;
			}
			void
			onLinkDownEvent(const wns::service::dll::mih::LinkDownEvent& linkEvent) {
				this->type=linkEvent.type;
			}
			void
			onLinkGoingDownEvent(const wns::service::dll::mih::LinkGoingDownEvent& linkEvent) {
				this->type=linkEvent.type;
			}
		private:

			void
			doSendData(const wns::ldk::CompoundPtr&) {
				throw wns::Exception("sendData(...) of LinkEventNotificationStub must not be called.");
			} // doSendData

			void
			doOnData(const wns::ldk::CompoundPtr&) {
				throw wns::Exception("onData(...) of LinkEventNotificationStub must not be called.");
			} // doOnData

			bool
			doIsAccepting(const wns::ldk::CompoundPtr&) const {
				throw wns::Exception("isAccepting(...) of LinkEventNotificationStub must not be called.");
				return false;

			} // doIsAccepting

			void
			doWakeup(){
				throw wns::Exception("wakeup(...) of LinkEventNotificationStub must not be called.");
			}

		}; // class LinkEventNotificationStub

	private:
		Layer* layer;
		fun::Main* fuNet;
		LinkEventNotificationStub* upper;
		MeasurementsMonitor* measurementsMonitor;
		BERProvider* berProvider;
	};

CPPUNIT_TEST_SUITE_REGISTRATION( MeasurementsMonitorTest );

void
MeasurementsMonitorTest::prepare()
{
	layer = new wns::ldk::tests::LayerStub();
	fuNet = new fun::Main(layer);

	wns::pyconfig::Parser emptyConfig;
	upper = new LinkEventNotificationStub(fuNet, emptyConfig);

	setUpMeasurementsMonitor();
	setUpBERProvider();
} // prepare

void
MeasurementsMonitorTest::setUpMeasurementsMonitor()
{
	// Construct MeasurementsMonitor FU
	std::stringstream ss;
	ss << "import glue.MIH\n"
	   << "measurementsMonitor = glue.MIH.MeasurementsMonitor(\"berProvider\",\"glue.\", \"1234\" ,\n"
	   << "useLinkDetectedProbe = False, useLinkDownProbe = False, useBERLevelProbe = False)\n"
	   << "measurementsMonitor.linkEventMeasurementWindow = 1\n"
	   << "measurementsMonitor.linkDetectedThreshold = 1e-5\n"
	   << "measurementsMonitor.linkDownThreshold = 1e-6\n"
	   << "measurementsMonitor.periodicTimeoutPeriod = 0.001\n"
	   << "measurementsMonitor.periodicTimeoutDelay = 0.0\n";

	wns::pyconfig::Parser all;
	all.loadString(ss.str());

	wns::pyconfig::View config(all, "measurementsMonitor");

	measurementsMonitor = new MeasurementsMonitor(fuNet, config);
	measurementsMonitor->registerHandler(upper);
} // setUpMeasurementsMonitor

void
MeasurementsMonitorTest::setUpBERProvider()
{
	// Construct BER Provider
	berProvider = new BERProvider;
	berProvider->attachBERConsumer(measurementsMonitor);
} // setUpBERProvider

void
MeasurementsMonitorTest::cleanup()
{
	delete measurementsMonitor;
	delete berProvider;
	delete upper;
	delete fuNet;
	delete layer;
} // cleanup


void
MeasurementsMonitorTest::testEvent()
{
	berProvider->notifyBERConsumers(0.3, 456);
	CPPUNIT_ASSERT_EQUAL(0.3, measurementsMonitor->getCurrBER());

	wns::service::dll::mih::ConnectionIdentifier connectionIdentifier;

	wns::service::dll::mih::LinkUpEvent LinkUp(connectionIdentifier, wns::service::dll::mih::LinkUp);
	measurementsMonitor->getLinkEventHandler()->onLinkUpEvent(LinkUp);
	CPPUNIT_ASSERT_EQUAL(wns::service::dll::mih::LinkUp, upper->type);
} // testEvent

} //tests

} //mih
}//glue

