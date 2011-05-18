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
 
#include <WIMEMAC/helper/Queues.hpp>
#include <DLL/UpperConvergence.hpp>

using namespace wimemac::helper;

Queues::Queues(const wns::pyconfig::View& _config, wns::ldk::fun::FUN* fun):
    maxSize(_config.get<long int>("queuesize")),
    FrameDuration(_config.get<wns::simulator::Time>("frameduration")),
    logger(_config.get("logger")),
    config(_config)
{
    friends.keyReader = fun->getProxy()->getCommandReader("upperConvergence");

    scheduler = wns::simulator::getEventScheduler();
    deleteQueues = _config.get<bool>("deleteQueues");
    nextPCAReceiver = wns::service::dll::UnicastAddress();
}

bool
Queues::isAccepting(const wns::ldk::CompoundPtr& compound) const
{
    int size = compound->getLengthInBits();

    wns::ldk::CommandPool* commandPool = compound->getCommandPool();
    dll::UpperCommand* unicastcommand =
    friends.keyReader->readCommand<dll::UpperCommand>(commandPool);
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

//         MESSAGE_SINGLE(NORMAL, logger, "Compound with size " << size
//             <<" accepted, because queue size is"
//             << queues.find(rx)->second.bits << " for target " << rx);

        return true;
}

void
Queues::put(const wns::ldk::CompoundPtr& compound)
{
    assure(compound, "No valid PDU");
    assure(compound != wns::ldk::CompoundPtr(), "No valid PDU");
    assure(isAccepting(compound), "sendData() has been called without isAccepting()");

    wns::ldk::CommandPool* commandPool = compound->getCommandPool();
    dll::UpperCommand* unicastcommand =
    friends.keyReader->readCommand<dll::UpperCommand>(commandPool);
    wns::service::dll::UnicastAddress rx = unicastcommand->peer.targetMACAddress;


    //MESSAGE_SINGLE(NORMAL, logger, "Compound for " << rx );

    // If this is a new queue initialize RemoveCompounds variable with config value
    if (queues.find(rx) == queues.end()) queues[rx].RemoveCompounds = deleteQueues;

    (queues[rx].pduQueue).push_back(compound);
    queues[rx].bits += compound->getLengthInBits();
}

void
Queues::putFront(const wns::ldk::CompoundPtr& compound, wns::service::dll::UnicastAddress rx)
{
    MESSAGE_SINGLE(NORMAL, logger, "Compound for " << rx << " is being re-inserted to the front of the queue");

    (queues[rx].pduQueue).push_back(compound);
    queues[rx].bits += compound->getLengthInBits();
}

wns::ldk::CompoundPtr
Queues::getHeadOfLinePDU(wns::service::dll::UnicastAddress rx)
{
    assure(queueHasPDUs(rx), "getHeadOfLinePDU called for mac without PDUs or on existing mac");

    wns::ldk::CompoundPtr pdu = queues[rx].pduQueue.front();
    queues[rx].pduQueue.pop_front();
    queues[rx].bits -= pdu->getLengthInBits();

    return pdu;
}

int
Queues::getHeadOfLinePDUbits(wns::service::dll::UnicastAddress rx)
{
        assure(queueHasPDUs(rx), "getHeadOfLinePDUbits called for mac without PDUs or non-existent mac " << rx);
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
    else
    {
        if(queues[rx].RemoveCompounds == true)
        {
            RemoveCompounds(rx);
            queues[rx].RemoveCompounds = false;
            MESSAGE_SINGLE(NORMAL, logger, "Deleting Queue on startUp");
            return false;
        }
        return (queues[rx].pduQueue.size() != 0);
    }
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

void
Queues::RemoveCompounds(wns::service::dll::UnicastAddress rx)
{
    while (!queues[rx].pduQueue.empty())
    {
        queues[rx].pduQueue.pop_front();
    }

    queues[rx].bits = 0;
    MESSAGE_SINGLE(NORMAL, logger, "Clear queue for target: " << rx
        <<" Queue size now: " << queues[rx].pduQueue.size());
}

void
Queues::resetQueue(wns::service::dll::UnicastAddress rx)
{
    queues[rx].RemoveCompounds = deleteQueues;   
}

wns::service::dll::UnicastAddress
Queues::getNextPCAReceiver()
{
    // If nextPCAReceiver was not initialized yet, but there is a Queue, set it to the first Queue storing compounds
    if(nextPCAReceiver == wns::service::dll::UnicastAddress() && queues.size() > 0)
    {
        for (std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter = queues.begin();
                 iter != queues.end(); ++iter)
        {
            if(iter->second.pduQueue.size() > 0)
            {
                nextPCAReceiver = iter->first;
                break;
            }
        }
    }

    if(queues[nextPCAReceiver].pduQueue.size() == 0)
    {
        MESSAGE_SINGLE(NORMAL, logger, "The last set PCA Receiver has no compounds in Queue due to DRP transmissions");
        changePCAreceiver();
    }

    return nextPCAReceiver;
}

void
Queues::changePCAreceiver()
{
    bool foundNewPCAreceiver = false;
    wns::service::dll::UnicastAddress oldPCAreceiver = nextPCAReceiver;
    std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter_oldPCArec;
    for (std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter = queues.begin();
             iter != queues.end(); ++iter)
    {
        if(iter->first == oldPCAreceiver)
        {
            iter_oldPCArec = iter;
            break;
        }
    }

    for (std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter = ++iter_oldPCArec;
             iter != queues.end(); ++iter)
    {
        if(iter->second.pduQueue.size() > 0)
        {
            nextPCAReceiver = iter->first;
            foundNewPCAreceiver = true;
            break;
        }
    }

    if(!foundNewPCAreceiver)
    {
        for (std::map<wns::service::dll::UnicastAddress, Queue>::iterator iter = queues.begin();
             iter != queues.end(); ++iter)
        {
            if(iter->second.pduQueue.size() > 0)
            {
                nextPCAReceiver = iter->first;
                foundNewPCAreceiver = true;
                break;
            }
        }
    }

    if(!foundNewPCAreceiver) nextPCAReceiver = wns::service::dll::UnicastAddress();
    MESSAGE_SINGLE(NORMAL, logger, "Next PCA Receiver will be " << nextPCAReceiver << " with " << queues[nextPCAReceiver].pduQueue.size() << " waiting compounds");
}
