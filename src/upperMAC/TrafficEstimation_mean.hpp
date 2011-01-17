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

#ifndef WIMEMAC_UPPERMAC_TRAFFICESTIMATION_MEAN_HPP
#define WIMEMAC_UPPERMAC_TRAFFICESTIMATION_MEAN_HPP

#include <map>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/simulator/Time.hpp>
#include <WIMEMAC/management/BeaconBuilder.hpp>
#include <WIMEMAC/lowerMAC/IManagerServices.hpp>
#include <boost/bind.hpp>
#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/events/PeriodicTimeout.hpp>

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/StaticFactory.hpp>



namespace wimemac { namespace upperMAC {

    class TrafficEstimation_mean :
        public wns::ldk::fu::Plain<TrafficEstimation_mean, wns::ldk::EmptyCommand>,
        public wns::events::PeriodicTimeout
        {
    public:

        TrafficEstimation_mean(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        virtual
        ~TrafficEstimation_mean();

    private:
        // FunctionalUnit / CompoundHandlerInterface


        virtual void
        onFUNCreated();

        virtual bool
        doIsAccepting(const wns::ldk::CompoundPtr& compound) const;


        virtual void
        doSendData(const wns::ldk::CompoundPtr& compound);

        virtual void
        doWakeup();

        virtual void
        doOnData(const wns::ldk::CompoundPtr& compound);

        //void QueueEval(wns::service::dll::UnicastAddress rx);

        virtual void
        periodically();

        const std::string managerName;

        struct Friends
        {
            wns::ldk::CommandReaderInterface* keyReader;
            wimemac::lowerMAC::IManagerServices* manager;
        } friends;

        struct config
        {
            int CompoundspSF;
            int BitspSF;
            int MaxCompoundSize;
            bool overWriteEstimation;
        }   trafficEstimationConfig;

        struct measurementDatapSF
        {
            int sentCompounds;//number of compounds sent to a specific address
            int maxCompoundSize;//the size of the biggest compound sent to a specific address
            int bitsTotal;//number of bits sent to a specific address so far
        };
        
        struct estimatedTraffic
        {
            int CompoundspSF;
            int BitspSF;
            int MaxCompoundSize;
        };
        
        std::deque<measurementDatapSF> measurementWindow;
        
        //stores the addresses of all the stations served so far and their connection parameters
        std::map<wns::service::dll::UnicastAddress, std::deque<measurementDatapSF> > windowedTrafficPerAddress;
        std::map<wns::service::dll::UnicastAddress, estimatedTraffic > currentTrafficPerAddress;
        std::map<wns::service::dll::UnicastAddress, estimatedTraffic > lastSetTrafficPerAddress;
        
        std::map<wns::service::dll::UnicastAddress, bool > NeedsNewEstimatePerAddress;

        wns::logger::Logger logger;

        wns::ldk::CompoundPtr compound;

        wns::simulator::Time maxWait;
        wns::probe::bus::ContextCollectorPtr datathroughputProbe;
        int bitsPerSF;
        wns::simulator::Time measuringSince;
        int averageOverSFs;

        wns::events::scheduler::Interface* scheduler;
        wns::events::scheduler::Callable call;

    };

} // namespace upperMAC
} // namespace wimemac

#endif // WIMEMAC_UPPERMAC_TRAFFICESTIMATION_MEAN_HPP
