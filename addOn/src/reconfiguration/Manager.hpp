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

#ifndef GLUE_RECONFIGURATION_MANAGER_HPP
#define GLUE_RECONFIGURATION_MANAGER_HPP

#include <WNS/ldk/SuspendedInterface.hpp>
#include <WNS/ldk/Link.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace ldk { namespace fun {
	class FUN;
	class SuspendableInterface;
} // fun
} // ldk
} // wns

namespace glue { namespace reconfiguration {

	class SupportUpper;
	class SupportLower;

	/**
	 * @brief Manager implementation of the FU interface.
	 *
	 */
	class Manager :
		virtual public wns::ldk::SuspendedInterface
	{
		typedef wns::ldk::Link::ExchangeContainer FUContainer;

		class RaceConditionSolver :
			public wns::events::CanTimeout
		{
		public:
			RaceConditionSolver(Manager* _manager);

			virtual void
			solveRaceCondition();

			virtual void
			onTimeout();

		private:
			Manager* manager;
		};

	public:
		// FUNConfigCreator interface realisation
		Manager(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config);

		virtual
		~Manager();

		// These functions are called by friend FUs and should not be
		// used by derived classes
		virtual void
		setSupportUpper(SupportUpper* su);

		virtual void
		setSupportLower(SupportLower* sl);

		virtual int
		getControlCTIForSimpleAddFU() const;

		virtual void
		suspended(const wns::ldk::SuspendableInterface* si);

		virtual void
		raceConditionSolved();

		// Use these functions in derived classes to control the
		// reconfiguration process
		virtual void
		setNotAccepting();

		virtual void
		setAccepting(int cti);

		virtual void
		priorizeCTI(int priorizeCTI);

		virtual void
		createPath(int cti, const wns::pyconfig::View& config);

		virtual void
		removePath(int cti);

		virtual void
		suspendPath(int cti);

		virtual void
		cancelSuspendPath();

		virtual void
		wakeupPath();

		virtual void
		notifyWhenReceived(int cti);

		// Implement resp. overload these functions in derived classes
		virtual void
		firstCompoundReceived();

		virtual void
		receivedCTI(int cti);

		virtual int
		getCTIForSimpleAddFU() = 0;

		virtual void
		suspendedPath() = 0;

	private:
		wns::ldk::fun::FUN* fun;

		struct Friends
		{
			SupportUpper* supportUpperFU;
			SupportLower* supportLowerFU;
		} friends;

		bool suspendingPath;
		FUContainer suspendFUs;
		wns::ldk::SuspendableInterface* suspendingFU;

		int controlCTI;

		RaceConditionSolver rcs;

		wns::pyconfig::View config;
		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_MANAGER_HPP


