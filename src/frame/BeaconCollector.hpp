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
 
#ifndef WIMEMAC_FRAME_BEACONCOLLECTOR_H
#define WIMEMAC_FRAME_BEACONCOLLECTOR_H

#include <WNS/ldk/fcf/CompoundCollector.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/events/CanTimeout.hpp>
#include <WNS/logger/Logger.hpp>
#include <WIMEMAC/management/IBeaconBuilderServices.hpp>
#include <WIMEMAC/lowerMAC/IManagerServices.hpp>
#include <boost/bind.hpp>
#include <WNS/events/scheduler/Callable.hpp>

namespace wimemac {
namespace frame {



    /*
        * @brief The BeaconCollector is the first phase within the WiMedia superframe.
    * Its length in time depends on active devices. 
    * It's mandatory to send a beacon inside the BP, hence the length of the BP is variable. 
    * If BeaconPhase has started, an offset timer is set (BeaconScheduler). When the selected offset
    * time has expired, a beacon compound is created. 
    * - # sending mode: build beacon compound, send broadcast beacon
    * - # receiving mode: forward compound to BeaconBuilder for evaluating
    * If BP is over, then trigger BeaconBuilder to create a DataTransferPeriodMap (DTPmap).
    * After beacon evaluating the DTPmap holds the upcoming reservation events
    */

    class BeaconCollector :
        public wns::ldk::fcf::CompoundCollector,
        public wns::ldk::CommandTypeSpecifier<wns::ldk::EmptyCommand>,
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasReceptor<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<BeaconCollector>,
        public wns::events::CanTimeout        
    {
        public:
            BeaconCollector(wns::ldk::fun::FUN* fun, const 
                wns::pyconfig::View& config );

            void doOnData(const wns::ldk::CompoundPtr& );
            void doSendData(const wns::ldk::CompoundPtr& );

            /*@brief accept beacon compound if in sending mode only */
            bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

            void doWakeup();
            void doStart(int);

            void doStartCollection(int){}
            void finishCollection(){}

            /*@brief offset timer has expired, build beacon compound and send it */
            void TimeToTransmit();

            wns::simulator::Time getCurrentDuration() const{return getMaximumDuration();}

        protected:

            virtual void onFUNCreated();

        private:

            wns::logger::Logger logger;
            wns::simulator::Time duration;
            wns::simulator::Time BeaconSlotDuration;
            void onTimeout();
            bool gotWakeup;
            bool sending;
            int BeaconSlot;
            wns::simulator::Time BPStart;


            /*@brief BeaconScheduler calculates offset time. Offset time depends on the
            * chosen beacon slot */
            //BeaconScheduler beaconscheduler;
            const std::string managerName;

            /*@brief This upper fu creates and evaluates beacon compounds */

            struct Friends
            {
                wimemac::management::IBeaconBuilderServices* beaconbuilder;
                wimemac::lowerMAC::IManagerServices* manager;
            } friends;
    };

}
}
#endif
