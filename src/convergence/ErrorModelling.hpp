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

#ifndef WIMEMAC_ERRORMODELLING_HPP
#define WIMEMAC_ERRORMODELLING_HPP
#include <WNS/ldk/ErrorRateProviderInterface.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/PowerRatio.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/distribution/Uniform.hpp>
#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/container/RangeMap.hpp>

namespace wimemac { namespace convergence {
    class PhyUser;
}}

namespace wimemac { namespace convergence {

    /**
     * @brief The Command of the ErrorModelling.
     */
    class ErrorModellingCommand :
        public wns::ldk::Command,
        public wns::ldk::ErrorRateProviderInterface

    {
    public:
        ErrorModellingCommand()
        {
            local.per = 1;
            local.destructorCalled = NULL;
            local.sinr.set_dB(0);
            local.checkOK = false;

        }

        ~ErrorModellingCommand()
        {
            if(NULL != local.destructorCalled)
                *local.destructorCalled = true;
        }


        virtual double getErrorRate() const
        {
            return local.per;
        }

        virtual wns::Ratio getSINR() const
        {
            return local.sinr;
        }

        struct {
            double per;
            long *destructorCalled;
            wns::Ratio sinr;
            bool checkOK;
        } local;
        struct {} peer;
        struct {} magic;

    };

    /**
     * @brief ErrorModelling implementation of the FU.
     *
     * It maps the Carry Interference Ratio (CIR) for a PhyMode
     * to the Symbol Error Rate (SER) and calculate the
     * Packet Error Rate (PER).
     */
    class ErrorModelling :
        public wns::ldk::fu::Plain<ErrorModelling, ErrorModellingCommand>,
        public wns::ldk::Processor<ErrorModelling>
    {
    public:
        // FUNConfigCreator interface realisation
        ErrorModelling(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        /**
         * @brief Processor Interface Implementation
         */
        void processIncoming(const wns::ldk::CompoundPtr& compound);
        void processOutgoing(const wns::ldk::CompoundPtr& compound);

        /** @brief returns the maximum possible phymode for the given SINR, compoundsize and wanted PER */
        wimemac::convergence::MCS getMaxPosMCS(wns::Ratio sinr_, Bit maxCompoundSize_, double per_);
        
        /** @brief returns the the PER for command frames with given compound size */
        double getErrorRateForCommandFrames(wns::Ratio sinr_, Bit maxCompoundSize_);

        void onFUNCreated();

    private:

        /** @brief Probe Front-ends */
        wns::probe::bus::ContextCollectorPtr perProbe_;

        typedef wns::container::RangeMap<float, float> SNR2PmeanMap;
        SNR2PmeanMap snr2pmean_100_;
        SNR2PmeanMap snr2pmean_150_;
        SNR2PmeanMap snr2pmean_200_;
        SNR2PmeanMap snr2pmean_300_;
        SNR2PmeanMap snr2pmean_375_;
        SNR2PmeanMap snr2pmean_600_;
        SNR2PmeanMap snr2pmean_750_;
        SNR2PmeanMap snr2pmean_900_;

        typedef wns::container::Registry<wimemac::convergence::MCS, SNR2PmeanMap*> SNR2PmeanRegistry;
        SNR2PmeanRegistry snr2pmeanReg_;

        wns::pyconfig::View config;
        wns::logger::Logger logger;
        const std::string phyUserCommandName;
        const std::string phyUserName;
        const std::string managerCommandName;
        const std::string managerName;

        /** @brief CRC functionality */
        wns::distribution::StandardUniform dis;
        wns::container::Registry<wns::service::dll::UnicastAddress, bool> lastPreambleWasDecoded;

        wns::probe::bus::ContextCollectorPtr lossRatio;

        struct Friends
        {
            wimemac::convergence::PhyUser* phyuser;
            wimemac::lowerMAC::Manager* manager;
        } friends;

    }; // ErrorModelling
} // namespace convergence
} // namespace WiMeMac

#endif

