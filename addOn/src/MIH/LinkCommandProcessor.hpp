/******************************************************************************
 * Link Command Processor                                                     *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_MIH_LINKCOMMANDPROCESSOR_HPP
#define GLUE_MIH_LINKCOMMANDPROCESSOR_HPP

#include <WNS/service/dll/mih/LinkIdentifier.hpp>
#include <WNS/service/dll/mih/LinkCommandProcessor.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/mih/ScanResponseSet.hpp>

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
	 * @brief Service definition for components that offer Data Link Layer
	 * command transmission service.
	 */
	class LinkCommandProcessor :
		virtual public wns::service::dll::mih::LinkCommandProcessor,
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<LinkCommandProcessor>
	{
	public:
		/**
		 * @brief Constructor
		 */
		LinkCommandProcessor(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		/**
		 * @brief Destructor
		 */
		~LinkCommandProcessor();

		/**
		 * @brief Perform some actions when all the FUs are created
		 */
		virtual void
		onFUNCreated();

		/**
		 * @brief Receives Commands From MIH Function
		 * In the moment it is doing nothing, but it is needed for MIH
		 * Function Component to work properly
		 */
		void
		linkConnect(const wns::service::dll::mih::ConnectionIdentifier& connectionIdentifier);

		void
		linkDisconnect(const wns::service::dll::mih::ConnectionIdentifier& connectionIdentifier);

		wns::service::dll::mih::ScanResponseSet
		scanLink(const wns::service::dll::mih::LinkIdentifier& linkIdentifier);

		wns::service::dll::mih::ScanResponseSet
		scanAllLinks();

		void
		configureThresholds(const wns::service::dll::mih::ConnectionIdentifier& connectionIdentifier);

	private:

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;

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

#endif // GLUE_MIH_LINKCOMMANDPROCESSOR_HPP

