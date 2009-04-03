/******************************************************************************
 * WiMeMAC                                                                    *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2009                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                   *
 * email: hor@comnets.de, www: http://wns.comnets.rwth-aachen.de/             *
 *****************************************************************************/
#ifndef WIMEMAC_DRP_DRPSCHEDULER_H
#define WIMEMAC_DRP_DRPSCHEDULER_H

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/helper/Queues.hpp>

#include <WIMEMAC/helper/IDRPQueueInterface.hpp>

namespace wimemac {
namespace drp {

	class DRPScheduler :
		
		public wns::ldk::fu::Plain<DRPScheduler, wns::ldk::EmptyCommand>,
 		public wimemac::helper::IDRPQueueInterface
					
	  {
		public:

			DRPScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ );
			void doOnData(const wns::ldk::CompoundPtr& );
			void doSendData(const wns::ldk::CompoundPtr& );
			bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
			void doWakeup();

			virtual void
			TimeToTransmit(wns::service::dll::UnicastAddress macaddress, wns::simulator::Time duration) const;

		protected:

			virtual void onFUNCreated();

		private:
			
			wns::logger::Logger logger;
			helper::Queues* DRPQueues;
			wns::pyconfig::View config;
		
				
		
	};
}
}
#endif

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
