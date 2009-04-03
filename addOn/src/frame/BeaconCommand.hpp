/******************************************************************************
 * WiMeMAC                                                                    *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2009                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                   *
 * email: hor@comnets.de, www: http://wns.comnets.rwth-aachen.de/             *
 *****************************************************************************/
#ifndef WIMEMAC_FRAME_BEACONCOMMAND_H
#define WIMEMAC_FRAME_BEACONCOMMAND_H
#include <WNS/ldk/Command.hpp>
#include <WNS/service/dll/Address.hpp>
#include <vector>

namespace wimemac {
namespace frame {
typedef std::vector<bool> Vector;

	class BeaconCommand:
		public wns::ldk::Command
	{
		public:
			
		
			BeaconCommand()
			{
				peer.reservationtype = Hard;
				peer.reasoncode = Accept;
				
				//peer.address = 0;
				peer.DRPAlloc.assign (256,true);
				
				for(int i = 0; i<10; i++)
				{
					peer.DRPAlloc[i] = false;
				}
				
			}
			
						
			
			enum ReservationType {
				Hard, Soft
			}; 

			enum ReasonCode{
				Accept, Conflict, Pending, Denied
			};
		
			enum DeviceType {
				Target, Owner
			};
			
			struct{} local;

			struct{
				enum ReservationType reservationtype;
				enum ReasonCode reasoncode;
				enum DeviceType devicetype;
				wns::service::dll::UnicastAddress address;
				wns::service::dll::UnicastAddress drpaddress;
				Vector DRPAlloc;				
								
			} peer;

							
			struct{}magic;
		
		private:
			
			
		
		
	};
}

}
#endif
