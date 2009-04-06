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
#ifndef WIMEMAC_FRAME_BEACONCOLLECTOR_H
#define WIMEMAC_FRAME_BEACONCOLLECTOR_H

#include <WNS/ldk/fcf/CompoundCollector.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
//#include <WNS/events/MultipleTimeout.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/logger/Logger.hpp>
#include <WIMEMAC/frame/BeaconBuilder.hpp>
#include <WIMEMAC/frame/BeaconScheduler.hpp>


namespace wimemac {
namespace frame {

	class BeaconCollector :
		public wns::ldk::fcf::CompoundCollector,
	  	public wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>,
	  	public wns::ldk::HasConnector<>,
	  	public wns::ldk::HasReceptor<>,
	  	public wns::ldk::HasDeliverer<>,
	  	public wns::Cloneable<BeaconCollector>,
		public wns::events::PeriodicTimeout,
		public wns::events::CanTimeout,

		public virtual TransmissionTiming
	{
		public:
			BeaconCollector(wns::ldk::fun::FUN* fun, const 
				wns::pyconfig::View& config );

			void doOnData(const wns::ldk::CompoundPtr& );
			void doSendData(const wns::ldk::CompoundPtr& );

			bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

			void doWakeup();

			void doStart(int);

			void doStartCollection(int){}
			void finishCollection(){}
			virtual void TimeToTransmit();

				
			wns::simulator::Time getCurrentDuration() const{return getMaximumDuration();}

			virtual void periodically();

		protected:

			virtual void onFUNCreated();

		private:
			wns::logger::Logger logger;
			wns::simulator::Time duration;
			void onTimeout();
			bool gotWakeup;
			bool sending;
			int BeaconSlotCounter;
			int BeaconSlot;
			BeaconScheduler beaconscheduler;

			struct Friends
			{		
				wimemac::frame::BeaconBuilder* beaconbuilder;
			} friends;
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
