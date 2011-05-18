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

#include <WIMEMAC/management/BeaconEvaluator.hpp>
#include <WIMEMAC/lowerMAC/Manager.hpp>
#include "BeaconCommand.hpp"

using namespace wimemac::management;

BeaconEvaluator::BeaconEvaluator()
{
   
    DRPmapManager = new drp::DRPmap(256);
    hasPendingProbe = false;
    requestedProbes.drpAvailability = 0;
}

void
BeaconEvaluator::SetLogger(wns::logger::Logger _logger)
{
    logger = _logger;
    DRPmapManager->SetLogger(logger);
}

wimemac::convergence::PhyMode
BeaconEvaluator::getPhyMode(wns::service::dll::UnicastAddress rx, int masNumber)
{
    
    if (DRPOutgoingConnections.knows(rx))
    {
        DRPmanager = DRPOutgoingConnections.find(rx);
        return DRPmanager->getPhyMode();
    }
    else
    {
        if(friends.manager->getPCAchannelAccess())
        {
            return friends.manager->getDefaultPhyMode();
        }
        else
            assure(false,"If only using DRP as channel access it shouldn't be asked for the PhyMode until a DRPManager exists");
    }
}

void
BeaconEvaluator::BeaconExamination(wns::service::dll::UnicastAddress tx, wns::service::dll::UnicastAddress iam, BeaconCommand* BeaconCommand,
    wns::logger::Logger _logger)
{
    ExamineBeaconPeriodOccupancy(tx,BeaconCommand);
    
    logger =_logger;
    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Receive Beacon from device:" << tx);

    // Fill rxAvailabilityBitamp
    if (BeaconCommand->peer.HasAvailabilityIE)
    {
        // If sender is already known update bitmap
        if (rxAvailabilityBitmap.knows(tx))
        {
            rxAvailabilityBitmap.update(tx, BeaconCommand->peer.availabilityBitmap);
        }
        // Else insert new element
        else rxAvailabilityBitmap.insert(tx , BeaconCommand->peer.availabilityBitmap);
    }

    // Evaluate Probes sent to me with the beacon
    if(BeaconCommand->HasProbeIE(iam))
    {
        std::queue<BeaconCommand::ProbeElementID> probeElements = BeaconCommand->peer.probe.find(iam);
        while (probeElements.size() > 0)
        {
            // get first requested element ID
            BeaconCommand::ProbeElementID elementID = probeElements.front();
            probeElements.pop();

            MESSAGE_SINGLE(NORMAL, logger, "Received Probe asking for IE : " << elementID);
            if (elementID == BeaconCommand::DRPAvailability)
            {
                // set counter to include IE in the next beacons, beginning next SF
                if (requestedProbes.drpAvailability > 0)
                {
                    // The IE was already requested, reset the counter to max
                    requestedProbes.drpAvailability = 3; // in the next x beacons the IE is included
                }
                // The IE was not included the in the last beacon. Set counter to -1 to signal that the IE shall be included in the next beacons beginning next SF
                else requestedProbes.drpAvailability = -1;
            }
            else assure(false, "The requested element ID : " << elementID << " is not known");
        }

    } // end probe evaluation

    //if drp connection is always known, update the information if necessary, else create a new DRPManager and insert it into DRPConnections

    if(BeaconCommand->peer.HasDRPIE)
    {   int i = 1;
        while(BeaconCommand->GetDRPIESize()!=0)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Evaluate DRPIE nr. " << i);
            i++;
            BeaconCommand::DRP DRPIECommand;
            DRPIECommand = BeaconCommand->GetDRPIE();

            if(DRPIECommand.address == iam)
            {
                if(!DRPIncomingConnections.knows(tx) && DRPIECommand.devicetype == BeaconCommand::Owner)
                {
                    //peer.address as a unique identifier for a specific drp connection
                    //This device doesn't know the drp connection, hence create a new drp manager for a new incoming
                    //connection. Set the parameters according to incoming beacon command
                    DRPIncomingConnections.insert(tx, new drp::DRPManager(tx,
                        DRPIECommand.reservationtype,DRPIECommand.reasoncode, BeaconCommand::Target,
                        DRPIECommand.DRPAlloc, queueInterface, logger, friends.manager, NumberOfBPSlots, friends.manager->getRateAdaptation(), friends.manager->getPatternPEROffset()));

                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator has built a new drp connection manager for "
                        << "transmission owner " << tx);
                }
                else if(DRPIncomingConnections.knows(tx) && DRPIECommand.devicetype == BeaconCommand::Owner)
                {
                    DRPmanager = DRPIncomingConnections.find(tx);
                    if(DRPIECommand.isAdditionalDRPIE == false)
                    {
                        // DRPIE is a standard DRPIE from the transmission owner
                        if(DRPIECommand.reasoncode != BeaconCommand::Accept)
                        {
                            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Status "
                            << DRPIECommand.status << " and Reason Code " << DRPIECommand.reasoncode);
                            DRPmanager->SetReasonCode(DRPIECommand.reasoncode);

                            if(DRPIECommand.status == false)
                                DRPmanager->SetStatus(DRPIECommand.status);

                            if(DRPIECommand.reasoncode == BeaconCommand::Modified)
                                DRPmanager->SetPattern(DRPIECommand.DRPAlloc);
                        }
                    }
                    else
                    {
                        // DRPIE is an additional DRPIE created for merging by the transmission owner
                        if(DRPmanager->HasPendingDRPMerge())
                        {
                            // DRP merge is already known -> update
                            if(DRPIECommand.reasoncode != BeaconCommand::Accept)
                            {
                                MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Status for merge DRPIE"
                                << DRPIECommand.status);
                                DRPmanager->SetMergeReasonCode(DRPIECommand.reasoncode);

                                if(DRPIECommand.status == false)
                                    DRPmanager->SetMergeStatus(DRPIECommand.status);

                                if(DRPIECommand.reasoncode == BeaconCommand::Modified)
                                    DRPmanager->SetMergePattern(DRPIECommand.DRPAlloc);
                            }
                        }
                        else
                        {
                            // DRP merge is unknown -> inform DRP Manager about new reservation for merge
                            DRPmanager->AddReservation(DRPIECommand.reasoncode, DRPIECommand.DRPAlloc);
                        }
                    }

                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Owner address " << tx << " is already known");
                }
                else if(DRPOutgoingConnections.knows(tx) && DRPIECommand.devicetype == BeaconCommand::Target)
                {
                    DRPmanager = DRPOutgoingConnections.find(tx);

                    if(DRPIECommand.isAdditionalDRPIE == false)
                    {
                        // DRPIE is a standard DPRIE from the transmission target
                        DRPmanager->SetReasonCode(DRPIECommand.reasoncode);
                        DRPmanager->SetStatus(DRPIECommand.status);
                    }
                    else
                    {
                        // DRPIE is an additional DRPIE created for merging
                        // update status of merge process
                        DRPmanager->SetMergeReasonCode(DRPIECommand.reasoncode);
                        DRPmanager->SetMergeStatus(DRPIECommand.status);

                        if(DRPIECommand.status == true && DRPIECommand.reasoncode == BeaconCommand::Accept)
                        {
                            assure(DRPIECommand.DRPAlloc == DRPmanager->GetMergePattern()  , "Accepted additional pattern from " << tx << " differs from pattern of owner");
                            
                            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Merge DRPIE was accepted -> merging reservation patterns");
                            DRPmanager->MergePatterns();
                            
                        }
                    }

                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Target address " << tx << " is already known");
                }
            }

            //Updating the maps if I'm the reservation target and the reservation is not acknowleged yet will cause a conflict
            //(the global maps mark the proposed slots as occupied during negotation)
            //If DRPIECommand.status is true, the reservation pattern is acknowleged, hence update the DRP Maps.

            if(DRPIECommand.status == true)
            {
                MESSAGE_SINGLE(NORMAL, logger, "Updating my DRPMap; Received 'Accept' from address " << tx << " who accepts the connection with address " << DRPIECommand.address << "; DeviceType: " << DRPIECommand.devicetype);
                if(DRPIECommand.reservationtype == BeaconCommand::Hard)
                {
                    UpdateGlobalHardDRPMap(DRPIECommand.DRPAlloc);
                }
                else if(DRPIECommand.reservationtype == BeaconCommand::Soft)
                {
                    UpdateGlobalSoftDRPMap(DRPIECommand.DRPAlloc);
                }

                if(DRPIECommand.address != iam)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "There is a valid connection now between address " << DRPIECommand.address << " (Owner) and address " << tx << " (Target).");
                }
            }
            else
            {
                if(DRPIECommand.address != iam)
                {
                    
                    if (!(DRPIECommand.reasoncode == BeaconCommand::Conflict || DRPIECommand.reasoncode == BeaconCommand::Denied))
                    {
                        // Only respect the proposed pattern if it's not for me! Otherwise the negotiation will be conflicted
                        MESSAGE_SINGLE(NORMAL, logger, "Received a DRPIE with pending negotiation for address " << DRPIECommand.address << " from address " << tx << ". The proposed reservation will be respected until final state is known");
                        
                        // Since only other DRP reservations need to respect the ongoing negotiation PCA is still usable -> Soft DRP Map
                        if (DRPIECommand.devicetype == BeaconCommand::Target)
                        {
                            DRPmapManager->UpdatePendingDRPMap(DRPIECommand.address, tx, DRPIECommand.DRPAlloc);
                        }
                        else
                        {
                            DRPmapManager->UpdatePendingDRPMap(tx, DRPIECommand.address, DRPIECommand.DRPAlloc);
                        }
                    }
                    else
                    {
                        assure(DRPIECommand.devicetype == BeaconCommand::Target, "Received a reasoncode conflict/denied from an Owner! Not an expected behaviour" );
                        
                        MESSAGE_SINGLE(NORMAL, logger, "Received a DRPIE with a denied reservation for address " << DRPIECommand.address << " from address " << tx << ". Earlier respected MASs for this connection are marked as free");
                        
                        DRPmapManager->ReleasePendingDRPMap(DRPIECommand.address, tx);
                    }
                
                
                }
            }
        }
    }
    
    
    
} // end BeaconExamination

