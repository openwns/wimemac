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

#ifndef GLUE_RECONFIGURATION_SUPPORTLOWERCTISETTER_HPP
#define GLUE_RECONFIGURATION_SUPPORTLOWERCTISETTER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/SuspendableInterface.hpp>
#include <WNS/ldk/SuspendSupport.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	class SupportLower;

	class SupportLowerCTISetter :
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Processor<SupportLowerCTISetter>,
		virtual public wns::ldk::SuspendableInterface,
		public wns::ldk::SuspendSupport,
		public wns::Cloneable<SupportLowerCTISetter>
	{
	public:
		SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config, SupportLower* supportLower);
		SupportLowerCTISetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config, SupportLower* supportLower, int cti, int controlCTI);

		virtual void
		onFUNCreated();

		// Processor interface
		virtual void
		processOutgoing(const wns::ldk::CompoundPtr& compound);

		virtual void
		processIncoming(const wns::ldk::CompoundPtr& compound);

		virtual void
		setCTI(int _cti, int _controlCTI);

		virtual bool
		onSuspend() const;

	private:
		int cti;
		int controlCTI;

		struct Friends
		{
			SupportLower* supportLower;
		} friends;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTLOWERCTISETTER_HPP


