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

#ifndef GLUE_RECONFIGURATION_SUPPORTUPPERCONNECTOR_HPP
#define GLUE_RECONFIGURATION_SUPPORTUPPERCONNECTOR_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Connector.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

#include <WNS/container/Registry.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	/**
	 * @brief Connector implementation for the SupportUpper FU
	 * @ingroup hasconnector
	 *
	 * This class
	 * @li implements the Connector interface,
	 * @li may be used as parameter to HasConnector.
	 *
	 */
	class SupportUpperConnector :
		public wns::ldk::Connector
	{
		typedef wns::container::Registry<int, wns::ldk::FunctionalUnit*> ContainerType;

	public:
		SupportUpperConnector();

		/**
		 * @name Connector interface
		 */
		/**
		 * @brief Checks if an acceptor for the given compound is available
		 */
		virtual bool
		hasAcceptor(const wns::ldk::CompoundPtr& compound);

		/**
		 * @brief Retrieve acceptor for a given compound
		 *
		 * @pre hasAcceptor must have returned true for this compound
		 */
		virtual wns::ldk::FunctionalUnit*
		getAcceptor(const wns::ldk::CompoundPtr& compound);

		/**
		 * @name Extended link interface
		 */
		//{@
		virtual void
		add(wns::ldk::FunctionalUnit* it);

		virtual wns::ldk::FunctionalUnit*
		getSimpleAddFU() const;

		virtual void
		add(wns::ldk::FunctionalUnit* it, int cti);

		wns::ldk::FunctionalUnit*
		getFU(int cti);

		virtual void
		remove(int cti);

		virtual void
		setAccepting(int cti);

		virtual void
		setNotAccepting();

		virtual uint32_t
		size() const;

		virtual void
		clear();

		virtual const wns::ldk::Link::ExchangeContainer
		get() const;

		virtual void
		set(const wns::ldk::Link::ExchangeContainer&);
		//@}

		virtual void
		setLogger(const wns::logger::Logger& _logger);

	private:
		ContainerType fus;

		bool simpleAddSealed;
		wns::ldk::FunctionalUnit* simpleAddFU;

		wns::ldk::FunctionalUnit* currentConnectorFU;

		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTUPPERCONNECTOR_HPP


