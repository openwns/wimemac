/*******************************************************************************
 * WiMeMAC                                                                     *
 * __________________________________________________________________________  *
 *                                                                             *
 * Copyright (C) 2009                                                          *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                                *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                   *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                    *
 * email: <hor@comnets.rwth-aachen.de>, www: http://wns.comnets.rwth-aachen.de/*
 ******************************************************************************/


#include <WIMEMAC/DRPmap.hpp>



using namespace wimemac::drp;

DRPmap::DRPmap(int globalMASpSF)
		
{
	globalSoftDRPmap.assign(globalMASpSF,false);
	globalHardDRPmap.assign(globalMASpSF,false);
	
}
void
DRPmap::UpdateHardDRPmap(Vector UpdateHard, wns::logger::Logger _logger)
{
	UpdateDRPmap(UpdateHard, globalHardDRPmap);
}
void
DRPmap::UpdateSoftDRPmap(Vector UpdateSoft, wns::logger::Logger _logger)
{
	logger = _logger;
	UpdateDRPmap(UpdateSoft, globalSoftDRPmap);
	
	
}

void
DRPmap::UpdateDRPmap(Vector UpdateMap, Vector UpdatedMap)
{
	
	Vector::iterator it1 = UpdateMap.begin();
	Vector::iterator it2 = UpdatedMap.begin();
	
	for(it1, it2;  it1!=UpdateMap.end() || it2 != UpdatedMap.end() ; ++it1, ++it2)
	{
		*it2 = *it1 | *it2;
	}
		MESSAGE_SINGLE(NORMAL, logger, "Update DRPMap");
}
