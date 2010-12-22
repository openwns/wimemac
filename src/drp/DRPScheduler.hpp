/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2010
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
#ifndef WIMEMAC_DRP_DRPSCHEDULER_H
#define WIMEMAC_DRP_DRPSCHEDULER_H

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/helper/Queues.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <WIMEMAC/helper/IDRPQueueInterface.hpp>
#include <WIMEMAC/drp/TempSendBuffer.hpp>

#include <WNS/events/CanTimeout.hpp>

#include <map>
#include <WNS/PowerRatio.hpp>

#include <WNS/probe/bus/utils.hpp>
#include <WNS/ldk/probe/Probe.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WIMEMAC/management/BeaconBuilder.hpp>
#include <WIMEMAC/management/BeaconCommand.hpp>
#include <WIMEMAC/lowerMAC/timing/DCF.hpp>
#include <WIMEMAC/lowerMAC/TXOP.hpp>

#include <DLL/Layer2.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WIMEMAC/TxType.hpp>
#include <WIMEMAC/lowerMAC/ITXOPWindow.hpp>

#include <WIMEMAC/management/PERInformationBase.hpp>
#include <boost/bind.hpp>
#include <WNS/events/scheduler/Callable.hpp>

//  //
namespace wimemac { namespace helper {
        class Queues;
}}
//  //
namespace wimemac { namespace drp {
         class TempSendBuffer;
}}
//  //
namespace wimemac { namespace management {
         class BeaconBuilder;
}}
//  //
namespace wimemac { namespace lowerMAC { namespace timing {
         class DCF;
}}}
//  //
namespace wimemac { namespace lowerMAC { 
         class TXOP;
}}

namespace wimemac { namespace drp {

    /** @brief Command of wifimac::lowerMAC::Manager */
    class DRPSchedulerCommand :
        public wimemac::IKnowsTxTypeCommand
    {
    public:
        struct { } local;

        struct {
            wimemac::TxType type;
            } peer;

        struct { } magic;

        DRPSchedulerCommand()
        {
            peer.type = DRP;
        }

        TxType getTxType()
        {
            return peer.type;
        }
    };


    /** @brief DRPScheduler: IDRPQueueInterface has a virtual TimeToTransmit function. This function will be called by DRPManager if
    * an own drp connection starts. The DRP Scheduler creates a buffer temporarily (intermediate buffer). 
    * This buffer will be filled according to 
    * drp timing constraints. For outgoing direction, compounds will be stored in a queue. The sending process is done by
    * intermediate buffer only. Hence each compound in outgoing direction is redirected (queue -> intermediate buffer). This is done
    * to hold a copy of each compound according to the chosen acknowledgment policy.
    * If a new compound has arrived after drp connection started, the created intermediate buffer will be informed about this event. 
    * This process will prevent the intermediate buffer from getting empty during an active drp connection.
    * If there is sending time left, the intermediate buffer will be refilled.
    * If an ACK compound has arrived, the function Acknowledgment is invoked. DRP scheduler searches for the right 
    * intermediate buffer and informs the buffer about the ACK.
    * If the DRP connection time has expired, DRPScheduler stops the intermediate buffer. 
    */

    typedef std::vector<wns::Power> PowerMap;
    typedef std::vector<bool> Vector;

    class DRPScheduler :

        public wns::ldk::fu::Plain<DRPScheduler, DRPSchedulerCommand>,
        public wimemac::helper::IDRPQueueInterface,
        public wns::events::CanTimeout,
        public wns::ldk::probe::Probe,
        public wimemac::lowerMAC::ITXOPWindow

