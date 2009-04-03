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
#ifndef WIMEMAC_HELPER_IDRPQUEUEINTERFACE_H
#define WIMEMAC_HELPER_IDRPQUEUEINTERFACE_H


#include <WNS/service/dll/Address.hpp>
#include <WNS/simulator/Time.hpp>


namespace wimemac {namespace helper {

class IDRPQueueInterface
{
	public:
	virtual	~IDRPQueueInterface() {};

	
	virtual void
	TimeToTransmit(wns::service::dll::UnicastAddress macaddress, wns::simulator::Time duration) const = 0;
};


}//helper
}//wimemac


#endif