void
BeaconEvaluator::CreateDRPMaps()
{
    for(it = DRPOutgoingConnections.begin(); it != DRPOutgoingConnections.end() ;++it)
    {
        if((*it).second->GetDeviceType() == BeaconCommand::Owner)
        {
            Vector tmpGlobal(256,false);
            DRPmapManager->GetGlobalPattern(tmpGlobal);

            //bool areMASsAvailable = DRPmapManager->IsSpaceInGlobalPattern();
            bool areMASsAvailable = (*it).second->areMASsAvailable(tmpGlobal);

            // create new drp maps for the upcoming BP
            if((*it).second->IsMapCreated() == false)
            {
                if(areMASsAvailable)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: create a new reservation map for" 
                            << " the outgoing connection to target " << (*it).second->GetAddress());
                    (*it).second->FindNewPattern(tmpGlobal);
                    // Update own Soft DRP Map in order to respect the just requested MASs in case further reservations need to be generated
                    UpdateGlobalSoftDRPMap((*it).second->GetPattern());
                }
                else MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: All MASs in the SF are already reserverd. Cannot create an initial pattern");
            }

            if((*it).second->IsMapCreated() == true)
            {
                // A change in phymode could have caused the need for more reserved MASs
                if((*it).second->NeedsAdditionalPattern())
                {
                    if(areMASsAvailable)
                    {
                    
                        MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: create an additional reservation map for"
                                << " the outgoing connection to target" << (*it).second->GetAddress());
                        (*it).second->CreateAdditionalPattern(tmpGlobal);
                        // Update own Soft DRP Map in order to respect the just requested MASs in case further reservations need to be generated
                        UpdateGlobalSoftDRPMap((*it).second->GetMergePattern());
                    }
                    else MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: All MASs in the SF are already reserverd. Cannot create an additional pattern");
                }
            }
        }
    }
}

