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

#ifndef WIMEMAC_FRAMETYPE_HPP
#define WIMEMAC_FRAMETYPE_HPP

#include <WNS/ldk/Command.hpp>

namespace wimemac {


    enum CompoundType {
        //Standard and Burst through Phymode.PreambleMode
        DATA_PREAMBLE,
        BEACON_PREAMBLE,
        ACK_PREAMBLE,
        PAYLOAD,
        DATA,
        BEACON,
        ACK,
        DATA_TXOP
    };

    class IKnowsFrameTypeCommand :
        public wns::ldk::Command
    {
    public:
        virtual CompoundType getCompoundType() = 0;
        virtual bool isPreamble() const = 0;
    };
}

#endif
