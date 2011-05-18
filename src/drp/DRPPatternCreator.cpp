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

#include <WIMEMAC/drp/DRPPatternCreator.hpp>
#include <WIMEMAC/lowerMAC/Manager.hpp>
#include <math.h>

using namespace wimemac::drp;

DRPPatternCreator::DRPPatternCreator(wns::logger::Logger logger_, double patternPEROffset_, wns::service::dll::UnicastAddress TargetAddress_):
    SlotpSF(256),
    GlobalDRPMap(256, false),
    SlotDuration(256E-6),
    GuardDuration(12E-6),
    SIFSduration(10E-6),
    PreambPlusHeadDuration(11.25E-6),
    ACKduration(13.125E-6),
    patternPEROffset(patternPEROffset_),
    TargetAddress(TargetAddress_),
    logger(logger_)
{
}

void 
DRPPatternCreator::SetReservationGap(int SlotAmount)
{
    ReservationGap = SlotAmount;
}

void
DRPPatternCreator::SetNumberOfBPSlots(int numberOfBPSlots_)
{
    NumberOfBPSlots = numberOfBPSlots_;
}

void
DRPPatternCreator::SetPhyMode(wimemac::convergence::PhyMode phyMode_)
{
    phyMode = phyMode_;
}

int
DRPPatternCreator::CalcMissingPackets(Vector& allocMap_)
{
    int i = 0;
    int last = allocMap_.size();
    int tmpAdj = 0;
    int PosPacketpSF = 0;

    while(i < last)
    {
        assure(i<allocMap_.size(), "Vector boundary exeeded; loop error!");
        while(i < last && allocMap_[i] == false)
            i++;

        if (i >= last) break;
        assure(i<allocMap_.size(), "Vector boundary exeeded; loop error!");
        while(i < last && allocMap_[i] == true)
        {
            ++tmpAdj;
            ++i;
        }
        if(tmpAdj > 0)
        {
            wns::simulator::Time TXOP = 0;
            TXOP = tmpAdj * SlotDuration - GuardDuration;

            double FTDuration_ = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)MaxPacketSize, phyMode) + 2*SIFSduration + ACKduration;

            PosPacketpSF += floor(TXOP/FTDuration_);
            tmpAdj = 0;
        }

    }

    int missingPackets = (PosPacketpSF < PacketpSF) ? PacketpSF - PosPacketpSF : 0;
    return missingPackets;
}

wimemac::convergence::PhyMode
DRPPatternCreator::getPhyMode()
{
    return phyMode;
}

void
DRPPatternCreator::SetTrafficChar(int PacketpFrame, int BitpFrame, int MaxCompoundSize, int reservationBlocks)
{
    MaxPacketSize = MaxCompoundSize;
    PacketpSF = ceil( PacketpFrame * (1 + patternPEROffset) ); // Add traffic bandwidth according to max allowed PER
    DataDuration = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)MaxPacketSize, phyMode);
    FTDuration = DataDuration + 2*SIFSduration + ACKduration;
    DivideSFintoAreas = reservationBlocks;
    ReservationGap = (SlotpSF - NumberOfBPSlots) / DivideSFintoAreas;
    mAdjSlot = ceil( ((double) FTDuration + GuardDuration )/ (double) SlotDuration);
    PacketpArea = ceil( (double) PacketpSF / (double) DivideSFintoAreas);
    AdjSlot = ceil(  (((double)PacketpArea * (double)FTDuration) + GuardDuration) / (double)SlotDuration);

    if (AdjSlot > ReservationGap)
    {
        MESSAGE_SINGLE(NORMAL, logger, "SetTrafficChar : Too much traffic! Using phymode " << phyMode.getDataRate() << " the transmission would need " << AdjSlot * SlotpSF / ReservationGap << " MASs. Trying to use the maximum of " << SlotpSF << " - " <<  NumberOfBPSlots << " Beacon slots");
        AdjSlot = SlotpSF - NumberOfBPSlots;
        ReservationGap = SlotpSF - NumberOfBPSlots;
        PacketpArea = PacketpSF;
    }

    MESSAGE_SINGLE(NORMAL, logger, "SetTrafficChar : ReservationGap: " << ReservationGap << " | PacketpArea: " << PacketpArea << " | AdjSlot: " << AdjSlot << " | FTDuration is set to " << FTDuration);

}

