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

#include <WIMEMAC/drp/DRPManager.hpp>

#include <WIMEMAC/lowerMAC/Manager.hpp>

using namespace wimemac::drp;
using wimemac::management::BeaconCommand;


DRPManager::DRPManager(wns::service::dll::UnicastAddress TargetAddress,BeaconCommand::ReservationType InitialType,
            BeaconCommand::ReasonCode InitialReason, BeaconCommand::DeviceType InitialDevice, Vector DRPSlotMap,
            helper::IDRPQueueInterface* _QueueInterface, wns::logger::Logger logger_, wimemac::lowerMAC::Manager* manager_, int numberOfBPSlots_, bool useRateAdaptation_, double patternPEROffset_, wimemac::convergence::PhyMode DefPhyMode):

            DRPPatternCreator(logger_, patternPEROffset_, TargetAddress),

            reasoncode(InitialReason),
            reservationtype(InitialType),
            devicetype(InitialDevice),
            peerAddress(TargetAddress),
            DRPAllocMap(DRPSlotMap),
            QueueInterface(_QueueInterface),
            connectionstatus(false),
            mergeconnectionstatus(false),
            waitSFsForNewAvailIE(-1),
            hasPendingDRPMerge(false),
            needsAdditionalPattern(false),
            useRateAdaptation(useRateAdaptation_),
            logger(logger_)
{
    scheduler = wns::simulator::getEventScheduler();
    DRPPatternCreator::friends.manager = manager_;
    DRPPatternCreator::SetNumberOfBPSlots(numberOfBPSlots_);
    DRPPatternCreator::SetPhyMode(DefPhyMode);

    tmpDRPMergeMap = Vector(DRPSlotMap.size(), false);
}

DRPManager::DRPManager(wns::service::dll::UnicastAddress TargetAddress,BeaconCommand::ReservationType InitialType,
            BeaconCommand::ReasonCode InitialReason, BeaconCommand::DeviceType InitialDevice, Vector DRPSlotMap,
            helper::IDRPQueueInterface* _QueueInterface, wns::logger::Logger logger_, wimemac::lowerMAC::Manager* manager_, int numberOfBPSlots_, bool useRateAdaptation_, double patternPEROffset_):

            DRPPatternCreator(logger_, patternPEROffset_, TargetAddress),

            reasoncode(InitialReason),
            reservationtype(InitialType),
            devicetype(InitialDevice),
            peerAddress(TargetAddress),
            DRPAllocMap(DRPSlotMap),
            QueueInterface(_QueueInterface),
            connectionstatus(false),
            mergeconnectionstatus(false),
            waitSFsForNewAvailIE(-1),
            hasPendingDRPMerge(false),
            needsAdditionalPattern(false),
            useRateAdaptation(useRateAdaptation_),
            logger(logger_)
{
    scheduler = wns::simulator::getEventScheduler();
    DRPPatternCreator::SetNumberOfBPSlots(numberOfBPSlots_);
    DRPPatternCreator::friends.manager = manager_;

    tmpDRPMergeMap = Vector(DRPSlotMap.size(), false);
    
}

wns::service::dll::UnicastAddress DRPManager::GetAddress()
{
    return peerAddress;
}

void
DRPManager::SetReservationType(BeaconCommand::ReservationType DRPResType)
{
    reservationtype = DRPResType;
}

BeaconCommand::ReservationType
DRPManager::GetReservationType()
{
    return reservationtype;
}

void 
DRPManager::SetReasonCode(BeaconCommand::ReasonCode reason)
{
    reasoncode = reason;

}

BeaconCommand::ReasonCode
DRPManager::GetReasonCode()
{
    return reasoncode;
}

void 
DRPManager::SetMergeReasonCode(BeaconCommand::ReasonCode reason)
{
    mergereasoncode = reason;

}

BeaconCommand::ReasonCode
DRPManager::GetMergeReasonCode()
{
    return mergereasoncode;
}

bool
DRPManager::areMASsAvailable(Vector DRPGlobal)
{
    for(int i = 0; i<DRPGlobal.size(); i++)
    {
        // MASs are available if there is space in the global MAP
        if(DRPGlobal[i] == true)
            return true;
    }
    return false;
  
}

