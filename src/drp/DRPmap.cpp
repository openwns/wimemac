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
    globalEraseDRPmap.assign(globalMASpSF,false);
    
    mMaxLostBeacons = 3;
    numberOfBPSlots = 1;
    isPatternValid = true;

    // Initialise with empty reservation maps
    Vector empty_(globalMASpSF,false);
    globalHardDRPmapVec.push_back(empty_);
    globalSoftDRPmapVec.push_back(empty_);
    globalEraseDRPmapVec.push_back(empty_);
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
    for(int i = 0; i < UpdateHard.size(); i++)
    {
      if(UpdateHard[i] == true && globalEraseDRPmap[i] == true) 
        UpdateHard[i] = false;
      
    }    
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
DRPmap::onBPStarted()
{
    isPatternValid = true;

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

        bool eraseSlot = false;
        int eraseCount = 0;
        for(int k = 0; k < globalEraseDRPmapVec.size(); k++)
        {
            assure(i < (globalEraseDRPmapVec[k]).size(), "Vector boundary exeeded; loop error!");
            if ((globalEraseDRPmapVec[k])[i] == true) eraseCount += 1;
        }
        if(hardCount >= 1) hardSlot = true;
        
        globalHardDRPmap[i] = hardSlot;
        globalSoftDRPmap[i] = softSlot;
        globalEraseDRPmap[i] = eraseSlot;
        
        if(globalEraseDRPmap[i] == true && (globalHardDRPmap[i] == true || globalSoftDRPmap[i] == true))
        {
          globalHardDRPmap[i] == false;
          globalSoftDRPmap[i] == false;
        }
        
    }

    // Put new empty vector into queue for the next beacons reservations
    Vector empty_(globalHardDRPmap.size(), false);
    globalHardDRPmapVec.push_back(empty_);
    globalSoftDRPmapVec.push_back(empty_);
    globalEraseDRPmapVec.push_back(empty_);
    // Refill last drpmap in vector with BP slots
    setBPSlots(numberOfBPSlots);

    if( globalHardDRPmapVec.size() > mMaxLostBeacons)
        globalHardDRPmapVec.pop_front();
    if( globalSoftDRPmapVec.size() > mMaxLostBeacons)
        globalSoftDRPmapVec.pop_front();
    if( globalEraseDRPmapVec.size() > mMaxLostBeacons)
        globalEraseDRPmapVec.pop_front();
    

}

bool
DRPmap::isPatternValidated()
{
    return isPatternValid;
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

void
DRPmap::UpdateDRPmap(Vector UpdateMap, Vector& UpdatedMap)
{
    Vector::iterator it1 = UpdateMap.begin();
    Vector::iterator it2 = UpdatedMap.begin();

    for(it1, it2;  it1!=UpdateMap.end() || it2 != UpdatedMap.end() ; ++it1, ++it2)
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
    Vector::iterator it3 = CompareDRPMap.begin();

    for(it1, it2, it3;  it1!=globalHardDRPmap.end() || it2 != globalSoftDRPmap.end() || it3 != CompareDRPMap.end() ; ++it1, ++it2,++it3)
    {
        if((*it1 || *it2) && *it3 == true)
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
    for(int i = 0; i<globalHardDRPmap.size() ; i++)
    {
        GlobalPattern[i] = globalHardDRPmap[i] || globalSoftDRPmap[i];
    }
    
}

bool
DRPmap::IsSpaceInGlobalPattern()
{
    for(int i = 0; i<globalHardDRPmap.size() ; i++)
    {
        if(globalHardDRPmap[i] == false && globalSoftDRPmap[i] == false)
            return true;
    }
    return false;
}