void
DRPPatternCreator::UpdateTrafficChar(int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    MaxPacketSize = MaxCompoundSize;
    PacketpSF = ceil( CompoundspSF * (1 + patternPEROffset) ); // Add traffic bandwidth according to max allowed PER
    PacketpArea = ceil( (double) PacketpSF / (double) DivideSFintoAreas);
    
    UpdateTrafficChar();
}

void
DRPPatternCreator::UpdateTrafficChar()
{
    // Update TrafficChar according to changes of phymode
    DataDuration = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)MaxPacketSize, phyMode);
    FTDuration = DataDuration + 2*SIFSduration + ACKduration;
    mAdjSlot = ceil( ((double) FTDuration + GuardDuration )/ (double) SlotDuration);
    AdjSlot = ceil( (((double)PacketpArea * (double)FTDuration) + GuardDuration )/ (double)SlotDuration);

    if (AdjSlot > ReservationGap)
    {
        MESSAGE_SINGLE(NORMAL, logger, "UpdateTrafficChar : Too much traffic! Using phymode " << phyMode.getDataRate() << " the transmission would need " << AdjSlot * SlotpSF / ReservationGap << " MASs. Trying to use the maximum of " << SlotpSF << " - " <<  NumberOfBPSlots << " Beacon slots");
        AdjSlot = SlotpSF - NumberOfBPSlots;
        ReservationGap = SlotpSF - NumberOfBPSlots;
        PacketpArea = PacketpSF;
    }

    MESSAGE_SINGLE(NORMAL, logger, "UpdateTrafficChar : ReservationGap: " << ReservationGap << " | PacketpArea: " << PacketpArea << " | AdjSlot: " << AdjSlot << " | FTDuration is set to " << FTDuration);
}