void
DRPManager::FindNewPattern(Vector DRPGlobal)
{
    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "Alloc";
    MESSAGE_END();

    bool peerStationsIsFullyAvailable = true;
    
    Vector::iterator itg;
    Vector::const_iterator itp;
    
    
    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "DRPManager searches for a new reservation pattern ";MESSAGE_END();

    // Update DRPGlobal with DRP Availability from peer
    if (friends.manager->getDRPScheduler()->UpdateMapWithPeerAvailabilityMap( peerAddress, DRPGlobal))
    {
        MESSAGE_SINGLE(NORMAL, logger, "FindPattern: Updated DRPGlobalMap with availabilityMap from address " << peerAddress << " to support ongoing pattern creation");
        peerStationsIsFullyAvailable = false;
    }
    //else MESSAGE_SINGLE(NORMAL, logger, "FindPattern: There is no availabilityMap to update with or none of the available slots are unusable by address " << peerAddress);

    if(DRPPatternCreator::GetPattern(DRPAllocMap,DRPGlobal, true))
    {
        MESSAGE_SINGLE(NORMAL, logger, "FindPattern: A perfect pattern was found!");
    }
    else
        MESSAGE_SINGLE(NORMAL, logger, "FindPattern: There was no perfect pattern found! Proceeding with imperfect pattern");

}

void
DRPManager::CreateAdditionalPattern(Vector DRPGlobal)
{
    // Initialise this new pattern with reason code accept
    SetMergeReasonCode(BeaconCommand::Accept);
    bool peerStationsIsFullyAvailable = true;
    
    Vector::iterator itg;
    Vector::const_iterator itp;
      
    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << "DRPManager searches for an additional reservation pattern ";MESSAGE_END();
    

    // Update DRPGlobal with DRP Availability from peer
    if (friends.manager->getDRPScheduler()->UpdateMapWithPeerAvailabilityMap( peerAddress, DRPGlobal))
    {
        MESSAGE_SINGLE(NORMAL, logger, "AddPattern: Updated DRPGlobalMap with availabilityMap from address " << peerAddress << " to support ongoing pattern creation");
        peerStationsIsFullyAvailable = false;
    }
    else MESSAGE_SINGLE(NORMAL, logger, "AddPattern: There is no availabilityMap to update with or none of the available slots are unusable by address " << peerAddress);

    if(DRPPatternCreator::AddPattern(tmpDRPMergeMap, DRPAllocMap, DRPGlobal, true))
    {
        MESSAGE_SINGLE(NORMAL, logger, "AddPattern: A perfect pattern was found!");
    }
    else
        MESSAGE_SINGLE(NORMAL, logger, "AddPattern: There was no perfect pattern found! Proceeding with inperfect pattern");

    hasPendingDRPMerge = true;
    needsAdditionalPattern = false;

}

void
DRPManager::SetPattern(Vector DRPreservation)
{
    DRPAllocMap = DRPreservation;
}

void
DRPManager::SetMergePattern(Vector DRPreservation)
{
    tmpDRPMergeMap = DRPreservation;
}

Vector
DRPManager::GetPattern()
{
    return DRPAllocMap;
}

Vector
DRPManager::GetMergePattern()
{
    return tmpDRPMergeMap;
}

void
DRPManager::ResolveConflict()
{
    for(int i = 0; i < DRPAllocMap.size(); i++)
    {
        DRPAllocMap[i] = false;
    }

    SetStatus(false);
    SetReasonCode(BeaconCommand::Modified);
}

void
DRPManager::ResolveMergeConflict()
{
    for(int i = 0; i < tmpDRPMergeMap.size(); i++)
    {
        tmpDRPMergeMap[i] = false;
    }
    SetMergeStatus(false);
    SetMergeReasonCode(BeaconCommand::Modified);
}

void
DRPManager::SetDeviceType(BeaconCommand::DeviceType deviceType)
{
    devicetype = deviceType;
}

BeaconCommand::DeviceType
DRPManager::GetDeviceType()
{
    return devicetype;
}

BeaconCommand::DeviceType GetDeviceType();

void
DRPManager::StartRegisterReservation(wns::simulator::Time BPDuration)
{
    tmpDRPAllocMap = DRPAllocMap;
    RegisterReservation(BPDuration);
}

void
DRPManager::SetStatus(bool ConnectionStatus)
{
    connectionstatus = ConnectionStatus;
}


bool
DRPManager::GetStatus()
{
    return connectionstatus;
}

void
DRPManager::SetMergeStatus(bool ConnectionStatus)
{
    mergeconnectionstatus = ConnectionStatus;
}

bool
DRPManager::GetMergeStatus()
{
    return mergeconnectionstatus;
}

bool
DRPManager::IsMapCreated()
{
if(find(DRPAllocMap.begin(), DRPAllocMap.end(), true) == DRPAllocMap.end())
    return false;
else
    return true;
}

bool
DRPManager::NeedsAdditionalPattern()
{

    if ((DRPPatternCreator::CalcMissingPackets(DRPAllocMap) > 0) && !hasPendingDRPMerge)
        return true;

    if (needsAdditionalPattern)
    {   // The status was set to create a new pattern

        if (DRPPatternCreator::CalcMissingPackets(DRPAllocMap) <= 0)
        {
            // Needs additional pattern was probably set because of a MCS change. The Pattern however still suffices
            needsAdditionalPattern = false;
            return false;
        }

        if (hasPendingDRPMerge)
        {
            // There is still an ongoing merge in progress -> wait until its over
            MESSAGE_SINGLE(NORMAL, logger, "There is still a merge in progress, waiting with pattern creation until it's over");
            return false;
        }
        else return true;
    }
    else return false;
}

