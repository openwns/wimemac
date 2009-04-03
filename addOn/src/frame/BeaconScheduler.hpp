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
#ifndef WIMEMAC_FRAME_BEACONSCHEDULER_H
#define WIMEMAC_FRAME_BEACONSCHEDULER_H

#include <WNS/events/CanTimeout.hpp>
#include <WNS/logger/Logger.hpp>

namespace wimemac {namespace frame {


class TransmissionTiming
	{
	public:
		virtual
		~TransmissionTiming()
		{}

		virtual void TimeToTransmit() = 0;
		
	};



	class BeaconScheduler :
		public wns::events::CanTimeout
		
	{
		public:
			explicit
			BeaconScheduler(TransmissionTiming* transmissiontiming);
			void SetStartOffsetTime(int OffsetSlot);
	
		private:
			wns::logger::Logger logger;
			wns::simulator::Time OffsetTime;
			wns::simulator::Time SlotDuration;


			TransmissionTiming* transmissionTiming;
			void onTimeout();
			
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