bool DRPPatternCreator::GetPattern(Vector& ProposedPattern, Vector& ConstraintDRPMap, bool useInperfectPatterns)
{

    if(friends.manager->getRandomUse())
    {
        Vector tmpDRPAllocMap(256, false);
        return CreateRandomPattern(ProposedPattern, tmpDRPAllocMap, ConstraintDRPMap, useInperfectPatterns);
    }
    else
    {

        copy(ConstraintDRPMap.begin(), ConstraintDRPMap.end(), GlobalDRPMap.begin());

        Vector tmpProposedPattern(256,false);
        InitVector(tmpProposedPattern);
        int PosArea;
        CalcMaxPropMap(tmpProposedPattern);

        for(int i = 0; i < 256; i++)
        {
            assure(i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
            if(maxPosPattern.maxProposedPattern[i] == true)
                    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Maximum proper pattern occupies slot nr: " << i );
        }

        if(maxPosPattern.conflict != 0)
        {
            ClearConflictArea();

            for(int i = 0; i < 256; i++)
            {
                assure(i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                if(maxPosPattern.maxProposedPattern[i] == true)
                    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: After conflicts are removed pattern occupies slot nr: " << i );
            }

            int first = NumberOfBPSlots;
            int last = NumberOfBPSlots + ReservationGap;
            int nr = 1;
            while(last <= maxPosPattern.maxProposedPattern.size())
            {

                if(FindEmptyArea(maxPosPattern.maxProposedPattern, first, last))
                {
                    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: this area is empty " << nr << " of " << DivideSFintoAreas);
                    if(!AllocAreaOutside(first,last, AdjSlot))
                    {
                        int block = ceil(double(AdjSlot)/double(mAdjSlot));

                        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Blocks of size " << mAdjSlot << " needed : " << block);
                        while(block != 0)
                        {
                            if(AllocAreaOutside(first,last, mAdjSlot))
                                block--;
                            else break;
                        }
                    }

                    for(int i = first; i < last; i++)
                    {
                        assure(i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                        if(maxPosPattern.maxProposedPattern[i] == true)
                            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: SplitArea: " << i );
                    }

                }
                else
                {
                    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: this area is not empty " << nr << " of " << DivideSFintoAreas);
                }
                first = last;
                last += ReservationGap;
                if(last >= maxPosPattern.maxProposedPattern.size())
                    last =  maxPosPattern.maxProposedPattern.size() - 1;
                if(last <= first)
                    break;
                nr++;
            }

        }

        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: With this pattern  " << CalcPacketpArea(0, 256)  <<" compounds could be transported ");

        bool returnValue;
        // Evaluation of return value using only one single stream
        if (EvaluateReservation())
        {
            for(int i = 0; i< 256;i++)
            {
                assure(i<ProposedPattern.size() && i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                ProposedPattern[i] = maxPosPattern.maxProposedPattern[i];
            }
            returnValue = true;
        }
        else
        {
            if(useInperfectPatterns)
            {
                for(int i = 0; i< 256;i++)
                {
                    assure(i<ProposedPattern.size() && i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                    ProposedPattern[i] = maxPosPattern.maxProposedPattern[i];
                }
            }

            returnValue = false;
        }
        for(int i = 0; i < ProposedPattern.size(); i++)
        {
            if(ProposedPattern[i] == true)
                MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Pattern after evaluation slot nr: " << i );
        }
        return returnValue;
    }
}

bool
DRPPatternCreator::AddPattern(Vector& ProposedPattern, Vector& DRPAllocMap, Vector& ConstraintDRPMap, bool useInperfectPatterns)
{

    if(friends.manager->getRandomUse())
    {
        return CreateRandomPattern(ProposedPattern, DRPAllocMap, ConstraintDRPMap, useInperfectPatterns);
    }
    else
    {
    // Initialise pattern with already reserved slots of this station
    Vector tmpProposedPattern = DRPAllocMap;
    copy(ConstraintDRPMap.begin(), ConstraintDRPMap.end(), GlobalDRPMap.begin());

    // Since this pattern is already used there are no conflicts
    maxPosPattern.conflict = 0;
    maxPosPattern.SetVector(tmpProposedPattern);

    MESSAGE_SINGLE(NORMAL, logger, "AddPattern: With this pattern  " << CalcPacketpArea(0, 256)  <<" compounds could be transported ");

    // Evaluate complete reservation
    bool returnValue;
    if (EvaluateReservation())
    {
        // Reservation evaluation returned perfect -> use proposed pattern
        for(int i = 0; i < 256; i++)
        {
            assure(i<maxPosPattern.maxProposedPattern.size() && i< DRPAllocMap.size() && i<ProposedPattern.size(), "Vector boundary exeeded; loop error!");
            if (maxPosPattern.maxProposedPattern[i] == true && DRPAllocMap[i] == false)
                ProposedPattern[i] = true;
            else ProposedPattern[i] = false;
        }

        returnValue = true;
    }
    else
    {
        if(useInperfectPatterns)
        {
            // Reservation evaluation returned NOT perfect, but inperfect patterns should also be used
            for(int i = 0; i < 256; i++)
            {
                assure(i<maxPosPattern.maxProposedPattern.size() && i< DRPAllocMap.size() && i<ProposedPattern.size(), "Vector boundary exeeded; loop error!");
                if (maxPosPattern.maxProposedPattern[i] == true && DRPAllocMap[i] == false)
                    ProposedPattern[i] = true;
                else ProposedPattern[i] = false;
            }
        }

        returnValue = false;
    }

    MESSAGE_SINGLE(NORMAL, logger, "AddPattern: Additional pattern is determined:");
    for(int i = 0; i < ProposedPattern.size(); i++)
    {
        if(ProposedPattern[i] == true)
            MESSAGE_SINGLE(NORMAL, logger, "AddPattern: Additional pattern slot nr " << i );
    }

    return returnValue;
    }
}

bool
DRPPatternCreator::CreateRandomPattern(Vector& ProposedPattern, Vector& DRPAllocMap, Vector& ConstraintDRPMap, bool useInperfectPatterns)
{
    copy(ConstraintDRPMap.begin(), ConstraintDRPMap.end(), GlobalDRPMap.begin());
    Vector tmpProposedPattern(256,false);

    // Prevent allocated slots from being used that may not yet be in the constraintdrpmap
    for(int i = 0; i < 256; i++)
    {
        if(DRPAllocMap[i] == true) GlobalDRPMap[i] = true;
    }
    // Fill List with all free areas of size mAdjSlots
    std::vector<int> emptyAreas_;
    int i = 0;
    while (i < GlobalDRPMap.size())
    {
        while (GlobalDRPMap[i] == true && i < GlobalDRPMap.size()) i++;
        int tmpAdjSlots = 0;
        int firstSlot_ = i;
        while (GlobalDRPMap[i] == false && i < GlobalDRPMap.size())
        {
            i++;
            tmpAdjSlots++;
            if (tmpAdjSlots == 2) break;
        }
        if (tmpAdjSlots >= 2)
        {
            emptyAreas_.push_back(firstSlot_);
        }
    }
    int MissingReservation = CalcMissingPackets(DRPAllocMap);

    while (MissingReservation > 0 && emptyAreas_.size() > 0 )
    {
        int areaNumber_ = rand() % emptyAreas_.size();
        int i = emptyAreas_[areaNumber_];
        int GapSize_ = 0;
        while(GlobalDRPMap[i] == false && tmpProposedPattern[i] == false)
        {
            GapSize_++;
            i++;
            if(i >= tmpProposedPattern.size()) //if the end of the map is reached
                break;
            if(GapSize_ >= 2)   //if the number of MASs fits mAdjSlots
                break;
        }
        if(GapSize_ >= 2)
        {
            MESSAGE_SINGLE(NORMAL, logger, "CreateRandomPattern : Reserved slots " << i - GapSize_ << " to slot " << i-1);
            // Fill pattern with new allocated slots
            for(int j=i-GapSize_; j<i; j++)
            {
                tmpProposedPattern[j] = true;
            }
            // Delete allocated area from list
            emptyAreas_.erase(emptyAreas_.begin() + areaNumber_);
            // Missing now
            int packetsInThisGap = floor((GapSize_ * SlotDuration - GuardDuration) / FTDuration);
            MissingReservation -= packetsInThisGap;
        }
    }


    if (emptyAreas_.size() == 0) MESSAGE_SINGLE(NORMAL, logger, "CreateRandomPattern : No more Areas with mAdjSlot size available");

    if((MissingReservation <= 0) || useInperfectPatterns)
    {
        for (int i = 0; i < tmpProposedPattern.size(); i++)
        {
            if(tmpProposedPattern[i] == true) ProposedPattern[i] = true;
        }
    }

    if(MissingReservation > 0) return false;
    else return true;
}

int
DRPPatternCreator::GapFilling(int MissingReservation)
{
    MESSAGE_SINGLE(NORMAL, logger, "Start GapFilling");
    for(int i=0; i<maxPosPattern.maxProposedPattern.size(); i++)
    {
        if(MissingReservation > 0)
        {
            if(GlobalDRPMap[i] == false && maxPosPattern.maxProposedPattern[i] == false)
            {
                int GapSize = 0;
                int packetsInThisGap = 0;
                while(GlobalDRPMap[i] == false && maxPosPattern.maxProposedPattern[i] == false)
                {
                    GapSize++;
                    i++;
                    if(i >= maxPosPattern.maxProposedPattern.size()) //if the end of the map is reached
                        break;
                    packetsInThisGap = floor((GapSize * SlotDuration - GuardDuration) / FTDuration);
                    if(packetsInThisGap >= MissingReservation)   //if the number of neccessary packets is reached
                        break;
                }
                if(GapSize >= mAdjSlot)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "GapFilling : Found a gap.");
                    if(GapSize == 1)
                    {
                        MESSAGE_SINGLE(NORMAL, logger, "GapFilling : New reserved slot: " << i - GapSize);
                    }
                    else
                        MESSAGE_SINGLE(NORMAL, logger, "GapFilling : Reserved slot " << i - GapSize << " to slot " << i-1);

                    for(int j=i-GapSize; j<i; j++)
                    {
                        maxPosPattern.maxProposedPattern[j] = true;
                    }
                    MissingReservation -= packetsInThisGap;
                }
            }
        }
    }
    return MissingReservation;
}

void
DRPPatternCreator::InitVector(Vector& InitMap)
{
    int tmpAdjSlot = AdjSlot;
    int SlotNumber = NumberOfBPSlots;
    int it = NumberOfBPSlots;

    if (AdjSlot == 0) return;

    MESSAGE_SINGLE(NORMAL, logger, "InitVector :  AdjSlot : " << AdjSlot << " |  Size : " << InitMap.size());
    while(it <= InitMap.size() - AdjSlot)
    {

        while((SlotNumber - NumberOfBPSlots) % ReservationGap != 0 && it <= InitMap.size())
        {
            ++it;
            ++SlotNumber;
        }

        tmpAdjSlot = AdjSlot;
        if(it <= InitMap.size() - AdjSlot)
        {
            while(tmpAdjSlot > 0)
            {
                InitMap[it] = true;
                --tmpAdjSlot;
                it++;
                SlotNumber++;
            }

        }

    }

}

bool
DRPPatternCreator::IsPatternValid(Vector& ProposedMap) const
{
    for(int i = 0; i < ProposedMap.size(); i++)
    {
        assure(i<GlobalDRPMap.size(), "Vector boundary exeeded; loop error!");
        if(ProposedMap[i] && GlobalDRPMap[i] == true)
            return false;
    }
    return true;
}

int
DRPPatternCreator::CalcNumArea(Vector& ProposedMap)
{
    int Conflict = 0;
    for(int i = 0; i < 256; i++)
    {
        while(ProposedMap[i] == false && i < ProposedMap.size())
            i++;
        if(ProposedMap[i] && GlobalDRPMap[i] == true && i < GlobalDRPMap.size())
        {
            Conflict++;
            while(ProposedMap[i] == true && i < ProposedMap.size())
                i++;
        }
    }
    return Conflict;
}

void
DRPPatternCreator::CalcMaxPropMap(Vector& tmpProposedPattern)
{
    int conflict;
    conflict = CalcNumArea(tmpProposedPattern);
    maxPosPattern.conflict = conflict;
    maxPosPattern.SetVector(tmpProposedPattern);

    int RotationShifts = 0;
    while(!IsPatternValid(tmpProposedPattern) && RotationShifts < ReservationGap)
    {
        conflict = CalcNumArea(tmpProposedPattern);
        if(conflict < maxPosPattern.conflict)
        {
            maxPosPattern.conflict = conflict;
            maxPosPattern.SetVector(tmpProposedPattern);
        }
        rotate(tmpProposedPattern.begin(), tmpProposedPattern.end()-1, tmpProposedPattern.end());
        RotationShifts++;
    }
    conflict = CalcNumArea(tmpProposedPattern);
    if(conflict == 0)
        maxPosPattern.SetVector(tmpProposedPattern);
    maxPosPattern.conflict = conflict;

}

bool
DRPPatternCreator::PatternHasFreeSlots(Vector pattern)
{
    bool hasFreeSlots = false;

    for(int i = NumberOfBPSlots; i < pattern.size(); i++)
    {
        if(pattern[i] == false)
        {
            hasFreeSlots = true;
            break;
        }
    }
    return hasFreeSlots;
}

bool
DRPPatternCreator::FindEmptyArea(Vector& tmpProposedPattern, int first, int last)
{
    int i = first;
    int tmpAdj = AdjSlot;
    while(i < tmpProposedPattern.size() && i < last)
    {
        if(tmpProposedPattern[i] == true)
            tmpAdj--;

        i++;
    }

    if(tmpAdj == 0)
        return false;

    return true;
}

bool
DRPPatternCreator::ClearConflictArea()
{
    int i = 0;
    while(i < maxPosPattern.maxProposedPattern.size())
    {
        assure(i<GlobalDRPMap.size(), "Vector boundary exeeded; loop error!");
        if(maxPosPattern.maxProposedPattern[i] == true && GlobalDRPMap[i] == true)
        {
            int FirstConflict = i;
            while(i < maxPosPattern.maxProposedPattern.size() && maxPosPattern.maxProposedPattern[i] == true)
            {
                maxPosPattern.maxProposedPattern[i] = false;
                i++;
            }
            if(FirstConflict > 0)
            {
                i = FirstConflict - 1;
                while(i >= 0 && maxPosPattern.maxProposedPattern[i] == true)
                {
                    assure(i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                    maxPosPattern.maxProposedPattern[i] = false;
                    i--;
                }
            }

        }
        i++;
    }
}


bool
DRPPatternCreator::AllocAreaOutside(int first, int last, int CalcAdjSlot)
{
    assure(first<maxPosPattern.maxProposedPattern.size() && last<=maxPosPattern.maxProposedPattern.size() && CalcAdjSlot > 0, "AllocAreaOutside: invalid input parameters.");
    if(IsSpaceLeft(first, last, CalcAdjSlot))
    {
        int i = first;
        int firstfree;
        int tmpAdj = CalcAdjSlot;
        assure(i<GlobalDRPMap.size() && i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
        while(i < last && (GlobalDRPMap[i] == true || maxPosPattern.maxProposedPattern[i] == true))
            i++;
        firstfree = i;
        while(i < last && GlobalDRPMap[i] == false && maxPosPattern.maxProposedPattern[i] == false)
        {
            --tmpAdj;
            ++i;
        }
        if(tmpAdj <= 0)
        {
            tmpAdj = CalcAdjSlot;
            i = firstfree;
            while(tmpAdj > 0)
            {
                assure(i<maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
                maxPosPattern.maxProposedPattern[i] = true;
                ++i;
                --tmpAdj ;
            }
        }
        else
        {
            if(i < maxPosPattern.maxProposedPattern.size())
                return(AllocAreaOutside(i, last,CalcAdjSlot));
            else
                return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}


bool
DRPPatternCreator::IsSpaceLeft(int first, int last, int CalcAdjSlot) const
{
    int i = first;
    int space = 0;

    assure(last<=GlobalDRPMap.size(), "Vector boundary exeeded; loop error!");
        while(i < last && (GlobalDRPMap[i] == true || maxPosPattern.maxProposedPattern[i] == true))
            i++;

        assure(i <= last && last <= GlobalDRPMap.size(), "Vector boundary exeeded; loop error!");
        while(i < last && (GlobalDRPMap[i] == false && maxPosPattern.maxProposedPattern[i] == false))
        {
            ++space;
            ++i;
        }
        if(space >= CalcAdjSlot)
        {
            return true;
        }

        if(i == last)
            return false;
        return(IsSpaceLeft(i, last,CalcAdjSlot));

}



bool
DRPPatternCreator::EvaluateReservation()
{
    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: evaluate reservation ");

    int ExpectedPackets = PacketpSF;
    int first = 0;
    int last = maxPosPattern.maxProposedPattern.size();
    int MissingReservation = (CalcPacketpArea(first,last) < ExpectedPackets) ? ExpectedPackets - CalcPacketpArea(first,last) : 0;
    if(MissingReservation != 0)
    {
        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: evaluate the reservation: not perfect -> search for more slots");

        // Count number of reserved MASs
        int numberOfReservedSlots_ = 0;
        for(int i = 0; i< maxPosPattern.maxProposedPattern.size();i++)
            if(maxPosPattern.maxProposedPattern[i] == true) numberOfReservedSlots_++;

        if (numberOfReservedSlots_ == SlotpSF - NumberOfBPSlots)
        {
            // If the whole SF is reserved, don't continue since there are no more slots
            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: All MASs slots are used. The missing " << MissingReservation << " packets definitely cannot be send");
            return true;
        }

        BestEfficency(MissingReservation);

    }
    first = 0;
    last = maxPosPattern.maxProposedPattern.size();
    MissingReservation = (CalcPacketpArea(first,last) < ExpectedPackets) ? ExpectedPackets - CalcPacketpArea(first,last) : 0;

    if(MissingReservation > 0)
    {
        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: evaluate the reservation: not perfect -> Gapfilling");
        MissingReservation = GapFilling(MissingReservation);
    }
    if(MissingReservation > 0)
    {
        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: evaluate the reservation: not perfect -> returning anyway");
        return false;
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: evaluate the reservation: perfect");
        return true;
    }
}


void
DRPPatternCreator::BestEfficency(int MissingReservation)
{
    int i = 0;
    int tmpAdj = 0;
    int packet = 0;
    int first;
    int NextFirst = 0;
    int last;
    int FreeBefore = 0;
    int FreeAfter = 0;
    Vector CopyProposedPattern(256,false);
    int StoreMissingReservation = 0;
    int area = 0;


    while(MissingReservation >= 0)
    {
        i = 0;
        tmpAdj = 0;
        packet = 0;
        first = 0;
        NextFirst = 0;
        last = 0;
        FreeBefore = 0;
        FreeAfter = 0;

        copy(maxPosPattern.maxProposedPattern.begin(), maxPosPattern.maxProposedPattern.end(), CopyProposedPattern.begin());

        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: best efficiency, missing Reservations: " << MissingReservation);
        if(StoreMissingReservation == MissingReservation)
        {
            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: best efficiency, there were no new slots found in the last search. Exiting search");
            break;
        }

        StoreMissingReservation = MissingReservation;


        while(i < SlotpSF)
        {
            while(i < SlotpSF && maxPosPattern.maxProposedPattern[i] == false)
                i++;

            first = i;
            //data-slots are summarized to areas. Each area has [ReservationGap] slots
            area = floor((double)(i - NumberOfBPSlots)/ double (ReservationGap));

            while(i < SlotpSF && maxPosPattern.maxProposedPattern[i] == true)
                i++;

            last = i;
            //first: beginning of a reservation block; last: slot after the end of a reservation block
            NextFirst = i;

            while(NextFirst < SlotpSF && maxPosPattern.maxProposedPattern[NextFirst] == false)
                NextFirst++;

            //NextFirst: Beginning of the next reservation block. i will be set to NextFirst at the end of this cycle

            packet = CalcPacketpArea(first,last);
            wns::simulator::Time TrainDuration = packet * FTDuration;
            wns::simulator::Time iFragmentDuration = (last - first)*SlotDuration - GuardDuration - TrainDuration;
            if (iFragmentDuration < 0) iFragmentDuration = 0;
            double AddSlot = ceil ((FTDuration - iFragmentDuration) / SlotDuration);

            int tmpFirst = (first - AddSlot >= 0) ? first - AddSlot : 0;
            int tmpLast = (last + AddSlot) <= SlotpSF ? last + AddSlot : SlotpSF ;

            //tmpFirst and tmpLast: Appends [AddSlot] Slots to the examined reservation block

            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: best efficiency, free space between: " << tmpFirst <<" " << tmpLast);
            //IsSpaceLeft checks, if there are [AddSlot] free connected slots between tmpFirst and tmpLast
            if(!IsSpaceLeft(tmpFirst, tmpLast, AddSlot))
            {
                tmpFirst = first > 0 ? first - 1 : 0;
                tmpLast = first;
                int neighbour = 1;
                while(tmpFirst >= 0 && neighbour <= AddSlot && IsSpaceLeft(tmpFirst, tmpLast, neighbour) )
                {
                    tmpFirst--;
                    tmpAdj++;
                    ++neighbour;
                }
                FreeBefore = tmpAdj;
                tmpAdj = 0;
                tmpFirst = last;
                tmpLast = last < 256 ? last + 1 : 256;
                neighbour = 1;
                while(tmpLast < SlotpSF && neighbour <= AddSlot && IsSpaceLeft(tmpFirst, tmpLast, neighbour) )
                {
                    tmpLast++;
                    tmpAdj++;
                    ++neighbour;
                }
                FreeAfter = tmpAdj;
                tmpAdj = 0;

                if(FreeAfter + FreeBefore >= AddSlot)
                {
                    if(FreeAfter == AddSlot)
                        FreeBefore = 0;
                    if(FreeBefore == AddSlot)
                        FreeAfter = 0;
                    packet = floor( ( (last+AddSlot - first)*SlotDuration - GuardDuration )/ FTDuration);
                    TrainDuration = packet * FTDuration;
                    iFragmentDuration = (last + AddSlot - first)*SlotDuration - GuardDuration - TrainDuration;

                    if(maxEfficiency.iFragmentDuration > iFragmentDuration || maxEfficiency.AddSlot == 0)
                    {
                        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: I'm splitting fragmentation before : " << maxEfficiency.iFragmentDuration << " and now : " << iFragmentDuration << " in area " << area);

                        maxEfficiency.AddSlot = AddSlot;
                        maxEfficiency.area = area;

                        tmpLast = first;
                        tmpFirst = first - FreeBefore;
                        if(!AllocAreaOutside(tmpFirst, tmpLast, FreeBefore))
                            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Reservation error");

                        packet = floor(((last+AddSlot - first)*SlotDuration  - GuardDuration )/ FTDuration);
                        TrainDuration = packet * FTDuration;
                        iFragmentDuration = (last + AddSlot - first)*SlotDuration - GuardDuration - TrainDuration;

                        maxEfficiency.iFragmentDuration = iFragmentDuration;
                        maxEfficiency.AdditionalPackets = packet - floor( ((last - first)*SlotDuration - GuardDuration) / FTDuration);
                        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: set fragmentation: "<< iFragmentDuration);

                        tmpFirst = last;
                        tmpLast = last + FreeAfter;
                        if(!AllocAreaOutside(tmpFirst, tmpLast, FreeAfter))
                            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Reservation error");

                        copy(maxPosPattern.maxProposedPattern.begin(), maxPosPattern.maxProposedPattern.end(),
                            maxEfficiency.maxProposedPattern.begin());
                        copy(CopyProposedPattern.begin(), CopyProposedPattern.end(),
                            maxPosPattern.maxProposedPattern.begin());

                    }
                }
            }
            else
            {

                packet = floor(((last+AddSlot - first)*SlotDuration  - GuardDuration )/ FTDuration);
                TrainDuration = packet * FTDuration;
                iFragmentDuration = (last + AddSlot - first)*SlotDuration - GuardDuration - TrainDuration;
                MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Calculate fragment, without split; addslot : " << AddSlot << " packets now: "<< packet 
                << " train duration: " << TrainDuration << " Fragment duration: "
                << iFragmentDuration);

                if(maxEfficiency.iFragmentDuration > iFragmentDuration || maxEfficiency.AddSlot == 0)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Fragmentation before : " << maxEfficiency.iFragmentDuration << " and now : " << iFragmentDuration);
                    maxEfficiency.AddSlot = AddSlot;

                    if(!AllocAreaOutside(tmpFirst, tmpLast, AddSlot))
                        MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Reservation error");
                    if(last + AddSlot > maxPosPattern.maxProposedPattern.size())
                        packet = CalcPacketpArea(first - AddSlot,last);
                    else
                        packet = CalcPacketpArea(first - AddSlot,last + AddSlot); // Slots could have been added before or after the existing slots
                    TrainDuration = packet * FTDuration;
                    iFragmentDuration = (last + AddSlot - first)*SlotDuration - GuardDuration - TrainDuration;
                    maxEfficiency.AdditionalPackets = packet - CalcPacketpArea(first, last);
                    maxEfficiency.iFragmentDuration = iFragmentDuration;
                    maxEfficiency.area = area;
                    copy(maxPosPattern.maxProposedPattern.begin(), maxPosPattern.maxProposedPattern.end(),
                        maxEfficiency.maxProposedPattern.begin());
                    copy(CopyProposedPattern.begin(), CopyProposedPattern.end(),
                        maxPosPattern.maxProposedPattern.begin());
                }
            }

            i = NextFirst;
            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: best efficiency, next is: " << NextFirst);
        }
        if(maxEfficiency.AddSlot > 0)
        {
            copy(maxEfficiency.maxProposedPattern.begin(), maxEfficiency.maxProposedPattern.end(),
                maxPosPattern.maxProposedPattern.begin());

            maxEfficiency.Clear();
            MissingReservation -= maxEfficiency.AdditionalPackets;
            MESSAGE_SINGLE(NORMAL, logger, "InitPattern: Missing: " << MissingReservation);
            if (MissingReservation <= 0) 
            {
                MissingReservation = 0;
                break;
            }
            
            
        }
    }


}

int
DRPPatternCreator::CalcPacketpArea(int first, int last)
{
    int i = first;
    int tmpAdj = 0;
    int PacketpArea = 0;

    assure(last<=maxPosPattern.maxProposedPattern.size(), "Vector boundary exeeded; loop error!");
    while(i < last)
    {
        while(i < last && maxPosPattern.maxProposedPattern[i] == false)
        {
            i++;
        }

        if (i >= last) break;
        while(i < last && maxPosPattern.maxProposedPattern[i] == true)
        {
            ++tmpAdj;
            ++i;
        }
        if(tmpAdj > 0)
        {
            wns::simulator::Time TXOP = 0;
            TXOP = tmpAdj * SlotDuration - GuardDuration;

            double FTDuration_ = friends.manager->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)MaxPacketSize, phyMode) + 2*SIFSduration + ACKduration;

            PacketpArea += floor(TXOP/FTDuration_);
            tmpAdj = 0;
        }

    }
    return PacketpArea;

}
