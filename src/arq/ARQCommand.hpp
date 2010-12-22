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
#ifndef WIMEMAC_ARQ_ARQCOMMAND_H
#define WIMEMAC_ARQ_ARQCOMMAND_H
#include <WNS/ldk/Command.hpp>
#include <WNS/service/dll/Address.hpp>

namespace wimemac {
namespace arq {

	class ARQCommand:
		public wns::ldk::Command
	{
		public:
			
		
			ARQCommand()
			{
				peer.ACKpolicy = NoACK;
				peer.Frametype = Data;
			}
			
						
			
			enum ACKPolicy {
				NoACK, ImmACK, BACK
			}; 
			
			enum FrameType {
				Data, Ack
			};

			struct{} local;

			struct{
				enum ACKPolicy ACKpolicy;
				enum FrameType Frametype;
			} peer;
			

							
			struct{}magic;
		
		private:
			
			
		
		
	};
}

}
#endif
