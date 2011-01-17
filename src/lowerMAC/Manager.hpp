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

#ifndef WIMEMAC_LOWERMAC_MANAGER_HPP
#define WIMEMAC_LOWERMAC_MANAGER_HPP

#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WIMEMAC/convergence/PhyMode.hpp>
#include <WIMEMAC/FrameType.hpp>
#include <WIMEMAC/drp/DRPScheduler.hpp>
#include <WIMEMAC/convergence/ErrorModelling.hpp>
#include <WIMEMAC/management/ProtocolCalculator.hpp>
#include <WIMEMAC/lowerMAC/IManagerServices.hpp>

#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/service/dll/Address.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <DLL/Layer2.hpp>
#include <DLL/UpperConvergence.hpp>

#include <list>

namespace wimemac {
namespace lowerMAC {

    class IManagerServices;
}

namespace convergence {
    class PhyUser;
    class ErrorModelling;
}}

namespace wimemac { namespace drp {
    class DRPScheduler;
}}

namespace wimemac { namespace lowerMAC {

    typedef std::vector<bool> Vector;
  
    /** @brief Command of wimemac::lowerMAC::Manager */
    class ManagerCommand :
        public wimemac::IKnowsFrameTypeCommand
    {
    public:
        struct {
            wns::simulator::Time expirationTime;
        } local;

        struct {
            bool hasPayload;
            wimemac::CompoundType CompoundType;
            wimemac::convergence::PhyMode phyMode;

            /**
             * @brief Duration field
             * i.e. the duration of the frame exchange, without the current
             * frame!
             */
            wns::simulator::Time frameExchangeDuration;
            wns::simulator::Time psduDuration;

            /** @brief Frame requires a direct reply from peer */
            bool requiresDirectReply;
        } peer;

        struct { } magic;

        ManagerCommand()
        {
            peer.CompoundType = DATA;
            peer.hasPayload = true;
            peer.frameExchangeDuration = 0;
            peer.psduDuration = 0;
            peer.requiresDirectReply = false;
        }

        CompoundType getCompoundType()
        {
            return peer.CompoundType;
        }

        std::string getPreambleMode()
        {
            return this->peer.phyMode.getPreambleMode();
        }

        bool isPreamble() const
        {
            return (peer.CompoundType == ACK_PREAMBLE || peer.CompoundType == BEACON_PREAMBLE || peer.CompoundType == DATA_PREAMBLE);
        }

        wimemac::convergence::PhyMode getPhyMode() const
        {
            return this->peer.phyMode;
        }
    };

    /**
    * @brief Management Entity for a single IEEE 802.11 transceiver, i.e. the
     *        lower MAC
     *
     * Here, all management information related to a single IEEE 802.11
     * transceiver is stored and accessible for
     *  - FUs inside this transceiver
     *  - Higher FUs outside of a transceiver implementation
     */

    class Manager :
        public wns::ldk::fu::Plain<Manager, ManagerCommand>,
        public wns::ldk::Processor<Manager>,
        public wimemac::lowerMAC::IManagerServices


