/******************************************************************************
 * Capability Discovery Provider                                              *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_MIH_CAPABILITYDISCOVERYPROVIDER_HPP
#define GLUE_MIH_CAPABILITYDISCOVERYPROVIDER_HPP

#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/mih/CapabilityDiscoveryService.hpp>
#include <WNS/service/dll/mih/CapabilityDiscoveryHandler.hpp>
#include <WNS/service/dll/mih/LinkIdentifier.hpp>


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
	 * @brief CapabilityDiscoveryProvider generate Link Events and sends them to the
	 * MIH Function
	 * @author Nikola Gaydarov <nig@comnets.rwth-aachen.de>
	 */
	class CapabilityDiscoveryProvider :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<CapabilityDiscoveryProvider>,
		virtual public wns::service::dll::mih::CapabilityDiscoveryService

	{
	public:
		/**
		 * @brief Constructor
		 */
		CapabilityDiscoveryProvider(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		/**
		 * @brief Destructor
		 */
		~CapabilityDiscoveryProvider();

		/**
		 * @brief Register the Capability Discovery Handler
		 */
		void
		registerHandler(wns::service::dll::mih::CapabilityDiscoveryHandler* _capabilityDiscoveryHandler);

		/**
		 * @brief sends to MIHF the list of supported Events pro link type
		 */
		void
		provideLinkEventDiscover();

		wns::service::dll::UnicastAddress
		getAddress();

		std::string
		getTechnologyName();

		std::vector<std::string>
		getModeNames();

	private:
		/**
		 * @brief Holds the Handler to call onLinkEventDiscover()
		 */
		wns::service::dll::mih::CapabilityDiscoveryHandler* capabilityDiscoveryHandler;

		/**
		 * @brief Link Identifier
		 */

		wns::service::dll::mih::LinkIdentifier linkIdentifier;

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief PyConfig View
		 */
		wns::pyconfig::View myConfig;

		/**
		 * @brief MAC Address
		 */
		wns::service::dll::UnicastAddress address;

		/** @brief The name of the DLL technology */
		std::string dllTechnologyName;

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

#endif // NOT defined GLUE_MIH_CAPABILITYDISCOVERYPROVIDER_HPP
