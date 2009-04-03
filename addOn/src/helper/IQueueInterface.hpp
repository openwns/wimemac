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
#ifndef WIMEMAC_HELPER_IQUEUEINTERFACE_H
#define WIMEMAC_HELPER_IQUEUEINTERFACE_H


#include <WNS/service/dll/Address.hpp>
#include <WNS/simulator/Time.hpp>


namespace wimemac {namespace helper {

class IQueueInterface
{
	public:
	virtual	~IQueueInterface() {};
	
	virtual bool 
	isAccepting(const wns::ldk::CompoundPtr& compound) const = 0;

        virtual
        void put(const wns::ldk::CompoundPtr& compound) = 0;
 
// 	virtual 
// 	wns::scheduler::UserSet getQueuedUsers() const  = 0;

	virtual 
	uint32_t numCompoundsForMacAddress(wns::service::dll::UnicastAddress macaddress) const = 0;

	virtual 
	uint32_t numBitsForMacAddress(wns::service::dll::UnicastAddress) const = 0;

	virtual 
	wns::ldk::CompoundPtr getHeadOfLinePDU(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	int getHeadOfLinePDUbits(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	bool hasQueue(wns::service::dll::UnicastAddress macaddress) = 0;

	virtual 
	bool queueHasPDUs(wns::service::dll::UnicastAddress macaddress) = 0;

                /*virtual void 
                	setColleagues(wns::scheduler::RegistryProxyInterface* _registry);*/

	virtual 
	void setFUN(wns::ldk::fun::FUN* fun) = 0;

                /*virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetAllQueues();

                virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetQueues(wns::scheduler::UserID user);

                virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetQueue(wns::scheduler::ConnectionID cid);*/

	virtual 
	std::string printAllQueues() = 0;










};


}//helper
}//wimemac


#endif
