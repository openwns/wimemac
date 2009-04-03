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

#ifndef GLUE_RECONFIGURATION_TRANSMITTINGMANAGER_HPP
#define GLUE_RECONFIGURATION_TRANSMITTINGMANAGER_HPP

#include <GLUE/reconfiguration/Manager.hpp>

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	class TransmittingManagerCommand :
		public wns::ldk::Command
	{
	public:
		typedef enum {ReconfigurationRequest,
			      ReconfigurationConfirm,
			      ProceedToReconfigure,
			      ReconfigurationFinished} FrameType;

		TransmittingManagerCommand()
		{
			peer.cti = -1;
			peer.reconfigurationScheme = -1;
		}
		struct {} local;
		struct {
			FrameType type;
			int cti;
			int reconfigurationScheme;
		} peer;
		struct {} magic;
	};

	/**
	 * @brief TransmittingManager implementation of the FU interface.
	 *
	 */
	class TransmittingManager :
		public Manager,
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<TransmittingManagerCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<TransmittingManager>,
		public wns::events::PeriodicTimeout,
		public wns::events::CanTimeout
	{
	public:
		// FUNConfigCreator interface realisation
		TransmittingManager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~TransmittingManager();

		virtual int
		getCTIForSimpleAddFU();

		virtual void
		onFUNCreated();

		virtual void
		suspendedPath();

		virtual void
		firstCompoundReceived();

		virtual void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

	private:
		virtual void
		periodically();

		virtual void
		onTimeout();

		/**
		 * @name FunctionalUnit interface
		 */
		//@{

		virtual void
		doSendData(const wns::ldk::CompoundPtr&)
		{
			throw wns::Exception("doSendData of TransmittingManager FU may not be called.");
		} // doSendData

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const
		{
			throw wns::Exception("doIsAccepting of TransmittingManager FU may not be called.");
		} // doIsAccepting

		virtual void
		doWakeup();
		//@}

		void
		tryRemovingOldPath();

		bool master;
		bool isReconfiguring;
		bool isSuspending;
		bool useOptimizedReconfigurationIfPossible;
		bool useOptimizedReconfiguration;
		bool proceedToReconfigureTimeout;

		int cti;
		int oldCTI;
		int countReconfigurations;
		wns::pyconfig::View currentReconfigurationScheme;

		wns::ldk::CompoundPtr reconfigurationRequest;
		wns::ldk::CompoundPtr reconfigurationConfirm;
		wns::ldk::CompoundPtr proceedToReconfigure;
		wns::ldk::CompoundPtr reconfigurationFinished;

		bool incomingPathReconfigured;
		bool outgoingPathReconfigured;
		bool reconfigurationFinishedFrameReceived;

		int reconfigurationRequestFrameSize;
		int frameSize;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_TRANSMITTINGMANAGER_HPP