//BP has finished, update the upcoming drp reservations, set drp connection timers. This information is known by each DRPManager, hence notify each DRPManager to build DRPmaps
int
BeaconEvaluator::CollectDRPmaps(wns::simulator::Time BPDuration_)
{
    // set Probe counters to assure requested IEs are put into the beacons beginning next SF
    if (requestedProbes.drpAvailability < 0)
    {
        // requestedProbes.drpAvailability < 0 means a Probe was received this BP -> answer through beacon in next SF
        requestedProbes.drpAvailability = 3;
    }

    int reservedMASs = 0;
    tmpAllocatedMAS = Vector(256,false);

    // update upcoming drp reservations
    for(it = DRPOutgoingConnections.begin(); it != DRPOutgoingConnections.end() ;++it)
    {
        if((*it).second->GetReasonCode() == BeaconCommand::Accept && (*it).second->GetStatus() == true)
        {
            Vector drpMap_ = (*it).second->GetPattern();
            for(int i = 0; i < drpMap_.size(); i++)
            {
                if(drpMap_[i] == true)
                {
                    assure(i < drpMap_.size(), "Vector boundary exeeded; loop error!");
                    reservedMASs +=1;
                    tmpAllocatedMAS[i] = true;
                }
            }

            (*it).second->StartRegisterReservation(BPDuration_);
            MESSAGE_SINGLE(NORMAL, logger, "StartRegister for target " << (*it).second->GetAddress());
        }
    }

    return reservedMASs;
}