        {
        friend class TempSendBuffer;

        public:

            DRPScheduler(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ );
            ~DRPScheduler();
            void doOnData(const wns::ldk::CompoundPtr& );

            void doSendData(const wns::ldk::CompoundPtr& );
            bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
            void doWakeup();
            void SendCompounds();
            void SendCompounds(wns::service::dll::UnicastAddress macaddress);
            void Acknowledgment(wns::service::dll::UnicastAddress tx);

            virtual void
            TimeToTransmit(wns::service::dll::UnicastAddress macaddress, wns::simulator::Time duration);

            /** @brief Get the PhyMode for a transmission partner*/
            wimemac::convergence::PhyMode
            getPhyMode(wns::service::dll::UnicastAddress rx, int masNumber);

            /** @brief Provides access to the manager for the helper classes */
            wimemac::lowerMAC::Manager*
            getManager() {return friends.manager;}

            wimemac::management::BeaconBuilder*
            getBeaconBuilder() {return friends.bb;}

            /** @brief Returns maximum allowed PER */
            double getDesiredPER() {return maxPER;}

            /** @brief Returns offset added to pattern according to PER */
            double getPatternPEROffset() {return patternPEROffset;}

            int getIsDroppingAfterRetr() {return isDroppingAfterRetr;}

            wns::service::dll::UnicastAddress
            getCurrentTransmissionTarget();

            /** @brief Check if compound is a Beacon */
            bool
            isBeacon(const wns::ldk::CommandPool* commandPool) const;

            /** @brief To inform the scheduler about an unacknowledged compound so it can calculate the PER */
            void failedAck(wns::service::dll::UnicastAddress rx);

            /** @brief Returns true if the PER is above the limit and the PhyMode should be set down */
            bool adjustMCSdown(wns::service::dll::UnicastAddress rx);

            /** @brief Updates a DRPMap with the non available slots for a specified address */
            bool UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap);

            /** @brief call to request an IE from the specified address */
            void RequestIE(wns::service::dll::UnicastAddress rx, wimemac::management::BeaconCommand::ProbeElementID elementID);

            /** @brief returns transmission duration of next compound (if any) */
            virtual wns::simulator::Time
            getNextTransmissionDuration();

            /** @brief returns receiver address of next compound (if any) */
            virtual wns::service::dll::UnicastAddress
            getNextReceiver() const;

            /** @brief invokes a PCA transmission */
            bool
            startPCAtransmission();
            void
            stopPCAtransmission();
            void
            txOPCloseIn(wns::simulator::Time duration);

            wns::simulator::Time
            getTimeUntilNextDRPReservation();

            void
            onBPStart(wns::simulator::Time BPduration);
            void
            RegisterDRPReservations(int thisMAS);

            void onDRPStart();
            void onDRPStop();

            /** @brief get the number of retransmissions for the specified compound */
            int getNumOfRetransmissions(const wns::ldk::CompoundPtr& compound);

            /** @brief Updates the map of DRP reservations */
            void UpdateDRPMap(Vector DRPMap);

        protected:

            virtual void onFUNCreated();

        private:

            wns::logger::Logger logger;
            helper::Queues* DRPQueues;
            wns::pyconfig::View config;
            typedef std::map<wns::service::dll::UnicastAddress, TempSendBuffer*> SendBufferContainer;

            int isDroppingAfterRetr;
            bool isPCAtransmissionActive;
            bool PCAcompoundsWereSent;
            wns::service::dll::UnicastAddress ActivePCArx;
            Vector currentDRPMap;
            Vector currentRegisterDRPMap;

            wns::simulator::Time nextDRPReservationTime;

            SendBufferContainer SendBuffer;
            SendBufferContainer PCABuffer;
            void onTimeout();
            bool AccessPermission;
            wns::service::dll::UnicastAddress AccessRx;

            double maxPER;
            double patternPEROffset;
            const std::string managerName;
            const std::string dcfName;
            const std::string txopName;
            const std::string beaconBuilderName;
            const std::string perMIBServiceName;

            wimemac::management::PERInformationBase* perMIB;
            wns::probe::bus::ContextCollectorPtr pcaPortionProbe;

            struct Friends
            {
                wimemac::management::BeaconBuilder* bb;
                wns::ldk::CommandReaderInterface* keyReader;
                wimemac::lowerMAC::Manager* manager;
                wimemac::lowerMAC::timing::DCF* dcf;
                wimemac::lowerMAC::TXOP* txop;
            } friends;

    };
}//drp
}//wimemac
#endif
