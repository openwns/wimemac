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

#ifndef GLUE_RECONFIGURATION_COMPOUNDBACKTRACKER_HPP
#define GLUE_RECONFIGURATION_COMPOUNDBACKTRACKER_HPP

#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	/**
	 * @brief Shows backtraces of incoming and outgoing Compounds
	 *
	 * This FU may be inserted at any position within a FUN to show
	 * compound backtraces of incoming and outgoing Compounds which are
	 * passing this FU.
	 */
	class CompoundBacktracker :
		public wns::ldk::Processor<CompoundBacktracker>,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<CompoundBacktracker>
	{
	public:
		CompoundBacktracker(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~CompoundBacktracker();

	private:
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		processIncoming(const wns::ldk::CompoundPtr& compound);

		virtual void
		processOutgoing(const wns::ldk::CompoundPtr& compound);

		bool backtraceIncoming;
		bool backtraceOutgoing;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_COMPOUNDBACKTRACKER_HPP


