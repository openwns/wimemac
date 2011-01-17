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

#ifndef WIMEMAC_LOWERMAC_TXOP_HPP
#define WIMEMAC_LOWERMAC_TXOP_HPP

#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WIMEMAC/lowerMAC/IManagerServices.hpp>
#include <WIMEMAC/lowerMAC/ITXOPWindow.hpp>
#include <WIMEMAC/lowerMAC/ITXOPObserver.hpp>
#include <WIMEMAC/management/ProtocolCalculator.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/Delayed.hpp>
#include <WNS/ldk/probe/Probe.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/Observer.hpp>

namespace wimemac { namespace lowerMAC {
     /** 
     * @brief FU implementing TXOP functionality
     *
     * this FU mainly is responsible for setting the NAV of outgoing compounds
     * correctly, according to 802.11 standard.
     * In order to calculate the NAV for a given compound, it uses an FU
     * implementing the ITXOPTimeWindow interface to determine the duration of a possible subsequent transmission
     * and wether it fits into the remaining TXOP duration or not.
     * If this is the case, the NAV of the current compound is set to cover
     * its successor. Otherwise, the current TXOP is closed, another round
     * is initiated. 
     * To disable TXOP the TXOP limit has to be set to 0
     */
    class TXOP:
        public wns::ldk::fu::Plain<TXOP, wns::ldk::EmptyCommand>,
        public wns::ldk::Processor<TXOP>,
    public wns::ldk::probe::Probe
    {
    public:

        TXOP(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        virtual
        ~TXOP();

        // observer TxStartEnd
        void
        onTxStart(const wns::ldk::CompoundPtr& compound);
        void
        onTxEnd(const wns::ldk::CompoundPtr& compound);

        /** @brief register observer which gets called when the current TXOP round is closed
        */
        void
        registerObserver(wimemac::lowerMAC::ITXOPObserver *observer) {observers.push_back(observer);}

        void closeTXOP(bool forwardCall = true);
        void setTXOPLimit(wns::simulator::Time limit);
    private:
        /** @brief Processor Interface Implementation */
        void processIncoming(const wns::ldk::CompoundPtr& compound);
        void processOutgoing(const wns::ldk::CompoundPtr& compound);

        bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

        void onFUNCreated();

        /** @brief send probe values, call observers ... */

        const std::string managerName;
        const std::string protocolCalculatorName;
        const std::string txopWindowName;
        //const std::string raName;

        /** @brief Duration of the Short InterFrame Space */
        const wns::simulator::Time sifsDuration;
        const wns::simulator::Time maximumACKDuration;
        const bool singleReceiver;

        /** @brief indicates wether NAV should be set to cover complete TXOP window or not */
        bool maxOutTXOP;

        wns::simulator::Time txopLimit;
        wns::simulator::Time remainingTXOPDuration;
        wns::simulator::Time lastFTDuration;
        wns::service::dll::UnicastAddress txopReceiver;

        wns::logger::Logger logger;

        std::vector<wimemac::lowerMAC::ITXOPObserver *> observers;
        wimemac::management::ProtocolCalculator* protocolCalculator;
        struct Friends
        {
            wimemac::lowerMAC::IManagerServices* manager;
            wimemac::lowerMAC::ITXOPWindow* txopWindow;
            //wimemac::lowerMAC::RateAdaptation* ra;
        } friends;

    wns::probe::bus::ContextCollectorPtr TXOPDurationProbe;
    };

} // lowerMAC
} // wimemac

#endif // WIMEMAC_LOWERMAC_TXOP_HPP
