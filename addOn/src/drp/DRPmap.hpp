/*********************************************************************************
 * WiMeMAC                                                                       *
 * __________________________________________________________________________    *
 *                                                                               *
 * Copyright (C) 2009 by Holger Rosier                                           *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                    *
 * phone: ++49-241-80-20547 (phone), fax: ++49-241-80-22242                      *
 * email: <hor@comnets.rwth-aachen.de>, www: http://www.comnets.rwth-aachen.de     *
 ********************************************************************************/

#ifndef WIMEMAC_DRPmap_H
#define WIMEMAC_DRPmap_H
#include <WNS/logger/Logger.hpp>
#include <vector>


namespace wimemac {
namespace drp {
typedef std::vector<bool> Vector;

	class DRPmap
		{
		public:
			DRPmap(int gMASpSF);
			void UpdateHardDRPmap(Vector, wns::logger::Logger _logger);
			void UpdateSoftDRPmap(Vector, wns::logger::Logger _logger);
			void UpdateDRPmap(Vector UpdateMap, Vector UpdatedMap);
			
			
						
		private:
			Vector globalHardDRPmap;
			Vector globalSoftDRPmap;
			wns::logger::Logger logger;
					
		};
}
}
#endif

