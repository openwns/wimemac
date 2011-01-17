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
 
#ifndef WIMEMAC_ARQ_ARQ_H
#define WIMEMAC_ARQ_ARQ_H
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/arq/ARQCommand.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WIMEMAC/lowerMAC/IManagerServices.hpp>
#include <DLL/UpperConvergence.hpp>

#include <WIMEMAC/drp/DRPScheduler.hpp>
#include <boost/bind.hpp>
#include <WNS/events/scheduler/Callable.hpp>

namespace wimemac { namespace arq {



        /*
        * @brief ARQ for WiMedia
    */

    class ARQ:
        public wns::ldk::fu::Plain<ARQ, ARQCommand>
    {
        public:

            ARQ(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& config_ );
            void doOnData(const wns::ldk::CompoundPtr& );
            void doSendData(const wns::ldk::CompoundPtr& );
            bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
            void doWakeup();


            bool
            hasCommandOf(const wns::ldk::CompoundPtr& compound) const;

            void
            CreateACK(const wns::ldk::CompoundPtr& compound);



        protected:

            virtual void onFUNCreated();

        private:

            wns::ldk::CompoundPtr currentBeacon;
            wns::logger::Logger logger;

            wns::service::dll::UnicastAddress ownaddress;
            wns::simulator::Time SIFS;
            wns::simulator::Time AckDuration;

            wns::ldk::fun::FUN* fun;


            wns::events::scheduler::Interface* scheduler;
            wns::events::scheduler::Callable call;

            int retrans;
            const std::string managerName;

        struct Friends
        {
            dll::UpperConvergence* Upper;
            wns::ldk::CommandReaderInterface* keyReader;
            wimemac::lowerMAC::IManagerServices* manager;
        } friends;

        void calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;
        void TransmitAck(const wns::ldk::CompoundPtr& compound);

    };
}
}
#endif
