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
#include <WIMEMAC/helper/Queues.hpp>

using namespace wimemac::helper;

Queues::Queues(const wns::pyconfig::View& _config, wns::ldk::fun::FUN* fun):
	maxSize(1000000000),
	logger(_config.get("logger")),
	config(_config)
{
	friends.keyReader = fun->getProxy()->getCommandReader("unicastUpperConvergence");
}

void
Queues::setFUN(wns::ldk::fun::FUN* fun)
{
	MESSAGE_SINGLE(NORMAL, logger, "Queue::setFUN()");
//         assure(fun!=NULL, "fun==NULL");
//         // Obtain commandreader for Priority Info from FUN
//         friends.keyReader = fun->getProxy()->getCommandReader("unicastUpperConvergence");

}

bool
Queues::isAccepting(const wns::ldk::CompoundPtr& compound) const
{
	



	int size = compound->getLengthInBits();
	
	wns::ldk::CommandPool* commandPool = compound->getCommandPool();
	wimemac::convergence::UnicastUpperCommand* unicastcommand =
	friends.keyReader->readCommand<wimemac::convergence::UnicastUpperCommand>(commandPool);
	wns::service::dll::UnicastAddress rx = unicastcommand->peer.targetMACAddress;
	
	if(queues.find(rx) == queues.end())
	{
		MESSAGE_SINGLE(NORMAL, logger, "Compound with size " << size 
		<<" is accepted, a new queue will be created for target "<<rx);
                return true;
        }
	
	if(size + queues.find(rx)->second.bits > maxSize)
	{
		MESSAGE_SINGLE(NORMAL, logger, "Compound with size " << size 
		<<" is not accepted, because queue size is"
		<< queues.find(rx)->second.bits << " for target " << rx);
        
		return false;
	}

        	MESSAGE_SINGLE(NORMAL, logger, "Compound with size " << size 
		<<" accepted, because queue size is"
		<< queues.find(rx)->second.bits << " for target " << rx);
        	
      		  return true;
}

void
Queues::put(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "No valid PDU");
	assure(compound != wns::ldk::CompoundPtr(), "No valid PDU");
	assure(isAccepting(compound), "sendData() has been called without isAccepting()");
	
	wns::ldk::CommandPool* commandPool = compound->getCommandPool();
	wimemac::convergence::UnicastUpperCommand* unicastcommand =
	friends.keyReader->readCommand<wimemac::convergence::UnicastUpperCommand>(commandPool);
	wns::service::dll::UnicastAddress rx = unicastcommand->peer.targetMACAddress;


	MESSAGE_SINGLE(NORMAL, logger, "Compound for " << rx );
		
        
		(queues[rx].pduQueue).push(compound);
		queues[rx].bits += compound->getLengthInBits();
	
}


uint32_t 
Queues::numCompoundsForMacAddress(wns::service::dll::UnicastAddress rx) const
{
	std::map<wns::service::dll::UnicastAddress, Queue>::const_iterator iter = queues.find(rx);
	assure(iter != queues.end(), "cannot find queue for mac=" << rx);
	return iter->second.pduQueue.size();
}

uint32_t 
Queues::numBitsForMacAddress(wns::service::dll::UnicastAddress rx) const
{
	std::map<wns::service::dll::UnicastAddress, Queue>::const_iterator iter = queues.find(rx);
	assure(iter != queues.end(), "cannot find queue for mac=" << rx);
	return iter->second.bits;	
}

wns::ldk::CompoundPtr 
Queues::getHeadOfLinePDU(wns::service::dll::UnicastAddress rx)
{
	assure(queueHasPDUs(rx), "getHeadOfLinePDU called for mac without PDUs or on existing mac");
	
	wns::ldk::CompoundPtr pdu = queues[rx].pduQueue.front();
        queues[rx].pduQueue.pop();
        queues[rx].bits -= pdu->getLengthInBits();
     	
        return pdu;
}

int
Queues::getHeadOfLinePDUbits(wns::service::dll::UnicastAddress rx)
{
        assure(queueHasPDUs(rx), "getHeadOfLinePDUbits called for mac without PDUs or non-existent mac");
        return queues[rx].pduQueue.front()->getLengthInBits();
}

bool
Queues::hasQueue(wns::service::dll::UnicastAddress rx)
{
        return queues.find(rx) != queues.end();
}

bool
Queues::queueHasPDUs(wns::service::dll::UnicastAddress rx) 
{
        if (queues.find(rx) == queues.end())
                return false;
        return (queues[rx].pduQueue.size() != 0);
}


std::string
Queues::printAllQueues()
{
        std::stringstream s;
        for (std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter = queues.begin();
                 iter != queues.end(); ++iter)
        {
                wns::service::dll::UnicastAddress rx = iter->first;
                int bits      = iter->second.bits;
                int compounds = iter->second.pduQueue.size();
                s << rx << ":" << bits << "," << compounds << " ";
        }
        return s.str();
}

























