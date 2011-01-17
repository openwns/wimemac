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

#ifndef WIMEMAC_CONVERGENCE_PHYUSER_HPP
#define WIMEMAC_CONVERGENCE_PHYUSER_HPP

#include <WIMEMAC/lowerMAC/Manager.hpp>
#include <WIMEMAC/convergence/PhyUserCommand.hpp>
#include <WIMEMAC/convergence/PhyModeProvider.hpp>
#include <WIMEMAC/convergence/TxDurationSetter.hpp>
#include <WIMEMAC/convergence/ITxStartEnd.hpp>

#include <WIMEMAC/convergence/IPhyServices.hpp>

#include <WNS/service/dll/Address.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/events/CanTimeout.hpp>

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace wimemac { namespace lowerMAC {
         class Manager;
}}

namespace wimemac { namespace convergence {

    /**
     * @brief Convergence FU to the OFDM(A)-Module
     *
     * The PhyUser represents the lowest FU in the WiMeMAC FUN, it transates the
     * FU-Interface to the notification/request interface of the lower OFDM(A)
     * module.
     *
     * Furthermore, the PhyUser provides some Phy-dependent functions, e.g
     * access to the different PhyModes (MCSs) via the PhyModeProvider.
     */
    class PhyUser:
        public wns::ldk::fu::Plain<PhyUser, PhyUserCommand>,
        public wns::service::phy::ofdma::Handler,
        public wns::events::CanTimeout,
        public TxStartEndNotification,
        public wimemac::convergence::IPhyServices
    {

    public:
        PhyUser(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
        virtual ~PhyUser();

        /** @brief Interface to lower layer wns::service::phy::ofdma::Handler */
        void onData(wns::osi::PDUPtr, wns::service::phy::power::PowerMeasurementPtr);

        /** @brief Handling of the services */
        void setNotificationService(wns::service::Service* phy);
        wns::service::phy::ofdma::Notification* getNotificationService() const;
        void setDataTransmissionService(wns::service::Service* phy);
        wns::service::phy::ofdma::DataTransmission* getDataTransmissionService() const;

        /** @brief Handling of PhyModes */
        PhyModeProvider* getPhyModeProvider();

        /** @brief Frequency tuning */
        void setFrequency(double frequency);

        /** @brief Provides information about the rxPower and interference for a given compound */
        wns::Power
        getRxPower(const wns::ldk::CommandPool* commandPool);

        wns::Power
        getInterference(const wns::ldk::CommandPool* commandPool);

    private:

        // CompoundHandlerInterface
        virtual void doSendData(const wns::ldk::CompoundPtr& sdu);
        virtual void doOnData(const wns::ldk::CompoundPtr& compound);
        virtual void onFUNCreated();
        virtual bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
        virtual void doWakeup();

        // onTimeout realisation
        virtual void onTimeout();

        wns::pyconfig::View config;
        wns::logger::Logger logger;

        wns::service::phy::ofdma::Tune tune;
        wns::service::phy::ofdma::DataTransmission* dataTransmission;
        wns::service::phy::ofdma::Notification* notificationService;
        PhyModeProvider phyModes;

        const std::string managerName;

        /** @brief Probe Front-ends */
        wns::probe::bus::ContextCollectorPtr rxProbe_;
        wns::probe::bus::ContextCollectorPtr interferenceProbe_;
        wns::probe::bus::ContextCollectorPtr SINRProbe_;
        wns::probe::bus::ContextCollectorPtr SINRweightedProbe_;

        const std::string txDurationCommandName;
        const wns::simulator::Time txrxTurnaroundDelay;

        struct Friends
        {
            wimemac::lowerMAC::Manager* manager;
        } friends;

        enum PhyUserStatus
        {
            transmitting,
            receiving,
            txrxTurnaround
        } phyUserStatus;

        wns::ldk::CompoundPtr currentTxCompound;
    };

} // namespace convergence
} // namespace wimemac

#endif // NOT defined WIMEMAC_CONVERGENCE_PHYUSER_HPP


