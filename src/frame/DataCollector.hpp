/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2011
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
 
#ifndef WIMEMAC_FRAME_DATACOLLECTOR_H
#define WIMEMAC_FRAME_DATACOLLECTOR_H

#include <WNS/ldk/fcf/CompoundCollector.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/logger/Logger.hpp>

namespace wimemac { namespace frame {




        /*
        * @brief The DataCollector is the second phase within the WiMedia superframe.
	* Its length in time depends on active devices and beacon period (BP) length. 
	* It's mandatory to send a beacon inside the BP, hence the length of the BP is variable. 
	* If the DataPhase starts, DataCollector sends a wakeup to the upper fu 
	*/




	class DataCollector :
		public wns::ldk::fcf::CompoundCollector,
	  	public wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>,
	  	public wns::ldk::HasConnector<>,
	  	public wns::ldk::HasReceptor<>,
	  	public wns::ldk::HasDeliverer<>,
	  	public wns::Cloneable<DataCollector>,
		public wns::events::CanTimeout
		
		
	{
		public:
			DataCollector(wns::ldk::fun::FUN* fun, const 
				wns::pyconfig::View& config );

			
			void doOnData(const wns::ldk::CompoundPtr& );
			void doSendData(const wns::ldk::CompoundPtr& );

			bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

			void doWakeup();

			void doStart(int);

			void doStartCollection(int){}
			void finishCollection(){}

			wns::simulator::Time getCurrentDuration() const{return getMaximumDuration();}

		protected:

			virtual void onFUNCreated();

		private:
			
			wns::logger::Logger logger;
			wns::simulator::Time duration;
			void onTimeout();
			bool sending;
			wns::pyconfig::View _config;
				
			
		};
}//frame
}//wimemac

#endif
