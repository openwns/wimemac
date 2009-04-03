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

#ifndef GLUE_RECONFIGURATION_SUPPORTLOWER_HPP
#define GLUE_RECONFIGURATION_SUPPORTLOWER_HPP

#include <GLUE/reconfiguration/SupportLowerDeliverer.hpp>
#include <GLUE/reconfiguration/SupportLowerReceptor.hpp>

#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	class Manager;
	class Drain;

	class SupportLowerCommand :
		public wns::ldk::Command
	{
	public:
		SupportLowerCommand()
		{
			peer.cti = -1;
			magic.controlCTI = -1;
		}

		struct {} local;
		struct {
			int cti;
		} peer;
		struct {
			int controlCTI;
		} magic;
	};

	class SupportLower :
		public wns::ldk::CommandTypeSpecifier<SupportLowerCommand>,
		public wns::ldk::HasReceptor<SupportLowerReceptor>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<SupportLowerDeliverer>,
		public wns::ldk::Processor<SupportLower>,
		public wns::Cloneable<SupportLower>
	{
	public:
		SupportLower(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& _config);

		// connection setup modification
		virtual wns::ldk::FunctionalUnit*
		whenConnecting();

		virtual wns::ldk::FunctionalUnit*
		createCTISetter(int cti, int controlCTI);

		virtual void
		onFUNCreated();

		// processor interface
		virtual void
		processOutgoing(const wns::ldk::CompoundPtr& compound);

		virtual void
		processIncoming(const wns::ldk::CompoundPtr& compound);

		virtual void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const;

		virtual Manager*
		getManager() const;

	private:
		int ctiSize;

		struct Friends
		{
			Manager* manager;
			Drain* drain;
		} friends;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTLOWER_HPP


