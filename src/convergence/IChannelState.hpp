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

#ifndef WIMEMAC_CONVERGENCE_ICHANNELSTATE_HPP
#define WIMEMAC_CONVERGENCE_ICHANNELSTATE_HPP

#include <WNS/Subject.hpp>

namespace wimemac { namespace convergence {

    // The indicator for the channel state
    enum CS {
        idle,
        busy
    };

    /** @brief Notification of the channel state (busy/idle) */
    class IChannelState
    {
    public:
        virtual ~IChannelState(){};

        virtual void onChannelBusy() = 0;
        virtual void onChannelIdle() = 0;
    };

    class ChannelStateNotification :
        virtual public wns::Subject<IChannelState>
    {
    public:
        // @brief functor for IChannelState::onChangedCS calls
        struct OnChangedCS
        {
            OnChangedCS(const CS _newCS):
                newCS(_newCS)
                {}

            void operator()(IChannelState* cs)
                {
                    // The functor calls the onChannelBusy/onChannelIdle implemented by the
                    // Observer
                    if (newCS == idle)
                        cs->onChannelIdle();
                    else
                        cs->onChannelBusy();
                }
        private:
            CS newCS;
        };

    };


}
}

#endif // not defined WIMEMAC_CONVERGENCE_ICHANNELSTATE_HPP
