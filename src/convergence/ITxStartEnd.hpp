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

#ifndef WIMEMAC_CONVERGENCE_ITXSTARTEND_HPP
#define WIMEMAC_CONVERGENCE_ITXSTARTEND_HPP

#include <WNS/Subject.hpp>
#include <WNS/ldk/Compound.hpp>

namespace wimemac { namespace convergence {

    enum TxStartEndState {
        start,
        end
    };

    /**
     * @brief Notification of transmission start and end
     *
     * A lower convergence FU notifies for every MPDU the start and the end of
     * the transmission.
     */
    class ITxStartEnd
    {
    public:
        virtual ~ITxStartEnd()
            {}

        /** @brief Called when the transmission starts */
        virtual void onTxStart(const wns::ldk::CompoundPtr& compound) = 0;
        /** @brief Called when the transmission ends */
        virtual void onTxEnd(const wns::ldk::CompoundPtr& compound) = 0;
    };

	class TxStartEndNotification :
		virtual public wns::Subject<ITxStartEnd>
	{
	public:

		struct OnTxStartEnd
		{
			OnTxStartEnd(const wns::ldk::CompoundPtr& _compound, const TxStartEndState _state):
				compound(_compound),
                state(_state)
				{}

			void operator()(ITxStartEnd* tx)
				{
                    assure(compound, "compound is NULL");

                    if(state == start)
                    {
                        // The functor calls the txStartIndication implemented by the
                        // Observer
                        tx->onTxStart(compound);
                    }
                    else
                    {
                        // The functor calls the txEndIndication implemented by the
                        // Observer
                        tx->onTxEnd(compound);
                    }
				}
        private:
            wns::ldk::CompoundPtr compound;
            TxStartEndState state;
		};
	};
}
}

#endif // not defined WIMEMAC_SCHEDULER_TXSTARTENDINTERFACE_HPP