void
BeaconEvaluator::EvaluateConnection()
{
    for(it = DRPIncomingConnections.begin(); it != DRPIncomingConnections.end() ;++it)
    {
        // For all target managers check if there are new unestablished connections and evaluate pattern
        if((*it).second->GetStatus() == false)
        {
            if((*it).second->GetReasonCode() == BeaconCommand::Conflict || (*it).second->GetReasonCode() == BeaconCommand::Denied)
            {
                MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: DRP reservation with " << (*it).second->GetAddress() << " was conflicted/denied,"
                    <<" keeping status until Owner sends a modified reservation." );
            }
            else
            {
                if(DRPmapManager->PossiblePattern((*it).second->GetPattern()))
                {
                    (*it).second->SetStatus(true);
                    (*it).second->SetReasonCode(BeaconCommand::Accept);
                    
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: DRP reservation with " << (*it).second->GetAddress() << " is possible,"
                        <<" setting status to true." );
                        
                        for(int i = 0; i < 256; i++)
                        {
                        if((*it).second->GetPattern()[i] == true)
                        {
                            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: accepting MAS " << i );
                        }
                    
                        }

                }
                else
                {
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: DRP reservation with "<< (*it).second->GetAddress() << " is impossible,"
                        <<" set ReasonCode to conflict");
                    (*it).second->SetReasonCode(BeaconCommand::Conflict);
                    requestedProbes.drpAvailability = 3; // Send complete DRP Availability IE with conflict response
                }
            }
        }

        // Check for merge patterns
        if((*it).second->HasPendingDRPMerge() && (*it).second->GetMergeStatus() == false)
        {
            if((*it).second->GetMergeReasonCode() == BeaconCommand::Conflict || (*it).second->GetMergeReasonCode() == BeaconCommand::Denied)
            {
                MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: additional DRP reservation with " << (*it).second->GetAddress() << " was conflicted/denied,"
                    <<" keeping status until Owner sends a modified reservation." );
            }
            else
            {
                if(DRPmapManager->PossiblePattern((*it).second->GetMergePattern()))
                {
                    (*it).second->SetMergeStatus(true);
                    (*it).second->SetMergeReasonCode(BeaconCommand::Accept);
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: additional DRP reservation with " << (*it).second->GetAddress() << " is possible,"
                        <<" setting status to true.");

                }
                else
                {
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: additional DRP reservation with " << (*it).second->GetAddress() << " is impossible,"
                        <<" set ReasonCode to conflict");
                    (*it).second->SetMergeReasonCode(BeaconCommand::Conflict);
                    requestedProbes.drpAvailability = 3; // Send complete DRP Availability IE with conflict response
                }
            }
        }

        if((*it).second->GetReasonCode() == BeaconCommand::Accept && (*it).second->GetStatus() == true)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating DRP-Map of this station with a reservation as target with partner " << (*it).second->GetAddress());
            if((*it).second->GetReservationType() == BeaconCommand::Hard)
            {
                UpdateGlobalHardDRPMap((*it).second->GetPattern());
            }
            else if((*it).second->GetReservationType() == BeaconCommand::Soft)
            {
                UpdateGlobalSoftDRPMap((*it).second->GetPattern());
            }
        }
        if((*it).second->HasPendingDRPMerge() && (*it).second->GetMergeReasonCode() == BeaconCommand::Accept && (*it).second->GetMergeStatus() == true)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating DRP-Map of this station with an additional reservation as target with partner " << (*it).second->GetAddress());
            if((*it).second->GetReservationType() == BeaconCommand::Hard)
            {
                UpdateGlobalHardDRPMap((*it).second->GetMergePattern());
            }
            else if((*it).second->GetReservationType() == BeaconCommand::Soft)
            {
                UpdateGlobalSoftDRPMap((*it).second->GetMergePattern());
            }
        }

    }

    for(it = DRPOutgoingConnections.begin(); it != DRPOutgoingConnections.end() ;++it)
    {
        // Evaluate conflicted reservations
        if((*it).second->GetReasonCode() == BeaconCommand::Conflict)
        {
            (*it).second->ResolveConflict();
        }
        if((*it).second->GetMergeReasonCode() == BeaconCommand::Conflict)
        {
            (*it).second->ResolveMergeConflict();
        }
    
    }
}

