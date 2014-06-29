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


#include <WIMEMAC/drp/DRPmap.hpp>



using namespace wimemac::drp;

DRPmap::DRPmap(int globalMASpSF)

{
    globalSoftDRPmap.assign(globalMASpSF,false);
    globalHardDRPmap.assign(globalMASpSF,false);
    
    mMaxLostBeacons = 3;
    numberOfBPSlots = 1;
    
    // Initialise with empty reservation maps
    Vector empty_(globalMASpSF,false);
    globalHardDRPmapVec.push_back(empty_);
    globalSoftDRPmapVec.push_back(empty_);
}


void
DRPmap::SetLogger(wns::logger::Logger _logger)
{
  logger = _logger;
}

void
DRPmap::UpdateHardDRPmap(Vector UpdateHard, wns::logger::Logger _logger)
{
    logger = _logger;
    // Update mapVector
    UpdateDRPmap(UpdateHard, globalHardDRPmapVec.back());
    // Update current used hard reservation map
    UpdateDRPmap(UpdateHard, globalHardDRPmap);

}

void
DRPmap::UpdateSoftDRPmap(Vector UpdateSoft, wns::logger::Logger _logger)
{
    logger = _logger;
    // Update mapVector
    UpdateDRPmap(UpdateSoft, globalSoftDRPmapVec.back());
    // Update current used soft reservation map
    UpdateDRPmap(UpdateSoft, globalSoftDRPmap);
    
}

void
DRPmap::UpdatePendingDRPMap(wns::service::dll::UnicastAddress _owner, wns::service::dll::UnicastAddress _target, Vector _pendingMap)
{
    // Update Vector of last mMaxLostBeacons 
    // Check if current pending information contains this owner
    if(PendingDRPMapVec.back().knows(_owner))
    {
        // Check if current pending information of this owner contains this target
        if(PendingDRPMapVec.back().find(_owner).knows(_target))
        {
            assure(PendingDRPMapVec.back().find(_owner).find(_target) == _pendingMap, "DRPMapManager : The previously in this SF received pending DRP map differs from the just received DRP map" );
        }
        else
        {
            // Add the information regarding this target to the known owner
            PendingDRPMapVec.back().find(_owner).insert(_target, _pendingMap);
        }
    }
    else
    {
        // Add the information regarding this link as a new element
        MASsperStation _tmpMap;
        _tmpMap.insert(_target, _pendingMap);
        PendingDRPMapVec.back().insert(_owner, _tmpMap);
    }
    
    // Update pending DRP Map
    if(PendingDRPMap.knows(_owner))
    {
        if(PendingDRPMap.find(_owner).knows(_target))
        {
            
            if(PendingDRPMap.find(_owner).find(_target) != _pendingMap)
            {
                MESSAGE_SINGLE(NORMAL, logger, "DRPMapManager : The previously received pending DRP map differs from the just received DRP map -> dismiss old information");
                
                ReleasePendingDRPMap(_owner,_target);
                // Add new information
                UpdatePendingDRPMap(_owner, _target, _pendingMap);
            }
            
        }
        else
        {
            PendingDRPMap.find(_owner).insert(_target, _pendingMap);
        }
    }
    else
    {
        MASsperStation _tmpMap;
        _tmpMap.insert(_target, _pendingMap);
        PendingDRPMap.insert(_owner, _tmpMap);
    }
}

void
DRPmap::ReleasePendingDRPMap(wns::service::dll::UnicastAddress _owner, wns::service::dll::UnicastAddress _target)
{
    if(PendingDRPMap.knows(_owner))
    {
        if(PendingDRPMap.find(_owner).knows(_target))
        {
            // Delete from current map
            PendingDRPMap.find(_owner).erase(_target);
            if(PendingDRPMap.find(_owner).size() == 0) PendingDRPMap.erase(_owner);
            
            // Delete from history vector since a conflict negates all previous information
            for(int k = 0; k < PendingDRPMapVec.size(); k++)
            {
                if(PendingDRPMapVec[k].knows(_owner))
                {
                    if(PendingDRPMapVec[k].find(_owner).knows(_target))
                    {
                        PendingDRPMapVec[k].find(_owner).erase(_target);
                        if (PendingDRPMapVec[k].find(_owner).size() == 0) PendingDRPMapVec[k].erase(_owner);
                    }
                }
            }
        }
        else MESSAGE_SINGLE(NORMAL, logger, "DRPMapManager : There is no information regarding this target");
    }
    else MESSAGE_SINGLE(NORMAL, logger, "DRPMapManager : There is no information regarding this owner");
    
    
}

