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

#ifndef WIMEMAC_PHYUSER_COMMAND
#define WIMEMAC_PHYUSER_COMMAND

#include <functional>

#include <WIMEMAC/convergence/OFDMAAccessFunc.hpp>

#include <WNS/ldk/Command.hpp>

namespace wimemac { namespace convergence {
    class PhyUser;

    class PhyUserCommand :
        public wns::ldk::Command
    {
    public:
        struct {
            wns::Power rxPower;
            wns::Power interference;

            std::auto_ptr<OFDMAAccessFunc> pAFunc;

        } local;

        struct {} peer;

        struct {} magic;

        PhyUserCommand()
        {
        }

        // copy operator
        PhyUserCommand(const PhyUserCommand& other) :
            wns::ldk::Command()
        {
            local.rxPower            = other.local.rxPower;
            local.interference       = other.local.interference;

            if (other.local.pAFunc.get())
                local.pAFunc.reset(dynamic_cast<wimemac::convergence::OFDMAAccessFunc*>(other.local.pAFunc->clone()));
        }
    };
} // convergence
} // wimemac

#endif // WIMEMAC_PHYUSER_COMMAND