void
BeaconEvaluator::setFriend(helper::IDRPQueueInterface* QueueInterface)
{
    queueInterface = QueueInterface;
}

void
BeaconEvaluator::setManagerFriend(wimemac::lowerMAC::Manager* manager_)
{
    friends.manager = manager_;
}

void
BeaconEvaluator::SetBPDuration(wns::simulator::Time duration)
{
    BPDuration = duration;
    //Block the BP slots in the global drp map
    NumberOfBPSlots = ceil((int)(BPDuration*1E6) / 256);
    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Number of BP MASs set to : " << NumberOfBPSlots);

    DRPmapManager->setBPSlots(NumberOfBPSlots);
}

void
BeaconEvaluator::evaluatePERforConnections()
{

    for(it = DRPOutgoingConnections.begin(); it != DRPOutgoingConnections.end() ;++it)
    {
        // Evaluate the PER of the connections the device owns
        if( friends.manager->adjustMCSdown((*it).second->GetAddress()))
        {
            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: A lower PhyMode will be used for address : " << (*it).second->GetAddress());

            wimemac::convergence::PhyMode newPhyMode_ = (*it).second->getPhyMode();
            friends.manager->getPhyUser()->getPhyModeProvider()->mcsDown(newPhyMode_);

            if (newPhyMode_ < (*it).second->getPhyMode())
                (*it).second->UpdatePhyModeDown(newPhyMode_);
            else MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: The lowest PhyMode is already used. Proceed without changes");
        }
    }
}


//For outgoing connection compounds are queued (see DRPScheduler, Queues). If the target device's mac address isn't known,
// a new Queue will be created to store the outgoing device. CreateDRPManager will be invoked by DRPScheduler
// to create a new drp manager for the new outgoing drp connection, this will cause a drp reservation negotation

void
BeaconEvaluator::CreateDRPManager(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize, wimemac::convergence::PhyMode DefPhyMode)
{
    if(DRPOutgoingConnections.knows(rx))
    {
        MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: That's not a new DRP Connection as owner" );

    }
    else
    {
        Vector emptyInitMap_(256,false);
        
        MESSAGE_SINGLE(NORMAL, logger, "Create a new DRP Manager" );

        //create a new drp manager, set the parameters according to drp reservation negotation requirement
        //for own outgoing reservation DeviceType: Owner, ReservationStatus: 0, ReasonCode: Accept
        DRPOutgoingConnections.insert(rx, new drp::DRPManager(rx, BeaconCommand::Hard,BeaconCommand::Accept, BeaconCommand::Owner, emptyInitMap_, queueInterface, logger, friends.manager,  NumberOfBPSlots, friends.manager->getRateAdaptation(), friends.manager->getPatternPEROffset(), DefPhyMode));

        DRPmanager = DRPOutgoingConnections.find(rx);

        DRPmanager->SetTrafficChar(CompoundspSF,BitspSF,MaxCompoundSize, friends.manager->getReservationBlocks());
        MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: "
                                <<"DRP reservation with DeviceType "<< DRPmanager->GetDeviceType());


        MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator has built a new drp connection manager for an outgoing" 
        << " DRP reservation, target is "<< rx);

    }
}

void
BeaconEvaluator::UpdateDRPManager(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    assure(DRPOutgoingConnections.knows(rx), "Trying to update DRPManager for target " << rx << ", but target does not exist in DRPOutgoingConnections");
    DRPmanager = DRPOutgoingConnections.find(rx);
    DRPmanager->UpdateTrafficChar(CompoundspSF,BitspSF,MaxCompoundSize);
}

bool
BeaconEvaluator::CreateBPOIE(BeaconCommand* BeaconCommand)
{
  
  ExamineBeaconPeriodOccupancy(friends.manager->getMACAddress(),BeaconCommand);
  BeaconCommand->peer.BPOIE.BPsize = BPoccupancy.size();
  BeaconCommand->peer.BPOIE.IE = BPoccupancy;
}