bool
DRPManager::HasPendingDRPMerge()
{
    if(find(tmpDRPMergeMap.begin(), tmpDRPMergeMap.end(), true) == tmpDRPMergeMap.end())
    {
        // If the station got an update regarding the link quality an additional drp pattern is created.
        // However, if the pattern is still sufficient hasPendingDRPMerge would be true, but there would
        // be no additional Map -> no signalling for a mergemap is required
        hasPendingDRPMerge = false;
        return false;
    }
    else
        return hasPendingDRPMerge;
}

void
DRPManager::AddReservation(wimemac::management::BeaconCommand::ReasonCode mergeReasonCode_, Vector mergeMap_)
{
    hasPendingDRPMerge = true;
    tmpDRPMergeMap = mergeMap_;
    mergereasoncode = mergeReasonCode_;
}

void
DRPManager::MergePatterns()
{
    // Merge original and additional DRP Maps
    Vector::iterator it1 = tmpDRPMergeMap.begin();
    Vector::iterator it2 = DRPAllocMap.begin();

    for(it1, it2;  it1!= tmpDRPMergeMap.end() || it2 != DRPAllocMap.end() ; ++it1, ++it2)
    {
        *it2 = *it1 | *it2;
    }
    MESSAGE_SINGLE(NORMAL, logger, "Merging DRP Maps");

    for(int i = 0; i < tmpDRPMergeMap.size(); i++)
    {
        tmpDRPMergeMap[i] = false;
    }

    mergeconnectionstatus = false;
    hasPendingDRPMerge = false;
}

void
DRPManager::UpdatePhyModeDown(wimemac::convergence::PhyMode phyMode_)
{

    if (useRateAdaptation)
    {
            MESSAGE_SINGLE(NORMAL, logger, "Switching to a lower PhyMode with data rate " << phyMode_.getDataRate() << " Mb/s");
            // Update the phymode and recalculate the traffic characteristic
            DRPPatternCreator::SetPhyMode(phyMode_);
            DRPPatternCreator::UpdateTrafficChar();

            // phymode was changed -> create a new pattern for drp
            needsAdditionalPattern = true;
    }
    else MESSAGE_SINGLE(NORMAL, logger, "Rate Adaptation is switched off!");

}

void
DRPManager::RegisterReservation(wns::simulator::Time BPDuration)
{
    int adjacent = 0;
    double MASduration = 256E-6;
    int i = 0;

    while(i < tmpDRPAllocMap.size())
    {

        if(tmpDRPAllocMap[i] == true)
        {
            if(adjacent == 0)
            {
                ReservationStart = i * MASduration - BPDuration;
                MESSAGE_BEGIN(NORMAL, logger, m, "");
                    m << "Start MAS " << i << " with PhyMode " << DRPPatternCreator::getPhyMode();
                MESSAGE_END();
            }

            assure(i<tmpDRPAllocMap.size(), "Vector boundary exeeded; loop error!");
            tmpDRPAllocMap[i] = false;
            adjacent++;
        }
        else
        {
            if(adjacent != 0)
            {
                break;
            }
        }
        i++;
    }

    if(adjacent != 0)
    {
        duration = adjacent * MASduration - 1E-12;

        MESSAGE_BEGIN(NORMAL, logger, m, "");
            m << "The Timer-duration is set to " << duration << ". There are " << adjacent << " adjacent slots.";
        MESSAGE_END();

        adjacent = 0;
        wns::events::scheduler::Callable call = boost::bind(&DRPManager::Transmit, this, duration);
        scheduler->scheduleDelay(call, ReservationStart);


        MESSAGE_BEGIN(NORMAL, logger, m, "");
            m << "Set reservation timer "
            <<" Start Time: " << ReservationStart
            <<" Duration: " << duration;
        MESSAGE_END();

    }
    else
    {
        MESSAGE_BEGIN(NORMAL, logger, m, "");
        m << "There are no more MAS reserved in this superframe";
            MESSAGE_END();
    }
    if(i < tmpDRPAllocMap.size())
        RegisterReservation(BPDuration);


}


void
DRPManager::Transmit(wns::simulator::Time duration)
{
        MESSAGE_BEGIN(NORMAL, logger, m, "");
        m << "It's time to transmit for mac " << peerAddress;
            MESSAGE_END();

    //its time to transmit, signal
    QueueInterface->TimeToTransmit(peerAddress, duration);
}
