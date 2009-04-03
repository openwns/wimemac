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

#ifndef GLUE_RECONFIGURATION_SUPPORTUPPER_HPP
#define GLUE_RECONFIGURATION_SUPPORTUPPER_HPP

#include <GLUE/reconfiguration/SupportUpperConnector.hpp>

#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	class Manager;

	class SupportUpper :
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<SupportUpperConnector>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Processor<SupportUpper>,
		public wns::Cloneable<SupportUpper>
	{
	public:
		SupportUpper(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~SupportUpper();

		virtual void
		onFUNCreated();

		// processor interface
		virtual void
		processOutgoing(const wns::ldk::CompoundPtr& compound);

		virtual void
		processIncoming(const wns::ldk::CompoundPtr& compound);

	private:
		struct Friends
		{
			Manager* manager;
		} friends;

		wns::pyconfig::View config;
		wns::logger::Logger logger;

	}; // SupportUpper

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTUPPER_HPP