//BeaconBuilder builts a new beacon compound and asked whether a DRPIE should be included. If there are already established connections a DRPIE must be included, for new connection it should be included to start negotiation
bool
BeaconEvaluator::CreateDRPIE(BeaconCommand* BeaconCommand)
{
    bool HasCreated = false;

    for(it = DRPOutgoingConnections.begin(); it != DRPOutgoingConnections.end() ;++it)
    {
        if( (*it).second->IsMapCreated() ) // Reservation partner shall also be informed about conflicts
        {
            BeaconCommand::DRP DRPIECommand;
            DRPIECommand.reservationtype = (*it).second->GetReservationType();
            DRPIECommand.reasoncode = (*it).second->GetReasonCode();
            DRPIECommand.devicetype = (*it).second->GetDeviceType();
            DRPIECommand.address = (*it).second->GetAddress();
            DRPIECommand.status = (*it).second->GetStatus();
            DRPIECommand.DRPAlloc = (*it).second->GetPattern();
            DRPIECommand.isAdditionalDRPIE = false;
            BeaconCommand->peer.HasDRPIE = true;

            BeaconCommand->PutDRPIE(DRPIECommand);
            HasCreated = true;

            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: build DRPIE as owner, reasoncode " << DRPIECommand.reasoncode <<" and "
                <<"Status " << DRPIECommand.status);

            if ((*it).second->HasPendingDRPMerge())
            {
                // There is an additional DRP Map pending for acceptance and merge
                BeaconCommand::DRP DRPIECommand;
                DRPIECommand.reservationtype = (*it).second->GetReservationType();
                DRPIECommand.devicetype = (*it).second->GetDeviceType();
                DRPIECommand.address = (*it).second->GetAddress();

                DRPIECommand.reasoncode = (*it).second->GetMergeReasonCode();
                DRPIECommand.status = (*it).second->GetMergeStatus();
                DRPIECommand.DRPAlloc = (*it).second->GetMergePattern();

                DRPIECommand.isAdditionalDRPIE = true;

                BeaconCommand->PutDRPIE(DRPIECommand);

                MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: build additional DRPIE as owner for merge, reasoncode " << DRPIECommand.reasoncode <<" and Status " << DRPIECommand.status);

                // Additional pattern was accepted -> merge with original one
                if (DRPIECommand.status == true) assure(false, "False State: Owner should have merged an additional pattern with activated status");
            }
        }

        if((*it).second->GetReasonCode() == BeaconCommand::Accept && (*it).second->GetStatus() == true)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating DRP-Map of this station with an active reservation as owner with " << (*it).second->GetAddress() );
            if((*it).second->GetReservationType() == BeaconCommand::Hard)
            {
                UpdateGlobalHardDRPMap((*it).second->GetPattern());
            }
            else if((*it).second->GetReservationType() == BeaconCommand::Soft)
            {
                UpdateGlobalSoftDRPMap((*it).second->GetPattern());
            }
        }
    }

    for(it = DRPIncomingConnections.begin(); it != DRPIncomingConnections.end() ;++it)
    {
        if( (*it).second->IsMapCreated() ) // Reservation partner shall also be informed about conflicts
        {
            BeaconCommand::DRP DRPIECommand;
            DRPIECommand.reservationtype = (*it).second->GetReservationType();
            DRPIECommand.reasoncode = (*it).second->GetReasonCode();
            DRPIECommand.devicetype = (*it).second->GetDeviceType();
            DRPIECommand.address = (*it).second->GetAddress();
            DRPIECommand.status = (*it).second->GetStatus();
            DRPIECommand.DRPAlloc = (*it).second->GetPattern();
            DRPIECommand.isAdditionalDRPIE = false;
            BeaconCommand->peer.HasDRPIE = true;
            BeaconCommand->PutDRPIE(DRPIECommand);
            HasCreated = true;

            MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: build DRPIE as target, reasoncode " << DRPIECommand.reasoncode <<" and "
                <<"Status " << DRPIECommand.status);

            if ((*it).second->HasPendingDRPMerge())
            {
                // There is an additional DRP Map pending for acceptance and merge
                BeaconCommand::DRP DRPIECommand;
                DRPIECommand.reservationtype = (*it).second->GetReservationType();
                DRPIECommand.devicetype = (*it).second->GetDeviceType();
                DRPIECommand.address = (*it).second->GetAddress();

                DRPIECommand.reasoncode = (*it).second->GetMergeReasonCode();
                DRPIECommand.status = (*it).second->GetMergeStatus();
                DRPIECommand.DRPAlloc = (*it).second->GetMergePattern();

                DRPIECommand.isAdditionalDRPIE = true;

                BeaconCommand->PutDRPIE(DRPIECommand);

                MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: build additional DRPIE as target for merge, reasoncode " << DRPIECommand.reasoncode <<" and Status " << DRPIECommand.status);

                // Additional pattern was accepted -> merge with original one
                if (DRPIECommand.reasoncode == BeaconCommand::Accept && DRPIECommand.status == true)
                {
                    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Accepting DRP merge pattern -> merging reservation patterns");
                    (*it).second->MergePatterns();
                }
            }
        }

        if((*it).second->GetReasonCode() == BeaconCommand::Accept && (*it).second->GetStatus() == true)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating DRP-Map of this station with an active reservation as target with partner " << (*it).second->GetAddress());
            if((*it).second->GetReservationType() == BeaconCommand::Hard)
            {
                UpdateGlobalHardDRPMap((*it).second->GetPattern());
            }
            else if((*it).second->GetReservationType() == BeaconCommand::Soft)
            {
                UpdateGlobalSoftDRPMap((*it).second->GetPattern());
            }
        }
        if ((*it).second->HasPendingDRPMerge() && (*it).second->GetMergeReasonCode() == BeaconCommand::Accept && (*it).second->GetMergeStatus() == true)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Updating DRP-Map of this station with an additional active reservation as target with partner" << (*it).second->GetAddress());
            if((*it).second->GetReservationType() == BeaconCommand::Hard)
            {
                UpdateGlobalHardDRPMap((*it).second->GetMergePattern());
            }
            else if((*it).second->GetReservationType() == BeaconCommand::Soft)
            {
                UpdateGlobalSoftDRPMap((*it).second->GetMergePattern());
            }
        }
    }


    if(!HasCreated)
        MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: neither active nor pending connection, no stuff for a DRP InformationElement");
    return HasCreated;
}