void
DRPmap::onBPStarted()
{
    // Fill globalDRPmaps with reservations received during the last mMaxLostBeacons beacons
    for (int i = 0; i < globalHardDRPmap.size(); i++)
    {
        bool hardSlot = false;
        int hardCount = 0;
        for(int k = 0; k < globalHardDRPmapVec.size(); k++)
        {
            assure(i < (globalHardDRPmapVec[k]).size(), "Vector boundary exeeded; loop error!");
            if ((globalHardDRPmapVec[k])[i] == true) hardCount += 1;
        }
        if(hardCount >= 1) hardSlot = true;

        bool softSlot = false;
        int softCount = 0;
        for(int l = 0; l < globalSoftDRPmapVec.size(); l++)
        {
            assure(i < (globalHardDRPmapVec[l]).size(), "Vector boundary exeeded; loop error!");
            if ((globalSoftDRPmapVec[l])[i] == true) softCount += 1;
        }
        if(softCount >= 1) softSlot = true;

        
        globalHardDRPmap[i] = hardSlot;
        globalSoftDRPmap[i] = softSlot;
        
    }

    PendingDRPMap.clear();
    wns::container::Registry<wns::service::dll::UnicastAddress, MASsperStation >::const_iterator it_owner;
    MASsperStation::const_iterator it_target;
    // Fill pendingDRPMap with pending reservations of the last mMaxLostBeacons beacons
    for(int n = 0; n < PendingDRPMapVec.size(); n++)
    {
        for(it_owner = PendingDRPMapVec[n].begin(); it_owner != PendingDRPMapVec[n].end(); it_owner++)
        {
            for(it_target = (*it_owner).second.begin(); it_target != (*it_owner).second.end(); it_target++)
            {
                if(PendingDRPMap.knows((*it_owner).first))
                {
                    if(PendingDRPMap.find((*it_owner).first).knows((*it_target).first))
                    {
                        assure(PendingDRPMap.find((*it_owner).first).find((*it_target).first) == (*it_target).second, "The Pending DRP Map Vector contains differing entries");
                    }
                    else
                    {
                        PendingDRPMap.find((*it_owner).first).insert((*it_target).first, (*it_target).second);
                    }
                }
                else
                {
                    MASsperStation _tmpPendingDRP;
                    _tmpPendingDRP.insert((*it_target).first, (*it_target).second);
                    PendingDRPMap.insert((*it_owner).first, _tmpPendingDRP);
                }
            }
        }
    }


    // Put new empty vector into queue for the next beacons reservations
    Vector empty_(globalHardDRPmap.size(), false);
    globalHardDRPmapVec.push_back(empty_);
    globalSoftDRPmapVec.push_back(empty_);
    wns::container::Registry<wns::service::dll::UnicastAddress, MASsperStation > emptyPending_;
    PendingDRPMapVec.push_back(emptyPending_);
    // Refill last drpmap in vector with BP slots
    setBPSlots(numberOfBPSlots);

    
    if( globalHardDRPmapVec.size() > mMaxLostBeacons)
        globalHardDRPmapVec.pop_front();
    if( globalSoftDRPmapVec.size() > mMaxLostBeacons)
        globalSoftDRPmapVec.pop_front();
    if( PendingDRPMapVec.size() > mMaxLostBeacons)
        PendingDRPMapVec.pop_front();

}

void
DRPmap::setBPSlots(int numberOfBPSlots_)
{
    numberOfBPSlots = numberOfBPSlots_;

    for (int i = 0; i < numberOfBPSlots; i++)
    {
        (globalHardDRPmapVec.back())[i] = true;
        (globalSoftDRPmapVec.back())[i] = true;
    }
}

Vector
DRPmap::GetGlobalHardDRPmap()
{
    GetPattern(logger);
    return globalHardDRPmap;
}

Vector
DRPmap::GetPendingDRPmap()
{
    Vector _pendingDRPmap = Vector(globalHardDRPmap.size(), false);
    
   wns::container::Registry<wns::service::dll::UnicastAddress, MASsperStation >::const_iterator it_owner;
   MASsperStation::const_iterator it_target;
    
   for(it_owner = PendingDRPMap.begin(); it_owner != PendingDRPMap.end(); it_owner++)
   {
       for(it_target = (*it_owner).second.begin(); it_target != (*it_owner).second.end(); it_target++)
       {
           UpdateDRPmap((*it_target).second, _pendingDRPmap);
       }
   }
   
   return _pendingDRPmap;
}

void
DRPmap::UpdateDRPmap(Vector UpdateMap, Vector& UpdatedMap)
{
    Vector::iterator it1 = UpdateMap.begin();
    Vector::iterator it2 = UpdatedMap.begin();

    for(;  it1!=UpdateMap.end() || it2 != UpdatedMap.end() ; ++it1, ++it2)
    {
        *it2 = *it1 | *it2;
    }
    MESSAGE_SINGLE(NORMAL, logger, "Update DRPMap");
}

bool
DRPmap::PossiblePattern(Vector CompareDRPMap)
{
    Vector::iterator it1 = globalHardDRPmap.begin();
    Vector::iterator it2 = globalSoftDRPmap.begin();
    Vector _pendingDRPmap = GetPendingDRPmap();
    Vector::iterator it3 = _pendingDRPmap.begin();
    Vector::iterator it4 = CompareDRPMap.begin();
    

    for(;  it1!=globalHardDRPmap.end() || it2 != globalSoftDRPmap.end() || it3 != _pendingDRPmap.end() || it4 != CompareDRPMap.end(); ++it1, ++it2,++it3, ++it4)
    {
        if((*it1 || *it2 || *it3) && *it4 == true)
            return false;
    }
    return true;
}

void
DRPmap::GetPattern(wns::logger::Logger _logger)
{
    logger = _logger;
    for(int i = 0; i< 256; i++)
    {

        assure(i < globalHardDRPmap.size(), "Vector boundary exeeded; loop error!");
        if((globalHardDRPmap[i]) == true)
            MESSAGE_SINGLE(NORMAL, logger, "Get Pattern Slot Nr "<<i << " is occupied");
    }
}

void
DRPmap::GetGlobalPattern(Vector& GlobalPattern)
{
    DRPmap::GetPattern(logger);
    Vector _pendingDRPmap = GetPendingDRPmap();
    
    for(int i = 0; i<globalHardDRPmap.size() ; i++)
    {
        GlobalPattern[i] = globalHardDRPmap[i] || globalSoftDRPmap[i] || _pendingDRPmap[i];
    }
    
}
