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
#ifndef WIMEMAC_DRP_DRPPATTERNCREATOR_H
#define WIMEMAC_DRP_DRPPATTERNCREATOR_H

#include <vector>
#include <WNS/service/dll/Address.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WIMEMAC/convergence/PhyMode.hpp>


namespace wimemac { namespace lowerMAC {
    class Manager;
}}

namespace wimemac { namespace drp {
typedef std::vector<bool> Vector;
  
    class DRPPatternCreator
    {
    public:

            DRPPatternCreator(wns::logger::Logger logger_, double patternPEROffset_, wns::service::dll::UnicastAddress TargetAddress_);

            void SetReservationGap(int SlotAmount);
            void SetNIBp6S(int BitAmountMCS);
            void SetNumberOfBPSlots(int numberOfBPSlots_);
            void SetPhyMode(wimemac::convergence::PhyMode phyMode_);
            wimemac::convergence::PhyMode getPhyMode();
            void SetTrafficChar(int PacketpFrame, int BitpFrame, int MaxCompoundSize, int reservationBlocks);
            void UpdateTrafficChar();
            void UpdateTrafficChar(int CompoundspSF, int BitspSF, int MaxCompoundSize);
            bool GetPattern(Vector& ProposedPattern, Vector& GlobalDRPMap, bool useInperfectPatterns);
            bool AddPattern(Vector& ProposedPattern, Vector& DRPAllocMap, Vector& GlobalDRPMap, bool useInperfectPatterns);
            bool CreateRandomPattern(Vector& ProposedPattern, Vector& DRPAllocMap, Vector& ConstraintDRPMap, bool useInperfectPatterns);
            void InitVector(Vector& InitMap);


        protected:

            struct Friends
            {
                wimemac::lowerMAC::Manager* manager;
            } friends;

            int
            CalcMissingPackets(Vector& allocMap_);

            int SlotpSF; //Slot per superframe
            int AdjSlot; //direct adjacent slots to transmit generated traffic during area-time
            int mAdjSlot; //mandatory adjacent slots to transmit one packet
            int ReservationGap; //number of unoccupied slots between reserved slots
            int DivideSFintoAreas; // number of areas the SF should be divided into
            int PacketpSF; //number of packtes generated during superframe time
            int PacketpArea;
            int BitpP; //number of bits per packet
            int MaxPacketSize; // maximum packet size
            int NumberOfBPSlots; // number of slots reserved for BP
            wimemac::convergence::PhyMode phyMode;
            wns::simulator::Time psduDuration;
            wns::simulator::Time FTDuration;
            wns::simulator::Time DataDuration;

            double patternPEROffset; // additional percentage added to pattern according to PER
            wns::service::dll::UnicastAddress TargetAddress;

            const wns::simulator::Time PreambPlusHeadDuration;
            const wns::simulator::Time ACKduration;
            const wns::simulator::Time SIFSduration;
            const wns::simulator::Time SlotDuration;
            const wns::simulator::Time GuardDuration;


        private:

            //compare proposed reservation pattern with the GlobalDRPMap (already reserved slots)
            bool
            IsPatternValid(Vector& ProposedMap) const;

            //Calculate the number of areas with reserved slots
            int
            CalcNumArea(Vector& ProposedMap);

            //search for a possible reservation with the highest number of reserved slots
            void
            CalcMaxPropMap(Vector& tmpProposedPattern);

            //Remove slots from proposal if there is a conflict with the GlobalDRPMap
            bool
            ClearConflictArea();

            //If there is a conflict, reserve slots in this area outside desired position
            bool
            AllocAreaOutside(int first, int last, int CalcAdjSlot);

            //Show areas without a reservation
            bool
            FindEmptyArea(Vector& tmpProposedPattern, int first, int last);

            //look if there is space left to reserv slots outside the desired position
            bool
            IsSpaceLeft(int first, int last,int CalcAdjSlot) const;

            //calculate packets which can be transmitted inside this area bounded by [first,last)
            int
            CalcPacketpArea(int first, int last);

            bool
            EvaluateReservation();

            void
            BestEfficency(int MissingReservation);

            int
            GapFilling(int MissingReservation);

            // Functions for multi stream handling
            void
            CalcMaxPosMCSPerMAS();

            Vector
            GetDRPMapWithPhyModeBetter(Vector& allocMap_, wimemac::convergence::PhyMode lowestAllowedPhy_);

            bool
            PatternHasFreeSlots(Vector pattern);

            wns::logger::Logger logger;

            //holds already reserved slots
            Vector GlobalDRPMap;

            struct MaxPosPattern
            {
                MaxPosPattern():
                conflict(0),
                maxProposedPattern(256,false)
                {}

                int conflict;
                Vector maxProposedPattern;

                void SetVector(Vector maxVec)
                {
                    for(int i = 0; i < maxVec.size(); i++)
                    {
                        assure(i < maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                        maxProposedPattern[i] = maxVec[i];
                    }
                }
            };


            struct MaxEfficiency
            {
                MaxEfficiency():
                    AddSlot(0),
                    iFragmentDuration(0),
                    area(-1),
                    AdditionalPackets(0),
                    maxProposedPattern(256,false)
                {}

                Vector maxProposedPattern;

                void SetVector(Vector maxVec)
                {
                    for(int i = 0; i < maxVec.size(); i++)
                    {
                        assure(i < maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                        maxProposedPattern[i] = maxVec[i];
                    }
                }
                void Clear()
                {
                    for(int i = 0; i < maxProposedPattern.size(); i++)
                        maxProposedPattern[i] = false;
                    AddSlot= 0;

                }

                int AddSlot;
                int area;
                wns::simulator::Time iFragmentDuration;
                int AdditionalPackets;

            };

            //holds the proposed pattern
            MaxPosPattern maxPosPattern;
            MaxEfficiency maxEfficiency;

    };
}//drp
}//wimemac
#endif