bool
BeaconEvaluator::CreateProbeIE(BeaconCommand* BeaconCommand)
{
    if (hasPendingProbe)
    {
        // Put Probe into beacon
        BeaconCommand->peer.probe = pendingProbe;

        // Reset internal Probe storage
        pendingProbe.clear();
        hasPendingProbe = false;

        return true;
    }
    else return false;
}

void
BeaconEvaluator::RequestIE(wns::service::dll::UnicastAddress rx, BeaconCommand::ProbeElementID elementID)
{
    // Insert IE request into next beacon probe
    if (pendingProbe.knows(rx))
    {
        // Push requested IE into existing Probe IE for that recipient
        pendingProbe.find(rx).push(elementID);
    }
    else
    {
        // Create a new Probe IE with the requested IE ID
        std::queue<BeaconCommand::ProbeElementID> probeElements;
        probeElements.push(elementID);
        pendingProbe.insert(rx, probeElements);
        hasPendingProbe = true;
    }
}

void
BeaconEvaluator::UpdateGlobalSoftDRPMap(Vector UpdateMap)
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Soft");
    DRPmapManager->UpdateSoftDRPmap(UpdateMap, logger);
}


void
BeaconEvaluator::UpdateGlobalHardDRPMap(Vector UpdateMap)
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Hard");
    DRPmapManager->UpdateHardDRPmap(UpdateMap, logger);
}

void
BeaconEvaluator::UpdateGlobalDRPMap(Vector UpdateMap)
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconEvaluator: Update Global");
    DRPmapManager->UpdateHardDRPmap(UpdateMap, logger);
    DRPmapManager->UpdateSoftDRPmap(UpdateMap, logger);
}

bool
BeaconEvaluator::UpdateMapWithPeerAvailabilityMap(wns::service::dll::UnicastAddress rx , Vector& DRPMap)
{
    if (rxAvailabilityBitmap.knows(rx))
    {
        Vector availabilityMap_ = rxAvailabilityBitmap.find(rx);
        bool hasUpdated = false;
        for (int i = 0; i < availabilityMap_.size(); i++)
        {
            assure(i < availabilityMap_.size() && i < DRPMap.size(), "Vector boundary exeeded; loop error!");
            if(availabilityMap_[i] == false)
            {
                // Return true only, if the availability map decreases the usable slots
                if(DRPMap[i] == false) hasUpdated = true;

                DRPMap[i] = true;
            }
        }
        return hasUpdated;
    }
    else return false;
}

void
BeaconEvaluator::ClearAvailabilityBitmap()
{
    rxAvailabilityBitmap.clear();
}

Vector
BeaconEvaluator::getAllocatedMASs()
{
    return tmpAllocatedMAS;
}

Vector
BeaconEvaluator::getReservedMASs()
{
    return DRPmapManager->GetGlobalHardDRPmap();
}

