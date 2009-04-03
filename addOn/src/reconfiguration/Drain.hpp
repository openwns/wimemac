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

#ifndef GLUE_RECONFIGURATION_DRAIN_HPP
#define GLUE_RECONFIGURATION_DRAIN_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	/**
	 * @brief The Drain FU simply discards any incoming or outgoing compound.
	 * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
	 */
	class Drain :
		virtual public wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<Drain>
	{
	public:
		/**
		 * @brief Constructor
		 */
		explicit
		Drain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

		/**
		 * @brief Destructor
		 */
		virtual
		~Drain();

	private:
		/**
		 * @name FunctionalUnit interface
		 */
		//@{
		virtual void
		doSendData(const wns::ldk::CompoundPtr&);

		virtual void
		doOnData(const wns::ldk::CompoundPtr&);

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const;

		virtual void
		doWakeup();
		//@}

		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_DRAIN_HPP


