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

#ifndef WIMEMAC_CONVERGENCE_TXDURATIONSETTER_HPP
#define WIMEMAC_CONVERGENCE_TXDURATIONSETTER_HPP

#include <WIMEMAC/lowerMAC/Manager.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Processor.hpp>

namespace wimemac { namespace lowerMAC {
         class Manager;
}}

namespace wimemac { namespace convergence {

    class TxDurationSetterCommand:
        public wns::ldk::Command
    {
    public:
        TxDurationSetterCommand()
        {
            local.txDuration = 0;
        };

        struct {
            wns::simulator::Time txDuration;
        } local;

        struct {} peer;
        struct {} magic;

        wns::simulator::Time getDuration() const { return local.txDuration; }
    };

    /**
     * @brief Sets the transmission duration in any outgoing compound.
     *
     * Works for both preambles and PSDUs.
     */
    class TxDurationSetter :
        public wns::ldk::fu::Plain<TxDurationSetter, TxDurationSetterCommand>,
        public wns::ldk::Processor<TxDurationSetter>
    {
    public:

        TxDurationSetter(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        virtual
        ~TxDurationSetter();

    private:
        /** @brief Processor Interface */
        void processIncoming(const wns::ldk::CompoundPtr& compound);
        void processOutgoing(const wns::ldk::CompoundPtr& compound);

        void onFUNCreated();

        const std::string managerName;
        wns::logger::Logger logger;

        void calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

        struct Friends
        {
            wimemac::lowerMAC::Manager* manager;
        } friends;
    };


} // convergence
} // wimemac

#endif // WIMEMAC_CONVERGENCE_TXDURATIONSETTER_HPP
