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

#ifndef GLUE_RECONFIGURATION_SIMPLEMANAGER_HPP
#define GLUE_RECONFIGURATION_SIMPLEMANAGER_HPP

#include <GLUE/reconfiguration/Manager.hpp>
#include <GLUE/trigger/FunctionalUnitLight.hpp>

#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/service/Service.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	/**
	 * @brief SimpleManager implementation of the FU interface.
	 *
	 */
	class SimpleManager :
		public Manager,
		public glue::trigger::FunctionalUnitLight,
		public wns::events::PeriodicTimeout,
		public wns::service::Service
	{
	public:
		// FUNConfigCreator interface realisation
		SimpleManager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config);

		virtual
		~SimpleManager();

		virtual int
		getCTIForSimpleAddFU();

		virtual void
		onFUNCreated();

		virtual void
		suspendedPath();

		virtual void
		peerSuspendedPath();

		virtual void
		periodically();

		virtual bool
		isMaster() const;

	private:
		void
		tryReconfiguration();

		bool master;
		SimpleManager* peerRM;
		bool isSuspending;

		bool pathSuspended;
		bool peerPathSuspended;

		int cti;

		SupportUpper* supportUpperFU;
		SupportLower* supportLowerFU;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SIMPLEMANAGER_HPP


