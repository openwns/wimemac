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
#ifndef WIMEMAC_HELPER_QUEUES_H
#define WIMEMAC_HELPER_QUEUES_H


#include <WNS/service/dll/Address.hpp>

#include <WNS/scheduler/SchedulerTypes.hpp>
//#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <map>
#include <queue>
#include <WIMEMAC/convergence/Upper.hpp>
#include <WIMEMAC/wimemac/helper/IQueueInterface.hpp>





namespace wimemac {namespace helper {

class Queues:
	public IQueueInterface
{
	public:
		Queues(const wns::pyconfig::View& _config, wns::ldk::fun::FUN* fun);
	
		
		virtual
                bool isAccepting(const wns::ldk::CompoundPtr& compound) const;
		
		virtual
                void put(const wns::ldk::CompoundPtr& compound);
 		
// 		virtual
//                 wns::scheduler::UserSet getQueuedUsers() const;
// 		
		virtual
                uint32_t numCompoundsForMacAddress(wns::service::dll::UnicastAddress macaddress) const;
		
		virtual
                uint32_t numBitsForMacAddress(wns::service::dll::UnicastAddress) const;
		
		virtual
                wns::ldk::CompoundPtr getHeadOfLinePDU(wns::service::dll::UnicastAddress macaddress);
		
		virtual
                int getHeadOfLinePDUbits(wns::service::dll::UnicastAddress macaddress);
		
		virtual
                bool hasQueue(wns::service::dll::UnicastAddress macaddress);
		
		virtual
                bool queueHasPDUs(wns::service::dll::UnicastAddress macaddress);

                /*virtual void 
                	setColleagues(wns::scheduler::RegistryProxyInterface* _registry);*/
		virtual
                void setFUN(wns::ldk::fun::FUN* fun);

                /*virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetAllQueues();

                virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetQueues(wns::scheduler::UserID user);

                virtual wns::scheduler::queue::QueueInterface::ProbeOutput 
                	resetQueue(wns::scheduler::ConnectionID cid);*/
		virtual
                std::string printAllQueues();
	private:
		
		struct Friends
		{
			wns::ldk::CommandReaderInterface* keyReader;
		} friends;
		

		struct Queue 
		{
                	Queue(): 
			bits(0)/*,
			macaddress (0)
			user(0)*/
                        {
			}
                        Bit bits;
                        wns::service::dll::UnicastAddress macaddress;
                        //UserID user;
                        std::queue<wns::ldk::CompoundPtr> pduQueue;
                };

                long int maxSize;

                typedef std::map<wns::service::dll::UnicastAddress, Queue> QueueContainer;
                QueueContainer queues;

               

                wns::logger::Logger logger;
                wns::pyconfig::View config;

};



}//helper
}//wimemac


#endif

