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

#ifndef WIMEMAC_CONVERGENCE_IRXSTARTEND_HPP
#define WIMEMAC_CONVERGENCE_IRXSTARTEND_HPP

#include <WNS/Subject.hpp>
#include <WNS/simulator/Time.hpp>

namespace wimemac { namespace convergence {

        class IRxStartEnd
        {
        public:
            virtual ~IRxStartEnd()
                {}

            virtual void onRxStart(const wns::simulator::Time expRxDuration) = 0;
            virtual void onRxEnd() = 0;
            virtual void onRxError() = 0;
        };

        /**
         * @brief Notification of the Start and End of a reception
         */
        class RxStartEndNotification :
        virtual public wns::Subject<IRxStartEnd>
        {
        public:
            // @brief functor for IRxStartEnd::rxStartIndicationon calls
            struct OnRxStartEnd
            {
                OnRxStartEnd(const wns::simulator::Time _expRxDuration, const bool _isStart, const bool _rxError):
                    expRxDuration(_expRxDuration),
                    isStart(_isStart),
                    rxError(_rxError)
                    {}

                void operator()(IRxStartEnd* rx)
                    {
                        if(isStart)
                        {
                            rx->onRxStart(expRxDuration);
                        }
                        else
                        {
                            if(rxError)
                            {
                                rx->onRxError();
                            }
                            else
                            {
                                rx->onRxEnd();
                            }
                        }
                    }
            private:
                wns::simulator::Time expRxDuration;
                bool isStart;
                bool rxError;
            };
        };
    }
}

#endif // not defined WIMEMAC_CONVERGENCE_IRXSTARTEND_HPP
