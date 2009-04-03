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

#ifndef GLUE_RECONFIGURATION_SUPPORTLOWERDELIVERER_HPP
#define GLUE_RECONFIGURATION_SUPPORTLOWERDELIVERER_HPP

#include <WNS/ldk/Deliverer.hpp>

#include <WNS/container/Registry.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

namespace glue { namespace reconfiguration {

	class SupportLower;

	/**
	 * @brief Deliverer implementation using a CTI to select an FU for delivery.
	 */
	class SupportLowerDeliverer :
		public wns::ldk::Deliverer
	{
		typedef wns::container::Registry<int, wns::ldk::FunctionalUnit*> FunctionalUnitContainer;
		typedef wns::container::Registry<int, int> IntContainer;

	public:
		SupportLowerDeliverer();

		virtual void
		add(wns::ldk::FunctionalUnit* fu);

		virtual wns::ldk::FunctionalUnit*
		getSimpleAddFU() const;

		virtual void
		add(wns::ldk::FunctionalUnit* it, int cti, int controlCTI);

		wns::ldk::FunctionalUnit*
		getFU(int cti);

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

		virtual wns::ldk::CompoundHandlerInterface*
		getAcceptor(const wns::ldk::CompoundPtr& compound);

		virtual void
		setDrainFU(wns::ldk::FunctionalUnit* _drainFU);

		virtual void
		setCTICommandProvider(SupportLower* sl);

		virtual void
		notifyWhenReceived(int cti);

		virtual void
		setLogger(const wns::logger::Logger& _logger);

	private:
		FunctionalUnitContainer fus;
		IntContainer controlCTIs;

		bool simpleAddSealed;
		wns::ldk::FunctionalUnit* simpleAddFU;

		wns::ldk::FunctionalUnit* drainFU;

		struct Friends
		{
			SupportLower* supportLowerFU;
		} friends;

		bool firstCompound;
		int notifyCTI;

		wns::logger::Logger logger;
	};

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_SUPPORTLOWERDELIVERER_HPP


