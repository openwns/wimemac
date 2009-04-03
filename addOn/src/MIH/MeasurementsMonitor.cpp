/******************************************************************************
 * Measurements Monitor                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include "MeasurementsMonitor.hpp"

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>

using namespace glue::mih;

STATIC_FACTORY_REGISTER_WITH_CREATOR(MeasurementsMonitor,
				     wns::ldk::FunctionalUnit,
				     "glue.mih.MeasurementsMonitor",
				     wns::ldk::FUNConfigCreator);

MeasurementsMonitor::MeasurementsMonitor(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config) :
		wns::ldk::CommandTypeSpecifier<>(fuNet),
		wns::ldk::HasReceptor<>(),
		wns::ldk::HasConnector<>(),
		wns::ldk::HasDeliverer<>(),
		wns::Cloneable<MeasurementsMonitor>(),
		linkEventHandler(NULL),
		berProviderName(config.get<std::string>("berProvider")),
		currentBER(0.0),
		connected(false),
		linkDetectedFlag(false),
		linkEventMeasurementWindow(config.get<unsigned int>("linkEventMeasurementWindow")),
		linkDownFlag(true),
        logger(config.get<wns::pyconfig::View>("logger")),
		myConfig(config)
{
	friends.berProvider = NULL;
	linkIdentifier.technologyName = "Glue";
	linkIdentifier.source = wns::service::dll::UnicastAddress(config.get<int>("address"));

    const wns::probe::bus::ContextProviderCollection& cpc = fuNet->getLayer()->getContextProviderCollection();

    if (config.get<bool>("useLinkDetectedProbe"))
    {
        linkDetectedTriggerLevel = wns::probe::bus::ContextCollectorPtr(
            new wns::probe::bus::ContextCollector(cpc,
                                                  config.get<std::string>("linkDetectedProbeName")
                ));
    }

    if (config.get<bool>("useLinkDownProbe"))
    {
        linkDownTriggerLevel = wns::probe::bus::ContextCollectorPtr(
            new wns::probe::bus::ContextCollector(cpc,
                                                  config.get<std::string>("linkDownProbeName")
                ));
    }
    
    if (config.get<bool>("useBERLevelProbe"))
    {
        linkDownTriggerLevel = wns::probe::bus::ContextCollectorPtr(
            new wns::probe::bus::ContextCollector(cpc,
                                                  config.get<std::string>("berLevelProbeName")
                ));
    }

    MESSAGE_SINGLE(NORMAL, logger, "MeasurementsMonitor FU created ");
	setUpThresholds(config);
	startPeriodicTimeout(config.get<double>("periodicTimeoutPeriod"),config.get<double>("periodicTimeoutDelay"));
} // MeasurementsMonitor

MeasurementsMonitor::~MeasurementsMonitor(){
	if (connected) {
		friends.berProvider->detachBERConsumer(this);
	}
}
void
MeasurementsMonitor::onFUNCreated()
{
	friends.berProvider = getFUN()->findFriend<glue::BERProvider*>(berProviderName);
	friends.berProvider->attachBERConsumer(this);
	connected = true;

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Attached MeasurementsMonitor to BERProvider ";
	MESSAGE_END();
}  // onFUNCreated

void
MeasurementsMonitor::periodically()
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " MeasurementsMonitor::periodically";
	m << " Current BER: "<< currentBER;
	MESSAGE_END();

	assure(this->linkEventThresholdRegistry.knows(wns::service::dll::mih::LinkDetected), "No Threshold set for this event");
	assure(this->linkEventThresholdRegistry.knows(wns::service::dll::mih::LinkDown), "No Threshold set for this event");

	if(currentBER != 0.0) {
        if (berLevel != wns::probe::bus::ContextCollectorPtr())
        {
            berLevel->put(log10(currentBER));
        }

		receivedMeasurementsList.push_back(currentBER);

		Threshold linkDetectedThreshold = this->linkEventThresholdRegistry.find(wns::service::dll::mih::LinkDetected);
		Threshold linkDownThreshold = this->linkEventThresholdRegistry.find(wns::service::dll::mih::LinkDown);

        if (linkDetectedTriggerLevel != wns::probe::bus::ContextCollectorPtr())
        {
            if (currentBER < linkDetectedThreshold.getThreshold() && linkDownFlag) linkDetectedTriggerLevel->put(log10(currentBER));
        }

        if (linkDownTriggerLevel != wns::probe::bus::ContextCollectorPtr())
        {
            if (currentBER > linkDownThreshold.getThreshold() && linkDetectedFlag) linkDownTriggerLevel->put(log10(currentBER));
        }
        

		if ( receivedMeasurementsList.size() > linkEventMeasurementWindow ) {
			receivedMeasurementsList.erase(receivedMeasurementsList.begin());
		}
		double averageBER = 0.0;
		for(unsigned int i=0;i<receivedMeasurementsList.size();i++) {
			averageBER += receivedMeasurementsList[i];
		}
		averageBER = averageBER/receivedMeasurementsList.size();

		if ((averageBER < linkDetectedThreshold.getThreshold()) && !linkDetectedFlag
		    && receivedMeasurementsList.size() == linkEventMeasurementWindow )
		{
			wns::service::dll::mih::ConnectionIdentifier connectionIdentifier;
			connectionIdentifier.technologyName = this->linkIdentifier.technologyName;
			connectionIdentifier.source = this->linkIdentifier.source;
			connectionIdentifier.destination = this->linkIdentifier.source;
			wns::service::dll::mih::LinkDetectedEvent linkDetected(connectionIdentifier, wns::service::dll::mih::LinkDetected, true);
			linkDetectedFlag = true;
			linkDownFlag = false;
			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
			m << " onLinkDetectedEvent in linkEventHandler called !!";
			MESSAGE_END();
			linkEventHandler->onLinkDetectedEvent(linkDetected);

		}

		if ((averageBER > linkDownThreshold.getThreshold()) && !linkDownFlag
		    && linkDetectedFlag && receivedMeasurementsList.size() == linkEventMeasurementWindow)
		{
			wns::service::dll::mih::ConnectionIdentifier connectionIdentifier;
			connectionIdentifier.technologyName = this->linkIdentifier.technologyName;
			connectionIdentifier.source = this->linkIdentifier.source;
			connectionIdentifier.destination = this->linkIdentifier.source;
			wns::service::dll::mih::LinkDownEvent linkDown(connectionIdentifier, wns::service::dll::mih::LinkDown);
			linkDownFlag = true;
			linkDetectedFlag = false;
			MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
			m << " onLinkDownEvent in linkEventHandler called !!";
			MESSAGE_END();
			linkEventHandler->onLinkDownEvent(linkDown);
		}
	}
}  // periodically

void
MeasurementsMonitor::registerHandler(wns::service::dll::mih::LinkEventHandler* _linkEventHandler)
{
	assureNotNull(_linkEventHandler);
	assure(this->linkEventHandler == NULL, "Link Event handler already set");
	this->linkEventHandler = _linkEventHandler;

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " MeasurementsMonitor got a linkevent handler";
	MESSAGE_END();
} // registerHandler

void
MeasurementsMonitor::onBERProviderDeleted()
{
	connected = false;

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Lost BERProvider!";
	MESSAGE_END();
} // BERProviderDeleted

void
MeasurementsMonitor::onNewMeasurement(double BER, int packetSize)
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Received BER: " << BER
	  << ", packetSize: " << packetSize;
	MESSAGE_END();

	this->currentBER = BER;
} // reportBER

void
MeasurementsMonitor::setUpThresholds(const wns::pyconfig::View& config)
{
	// set up the LinkDetected threshold and register it with a LinkDetected Link Event
	Threshold linkDetectedThreshold;
	linkDetectedThreshold.setThreshold(config.get<double>("linkDetectedThreshold"));
	assure(!this->linkEventThresholdRegistry.knows(wns::service::dll::mih::LinkDetected), "threshold already set");
	this->linkEventThresholdRegistry.insert(wns::service::dll::mih::LinkDetected, linkDetectedThreshold);

	// set up the LinkDown threshold and register it with a LinkDown Link Event
	Threshold linkDownThreshold;
	linkDownThreshold.setThreshold(config.get<double>("linkDownThreshold"));
	assure(!this->linkEventThresholdRegistry.knows(wns::service::dll::mih::LinkDown), "threshold already set");
	this->linkEventThresholdRegistry.insert(wns::service::dll::mih::LinkDown, linkDownThreshold);

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << " Thresholds Set"<<"\n"
	  <<" LinkDetected :"<<linkDetectedThreshold.getThreshold()<<"\n"
	  <<" LinkDown :"<<linkDownThreshold.getThreshold();
	MESSAGE_END();
} // setUpThresholds

void
MeasurementsMonitor::doSendData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("sendData(...) of FunctionalUnitLight must not be called.");
} // doSendData

void
MeasurementsMonitor::doOnData(const wns::ldk::CompoundPtr& )
{
	throw wns::Exception("onData(...) of FunctionalUnitLight must not be called.");
} // doOnData

bool
MeasurementsMonitor::doIsAccepting(const wns::ldk::CompoundPtr& ) const
{
	throw wns::Exception("isAccepting(...) of FunctionalUnitLight must not be called.");
	return false;

} // doIsAccepting

void
MeasurementsMonitor::doWakeup()
{
	throw wns::Exception("wakeup(...) of FunctionalUnitLight must not be called.");
} // doWakeup
