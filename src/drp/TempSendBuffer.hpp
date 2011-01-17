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

#ifndef WIMEMAC_DRP_TEMPSENDBUFFER_H
#define WIMEMAC_DRP_TEMPSENDBUFFER_H

#include <WNS/service/dll/Address.hpp>
#include <queue>
#include <WNS/simulator/Time.hpp>
#include <WIMEMAC/helper/Queues.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/events/CanTimeout.hpp>

namespace wimemac { namespace helper {
         class Queues;
}}

namespace wimemac {
namespace drp {
class TempSendBuffer;
class DRPScheduler;


    class TempSendBuffer:
        public wns::events::CanTimeout
    {
    public:
        TempSendBuffer(helper::Queues* _DRPQueues,wns::service::dll::UnicastAddress _target,
        DRPScheduler* _drpscheduler, wns::logger::Logger logger);

        void SetTxopDuration(wns::simulator::Time duration_);
        int CalculateMaxSize();
        int GetCompoundSize();
        bool StartBuffering();
        bool FillBuffer();
        bool CalculateTxTime(int size, wns::simulator::Time &duration);
        wns::simulator::Time CalculateTxTime(int size);
        bool IsBufferEmpty() const;
        wns::ldk::CompoundPtr GetCompound();
        void TransmitCompound(const wns::ldk::CompoundPtr& compound);
        void ImmAck();
        void StopBuffering();
        void NewArrival();
        int numCompounds();
        wns::simulator::Time txBufferTime();

        int GetNumOfTotalRetransmissions();
        int GetNumOfRetransmissions(const wns::ldk::CompoundPtr& compound);
        int GetNumOfRetransmissions();
        wns::service::dll::UnicastAddress GetTarget();
        Bit getNextCompoundSize();
        Bit getCurrentCompoundSize();

    private:
        enum AckType {
            No, Imm, Block
        };

        struct TempBuffer
        {
            Bit bits;
            wns::simulator::Time timeleft;
            wns::service::dll::UnicastAddress target;
            std::deque<wns::ldk::CompoundPtr> pduQueue;
        } tempBuffer;

        wns::simulator::Time txOPTime;
        wns::simulator::Time txOPStartTime;
        int firstMASOfTxOP;
        helper::Queues* DRPQueues;
        wns::logger::Logger logger;

        bool isWaitingSIFS;
        bool isTransmissionStarted;
        int numOfRetransmissions;
        int numTotalRetransmissions;

        int sentCompoundsInLastBufferAction;
        int successfullCompoundsInLastBufferAction;
        int sentCompoundsInLastTxOP;
        int successfullCompoundsInLastTxOP;

        const wns::simulator::Time GuardDuration;
        const wns::simulator::Time TxAckDuration;
        const wns::simulator::Time SIFS;

        DRPScheduler* drpscheduler;

        void SendCompounds();
        void onTimeout();

    };
}//helper
}//wimemac
#endif
