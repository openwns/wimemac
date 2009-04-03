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

#ifndef GLUE_MIH_MEASUREMENTSMONITOR_HPP
#define GLUE_MIH_MEASUREMENTSMONITOR_HPP

#include <GLUE/BERConsumer.hpp>
#include <GLUE/BERProvider.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/service/dll/mih/LinkEventService.hpp>
#include <WNS/service/dll/mih/LinkEventHandler.hpp>
#include <WNS/service/dll/mih/LinkIdentifier.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <vector>

namespace glue { namespace mih {

	/**
	 * @brief Threshold is used to represent thresholds for LinkEvents
	 * @author Nikola Gaydarov <nig@comnets.rwth-aachen.de>
	 */
	class Threshold
	{
	public:
		Threshold(){
			thresholdValue = 0.0;
		};

		double
		getThreshold() {
			return this->thresholdValue;
		};

		void
		setThreshold(double _thresholdValue){
			this->thresholdValue = _thresholdValue;
		};

	private:
		double thresholdValue;
	};

	/**
	 * @brief Contains a mapping between a Link Event Type and the appropiate Threshold for it
	 */
	typedef wns::container::Registry<wns::service::dll::mih::LinkEventType, Threshold> LinkEventThresholdRegistry;

	/**
	 * @brief MeasurementsMonitor receives measurements reports from other FUs and generates Link Events
	 * @author Nikola Gaydarov <nig@comnets.rwth-aachen.de>
	 */
	class MeasurementsMonitor :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<MeasurementsMonitor>,
		public glue::BERConsumer,
		public wns::events::PeriodicTimeout,
		virtual public wns::service::dll::mih::LinkEventNotification

	{
	public:
		/**
		 * @brief Constructor
		 */
		MeasurementsMonitor(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		/**
		 * @brief Destructor
		 */
		~MeasurementsMonitor();

		/**
		 * @brief Perform some actions when all the FUs are created
		 */
		virtual void
		onFUNCreated();

		/**
		 * @brief Your callback
		 * It gets called periodically,
		 * whenever the timer fires.
		 */
		virtual void
		periodically();

		/**
		 * @brief Register the Link Event Handler
		 */
		virtual void
		registerHandler(wns::service::dll::mih::LinkEventHandler* _linkEventHandler);

		/**
		 * @brief This function is called by the BERProvider if it is deleted
		 * Afterwards it will call this->setBERProvider(NULL).
		 */
		virtual void
		onBERProviderDeleted();

		/**
		 * @brief This function is called by BERProvider, when new measurement is
		 * available
		 */
		virtual void
		onNewMeasurement(double BER, int packetSize);

		/**
		 * @brief Returns the current BER
		 * @TODO: dbn: Remove this method (adapt test)
		 */
		double
		getCurrBER(){ return currentBER;};

		 /**
		 * @brief Returns the Handler to call onLinkEvent()
		 * @TODO: dbn: Remove this method (adapt test)
		 */
		wns::service::dll::mih::LinkEventHandler*
		getLinkEventHandler(){ return linkEventHandler;}

	private:
		/**
		 * @brief Configure Thresholds at start Up
		 */
		void
		setUpThresholds(const wns::pyconfig::View& config);

		 /**
		 * @brief Holds the Handler to call onLinkEvent()
		 */
		wns::service::dll::mih::LinkEventHandler* linkEventHandler;

		/**
		 * @brief The FU that is providing the BER
		 */
		std::string  berProviderName;

		/**
		 * @brief Contains the current BER
		 */
		long double currentBER;

		/**
		 * @brief Shows whether I am connected to a BER Provider
		 */
		bool connected;

		/**
		 * @brief Shows whether a new Link was Detected
		 */
		bool linkDetectedFlag;

		/**
		 * @brief Sets how many tries do we wait to fire a Link Up Event
		 */
		unsigned int linkEventMeasurementWindow;

		/**
		 * @brief Shows whether a Link Connection was closed
		 */
		bool linkDownFlag;

		/**
		 * @brief Contains the values from the measurements
		 */
		std::vector<double> receivedMeasurementsList;

		/**
		 * @brief Contains the Friend FU providing measurements
		 */
		struct Friends {

			/*
			 * @brief The FU's name that I am observing
			 */
			glue::BERProvider* berProvider;
		} friends;

		/**
		 * @brief probe to record the level of BER when a Link Event occurs
		 */
        wns::probe::bus::ContextCollectorPtr linkDetectedTriggerLevel;

		/**
		 * @brief probe to record the level of BER when a Link Event occurs
		 */
        wns::probe::bus::ContextCollectorPtr linkDownTriggerLevel;

		/**
		 * @brief probe to record the level of BER for the whole simulation
		 */
        wns::probe::bus::ContextCollectorPtr berLevel;

		/**
		 * @brief Link Identifier
		 */

		wns::service::dll::mih::LinkIdentifier linkIdentifier;
		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief Registry for Link Event Thresholds.
		 *
		 */
		LinkEventThresholdRegistry linkEventThresholdRegistry;

		/**
		 * @brief PyConfig View for this FU
		 *
		 */
		wns::pyconfig::View myConfig;

		/**
		 * @brief Make sure that these functions are never called
		 */
		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doWakeup();
	};


} // mih
} // glue

#endif // NOT defined GLUE_MIH_MEASUREMENTSMONITOR_HPP