//TODO: the Beacon Period Slot Duration must be inserted instead of 85E-6
//returns the beacon slot related to time of calling this function
int
BeaconEvaluator::getBeaconSlot()
{
  BPStartTime = friends.manager-> getBPStartTime();
  wns::simulator::Time timenow = wns::simulator::getEventScheduler()->getTime();
  
  MESSAGE_SINGLE(NORMAL, logger, "ExamineBeaconPeriodOccupancy, time now:  " << timenow 
  << " BP Start " << BPStartTime); 
  
  int BeaconSlot = floor((timenow-BPStartTime)/85E-6);
  return(BeaconSlot);
  
}

void 
BeaconEvaluator::ExamineBeaconPeriodOccupancy(wns::service::dll::UnicastAddress tx, BeaconCommand* BeaconCommand)
{
  
  int BeaconSlot = getBeaconSlot();
  
  wns::service::dll::UnicastAddress inValidAddress = wns::service::dll::UnicastAddress::UnicastAddress();
  
  for (int i = 0 ; i < BeaconCommand->peer.BPOIE.IE.size();i++)
  {
    MESSAGE_SINGLE(NORMAL, logger, "ExamineBeaconPeriodOccupancy, I've got a BPOIE from my neighbour " << tx <<", lets have a look:  " 
    << i
    << " Address: " << BeaconCommand->peer.BPOIE.IE[i]);
  }
  
  //if the current Beacon Period Occupancy Vector is smaller than the Beacon Period Size announced in the received Beacon, the Vector will be increased to store the information
  if(BeaconCommand->peer.BPOIE.IE.size() > BPoccupancy.size())
  {
    MESSAGE_SINGLE(NORMAL, logger, "Resize BPOIE size" << BeaconCommand->peer.BPOIE.IE.size() << " own Size " << BPoccupancy.size());
    
     //BPoccupancy.resize(BeaconCommand->peer.BPOIE.IE.size() - BPoccupancy.size(), inValidAddress);
     BPoccupancy.resize(BeaconCommand->peer.BPOIE.IE.size(), inValidAddress);
  }
    
  if(BeaconSlot >= BPoccupancy.size())
  {
    BPoccupancy.resize(BeaconSlot+1, inValidAddress);
  }
      
  MESSAGE_SINGLE(NORMAL, logger, "ExamineBeaconPeriodOccupancy, received Beacon in Beacon Slot:  " << BeaconSlot 
  << "Beacon is originated from  " << tx); 
              
  wns::service::dll::UnicastAddress address = BPoccupancy.at(BeaconSlot);
  if(!address.isValid())
  {
    MESSAGE_SINGLE(NORMAL, logger, "ExamineBeaconPeriodOccupancy, invalid");
    
    assure(BeaconSlot <= BPoccupancy.size(), "Element is not part of BPoccupancy, limit is exceeded");
    BPoccupancy[BeaconSlot] = tx;
  }
  
  for(int i = 0 ; i < BPoccupancy.size(); i++)
  {
    MESSAGE_SINGLE(NORMAL, logger, "ExamineBeaconPeriodOccupancy, BPoccupancy is now as followed: " << i 
  << " tx " << BPoccupancy[i]); 
  }
  
  //if the neighbour address is known, update the BPOIE or store the new element
  for(int i = 0; i < NeighboursBPoccupancy.size(); i++)
  {
    if(NeighboursBPoccupancy[i].source == tx)
    {
      MESSAGE_SINGLE(NORMAL, logger, "This neighbour is known, update the BPOIE");
      NeighboursBPoccupancy[i].BPoccupancy = BeaconCommand->peer.BPOIE.IE;      
      break;
    }
    else if(NeighboursBPoccupancy[i].source != tx && i == NeighboursBPoccupancy.size() - 1)
    {
      MESSAGE_SINGLE(NORMAL, logger, "This neighbour isn't known, insert it's BPOIE into the container");
      InsertInNeighoursBPoccupancy(tx, BeaconCommand);
    }
    
  }
  //if the container to store the BPOIEs of the neighbours is empty, insert the received BPOIE as the first element
  if(NeighboursBPoccupancy.size() == 0)
  {
    MESSAGE_SINGLE(NORMAL, logger, "This is the first received BPOIE, insert it into the container");
    InsertInNeighoursBPoccupancy(tx, BeaconCommand);
    
  }
       
}

void 
BeaconEvaluator::InsertInNeighoursBPoccupancy(wns::service::dll::UnicastAddress tx, BeaconCommand* BeaconCommand)
{
    struct BPallocation BPallocation_;
    BPallocation_.source = tx;
    BPallocation_.BPoccupancy = BeaconCommand->peer.BPOIE.IE;
    NeighboursBPoccupancy.push_back(BPallocation_);
  
}
