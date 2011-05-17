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
 
#ifndef WIMEMAC_HELPER_QUEUES_H
#define WIMEMAC_HELPER_QUEUES_H

#include <WNS/service/dll/Address.hpp>

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <map>
#include <queue>
#include <WIMEMAC/helper/IQueueInterface.hpp>

#include <boost/bind.hpp>
#include <WNS/events/scheduler/Callable.hpp>


namespace wimemac {namespace helper {

class Queues:
    public IQueueInterface
{
    public:
        Queues(const wns::pyconfig::View& _config, wns::ldk::fun::FUN* fun);

        virtual
        bool isAccepting(const wns::ldk::CompoundPtr& compound) const;

        virtual
        void put(const wns::ldk::CompoundPtr& compound);

        virtual
        void putFront(const wns::ldk::CompoundPtr& compound, wns::service::dll::UnicastAddress rx);

        virtual
        wns::ldk::CompoundPtr getHeadOfLinePDU(wns::service::dll::UnicastAddress macaddress);

        virtual
        int getHeadOfLinePDUbits(wns::service::dll::UnicastAddress macaddress);

        virtual
        bool hasQueue(wns::service::dll::UnicastAddress macaddress);

        virtual
        bool queueHasPDUs(wns::service::dll::UnicastAddress macaddress);

        virtual
        std::string printAllQueues();

        virtual
        void RemoveCompounds(wns::service::dll::UnicastAddress rx);

        void
        onExtendedReservation(wns::service::dll::UnicastAddress rx);
        
        wns::service::dll::UnicastAddress
        getNextPCAReceiver();

        void
        changePCAreceiver();

    private:

        struct Friends
        {
            wns::ldk::CommandReaderInterface* keyReader;
        } friends;


        struct Queue
        {
            Queue():
            bits(0),
            isNewQueue(true)
            {}

            Bit bits;
            wns::service::dll::UnicastAddress macaddress;
            bool isNewQueue;
            bool RemoveCompounds;
            std::deque<wns::ldk::CompoundPtr> pduQueue;
        };

        long int maxSize;
        bool deleteQueues;
        wns::service::dll::UnicastAddress nextPCAReceiver;

        typedef std::map<wns::service::dll::UnicastAddress, Queue> QueueContainer;
        QueueContainer queues;

        wns::events::scheduler::Interface* scheduler;
        wns::events::scheduler::Callable call;
        wns::simulator::Time FrameDuration;
        wns::logger::Logger logger;
        wns::pyconfig::View config;

};



}//helper
}//wimemac


#endif

