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

#ifndef GLUE_RECONFIGURATION_SUPPORTLOWERRECEPTOR_HPP
#define GLUE_RECONFIGURATION_SUPPORTLOWERRECEPTOR_HPP

#include <WNS/ldk/Receptor.hpp>

#include <WNS/container/Registry.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace wns { namespace ldk {

	class FunctionalUnit;

} // ldk
} // wns

namespace glue { namespace reconfiguration {

	/**
	 * @brief Receptor implementation using a CTI to select an FU to wakeup.
	 */
	class SupportLowerReceptor :
		public wns::ldk::Receptor
	{
		typedef wns::container::Registry<int, wns::ldk::FunctionalUnit*> FunctionalUnitContainer;

	public:
		SupportLowerReceptor();

		virtual void
		add(wns::ldk::FunctionalUnit* fu);

		virtual wns::ldk::FunctionalUnit*
		getSimpleAddFU() const;

		virtual void
		add(wns::ldk::FunctionalUnit* it, int cti);

		virtual void
		remove(int cti);

		virtual void
		clear();

		virtual uint32_t
		size() const;

		virtual const Link::ExchangeContainer
		get() const;

		virtual void
		set(const Link::ExchangeContainer&);

		virtual void
		wakeup();

		virtual void
		priorizeCTI(int _cti);

		virtual void
		setLogger(const wns::logger::Logger& _logger);

	private:
		FunctionalUnitContainer fus;

		bool simpleAddSealed;
		wns::ldk::FunctionalUnit* simpleAddFU;

		int priorizedCTI;

		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTLOWERRECEPTOR_HPP