    {
    public:

        Manager(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

        virtual
        ~Manager();

        /** @brief Process incoming compounds: Do Nothing at all*/
        void processIncoming(const wns::ldk::CompoundPtr& compound);

        /** @brief Process outgoing compounds: Activate command, set type to
         * DATA and frame exchange duration to SIFS+ACK */
        void processOutgoing(const wns::ldk::CompoundPtr& compound);

        bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

        /** @brief Returns the phyUser*/
        wimemac::convergence::PhyUser*
        getPhyUser();

        /** @brief Access the stationType */
        dll::Layer2::StationType
        getStationType() const;

        /** @brief Returns the MAC address of this transceiver. Do not use the
         * similar function from the dll::UpperConvergence - this will give you
         * the MAC address of the complete Layer2 */
        wns::service::dll::UnicastAddress
        getMACAddress() const;

        /** @brief Gets used methods */
        bool getRateAdaptation() {return useRateAdaptation;}
        int getReservationBlocks() {return reservationBlocks;}
        bool getRandomUse() {return useRandomPattern;}

        bool getDRPchannelAccess() {return useDRPchannelAccess;}
        bool getPCAchannelAccess() {return usePCAchannelAccess;}

        /** @brief Create-reply function */
        wns::ldk::CommandPool*
        createReply(const wns::ldk::CommandPool* original) const;

        /** @brief Create-reply and set frametype function*/
        wns::ldk::CommandPool*
        createReply(const wns::ldk::CommandPool* original, wimemac::CompoundType compoundType) const;

        /** @brief Helper function for all FUs in this transceiver to create a
         * new compound with given addresses, type, duration */
        wns::ldk::CompoundPtr
        createCompound(const wns::service::dll::UnicastAddress transmitterAddress,
                       const wns::service::dll::UnicastAddress receiverAddress,
                       const CompoundType compoundType,
                       const bool hasPayload,
                       const wns::simulator::Time frameExchangeDuration,
                       const bool requiresDirectReply = false);

        /** @brief Getter for the transmitter address of the compound*/
        wns::service::dll::UnicastAddress
        getTransmitterAddress(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Getter for the receiver address of the compound*/
        wns::service::dll::UnicastAddress
        getReceiverAddress(const wns::ldk::CommandPool* commandPool) const;

        /** @brief True if a received frame is intended for me */
        bool
        isForMe(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Check if Compound has payload */
        bool
        hasPayload(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Sets hasPayload bit */
        void
        setHasPayload(const wns::ldk::CommandPool* commandPool, bool setPayloadTo);

         /** @brief Get the compound type */
        wimemac::CompoundType
        getCompoundType(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Get Preamble Mode */
        std::string
        getPreambleMode(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Set the compound type */
        void
        setCompoundType(const wns::ldk::CommandPool* commandPool, const CompoundType type);

        /** @brief Check if compound type is PREAMBLE */
        bool
        isPreamble(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Check if compound is a Beacon */
        bool
        isBeacon(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Get the frame exchange duration
         *
         *  The frame exchange duration denotes the expected duration of the
         *  complete frame exchange, starting from the successful reception of
         *  the compound where this duration is stored. E.g. an RTS contains the
         *  sum of SIFS, CTS, SIFS, DATA, ACK. Nodes overhearing this duration
         *  will set their NAV accordingly.
         *
         */
        wns::simulator::Time
        getFrameExchangeDuration(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Set the frame exchange duration */
        void
        setFrameExchangeDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration);

        /** @brief Get the PSDU duration
         *
         *  The PSDU Duration is used in the preamble to store the duration of the following PSDU
         *  for synchronization purposes
         *
         */

        wns::simulator::Time
        getpsduDuration(const wns::ldk::CommandPool* commandPool) const;

        void
        setpsduDuration(const wns::ldk::CommandPool* commandPool, const wns::simulator::Time duration);

        /** @brief Get the PhyMode from a compound*/
        wimemac::convergence::PhyMode
        getPhyMode(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Set the PhyMode using wimemac::convergence::PhyMode */
        void
        setPhyMode(const wns::ldk::CommandPool* commandPool, const wimemac::convergence::PhyMode phyMode);

        /** @brief Get the default PhyMode*/
        wimemac::convergence::PhyMode
        getDefaultPhyMode() const;

        /** @brief Provides information about the rxPower and interference for a given compound */
        wns::Power
        getRxPower(const wns::ldk::CommandPool* commandPool);

        wns::Power
        getInterference(const wns::ldk::CommandPool* commandPool);

        /** @brief Returns the maximum possible MCS usable for the given SINR, PER and compoundsize */
        wimemac::convergence::MCS getMaxPosMCS(wns::Ratio sinr_, Bit maxCompoundSize_, double per_);
        
        /** @brief Returns the per for given sinr and lowest phy mode */
        double getErrorRateForCommandFrames(wns::Ratio sinr_, Bit maxCompoundSize_);

        /** @brief Returns maximum allowed PER */
        double getDesiredPER();

        /** @brief Returns offset added to pattern according to PER */
        double getPatternPEROffset();

        /** @brief Get the "requires direct reply" flag
         *
         *  If a compound requires a direct (i.e. after a short, constant
         *  duration) reply, this bit is set. The receiving node must stop all
         *  other transmission attempts and generate the accorind reply.
         *
         *  An examples is the RTS (requires a CTS after SIFS)
         *
         */
        bool
        getRequiresDirectReply(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Set the "requires direct reply" flag */
        void
        setRequiresDirectReply(const wns::ldk::CommandPool* commandPool, bool requiresDirectReply);

        /** @brief Returns if the lifetime limit of the given msdu is expired */
        bool
        lifetimeExpired(const wns::ldk::CommandPool* commandPool) const;

        /** @brief Returns the expiration time of the msdu*/
        wns::simulator::Time
        getExpirationTime(const wns::ldk::CommandPool* commandPool) const;

        void
        setBPStartTime(wns::simulator::Time bpStartTime_) {BPStartTime = bpStartTime_;}

        wns::simulator::Time
        getBPStartTime() { return BPStartTime;}

        int
        getMASNumber(wns::simulator::Time time_);

        wimemac::management::ProtocolCalculator* getProtocolCalculator();

        // For BeaconBuilder Services
        void prepareDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize);

        void updateDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize);
        
        void BuildDTPmap();
        
        void SetBPDuration(wns::simulator::Time duration);
        
        // For DRPScheduler Services
        bool startPCAtransmission();
        void stopPCAtransmission();
        void txOPCloseIn(wns::simulator::Time duration);
        
        wns::service::dll::UnicastAddress getCurrentTransmissionTarget();
        
         /** @brief get the number of retransmissions for the specified compound */
        int getNumOfRetransmissions(const wns::ldk::CompoundPtr& compound);
        
         /** @brief Updates a DRPMap with the non available slots for a specified address */
        bool UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap);
        
        /** @brief Returns true if the PER is above the limit and the PhyMode should be set down */
        bool adjustMCSdown(wns::service::dll::UnicastAddress rx);
        
        /** @brief Updates the map of DRP reservations */
        void UpdateDRPMap(Vector DRPMap);
        
        void onBPStart(wns::simulator::Time BPduration);
        
        void Acknowledgment(wns::service::dll::UnicastAddress tx);
        
        
    private:
        virtual void
        onFUNCreated();

        const wns::pyconfig::View config_;
        wns::logger::Logger logger_;
        /** @brief Expected duration of an ACK frame*/
        const wns::simulator::Time expectedACKDuration;

        /** @brief storing the different channel configurations, in case of multi channel devices*/

        /** @brief Short Interframce Space duration */
        const wns::simulator::Time sifsDuration;

        /** @brief my MAC address, as given by the upper convergence */
        const wns::service::dll::UnicastAddress myMACAddress_;

        /** @brief Name of the upper convergence */
        const std::string ucName_;

        /** @brief Lifetime limit of MSDUs*/
        const wns::simulator::Time msduLifetimeLimit;

        /** @brief number of reservation blocks */
        int reservationBlocks;

        /** @brief Implementation method dependent parameters */
        bool useRandomPattern;
        bool useRateAdaptation;

        /** @brief Channel access to be used */
        bool useDRPchannelAccess;
        bool usePCAchannelAccess;

        wns::simulator::Time BPStartTime;
        const std::string protocolCalculatorName;
        wns::probe::bus::ContextCollectorPtr mcsProbe;

        wimemac::management::ProtocolCalculator* protocolCalculator;

        struct Friends
        {
            dll::UpperConvergence* upperConvergence;
            wimemac::convergence::PhyUser* phyUser;
            wimemac::drp::DRPScheduler* drpScheduler;
            wimemac::convergence::ErrorModelling* errorModelling;
        } friends;
    };
} // lowerMAC
} // wimemac

#endif // WIMEMAC_LOWERMAC_MANAGER_HPP
