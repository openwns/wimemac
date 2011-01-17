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

#include <WIMEMAC/drp/TempSendBuffer.hpp>
#include <WIMEMAC/drp/DRPScheduler.hpp>
using namespace wimemac::drp;


TempSendBuffer::TempSendBuffer(helper::Queues* _DRPQueues, wns::service::dll::UnicastAddress _target, DRPScheduler* _drpscheduler, wns::logger::Logger _logger):
    logger(_logger),
    DRPQueues(_DRPQueues),

    TxAckDuration(13.125E-6),
    SIFS(10E-6),
    GuardDuration(12E-6)
{
    drpscheduler = _drpscheduler;
    tempBuffer.bits = 0;
    tempBuffer.target = _target;
    tempBuffer.timeleft = 0;
    isWaitingSIFS = false;
    isTransmissionStarted = false;

    numOfRetransmissions = 0;
    numTotalRetransmissions = 0;
    sentCompoundsInLastTxOP = 0;
    successfullCompoundsInLastTxOP = 0;
    sentCompoundsInLastBufferAction = 0;
    successfullCompoundsInLastBufferAction = 0;
}

void
TempSendBuffer::SetTxopDuration(wns::simulator::Time _duration)
{

    txOPStartTime = wns::simulator::getEventScheduler()->getTime();
    txOPTime = _duration - GuardDuration;
    firstMASOfTxOP = drpscheduler->getManager()->getMASNumber(txOPStartTime);

    if(!IsBufferEmpty())
    {
        MESSAGE_SINGLE(NORMAL, logger, "Buffer is not empty, there are still " << numCompounds() << " compounds with total size of " << tempBuffer.bits << " bits in the buffer.");

    }
    tempBuffer.timeleft = txOPTime - txBufferTime();
    assure(txOPTime > 0, "No valid TxOP");

    MESSAGE_SINGLE(NORMAL, logger, "SetTxOP : Sent compounds during the last TXOP : " << sentCompoundsInLastTxOP << " | Successfull compounds : " << successfullCompoundsInLastTxOP);

    if(sentCompoundsInLastBufferAction > 0)
    {
        sentCompoundsInLastTxOP = sentCompoundsInLastBufferAction;
        successfullCompoundsInLastTxOP = successfullCompoundsInLastBufferAction;

        sentCompoundsInLastBufferAction = 0;
        successfullCompoundsInLastBufferAction = 0;
    }
}

int
TempSendBuffer::GetNumOfTotalRetransmissions()
{
    return numTotalRetransmissions;
}

int
TempSendBuffer::GetNumOfRetransmissions(const wns::ldk::CompoundPtr& compound)
{
    assure(tempBuffer.pduQueue.front()->getData() == compound->getData(),"The compound that was asked for does not equal the first compound in the tempBuffer");
    return numOfRetransmissions;
}

int
TempSendBuffer::GetNumOfRetransmissions()
{
    return numOfRetransmissions;
}

wns::service::dll::UnicastAddress
TempSendBuffer::GetTarget()
{
    return tempBuffer.target;
}

bool
TempSendBuffer::StartBuffering()
{
    if(!hasTimeoutSet() && FillBuffer() == false)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Buffer : Cannot fill the buffer!" );
        return false;
    }
    while(FillBuffer() == true);
    MESSAGE_SINGLE(NORMAL, logger, "Buffer is filled size: " << tempBuffer.pduQueue.size() << " Start sending !" );
    if(!hasTimeoutSet())
        SendCompounds();
    else MESSAGE_SINGLE(NORMAL, logger, "Buffer : TimeOut already set -> wait until timeout!");
    return true;
}
void
TempSendBuffer::StopBuffering()
{
    isTransmissionStarted = false;

    if(hasTimeoutSet())
    {
        cancelTimeout();
        onTimeout();
    }

    // Remove compounds from TempBuffer after the TxOP is over
    while(IsBufferEmpty() == false)
    {
        DRPQueues->putFront(tempBuffer.pduQueue.back() , tempBuffer.target);

        wns::simulator::Time duration_ = CalculateTxTime(tempBuffer.pduQueue.back()->getLengthInBits());
        tempBuffer.timeleft += duration_;
        tempBuffer.bits -= tempBuffer.pduQueue.back()->getLengthInBits();
        tempBuffer.pduQueue.pop_back();
    }

    txOPTime = wns::simulator::getEventScheduler()->getTime() - txOPStartTime;
}

bool
TempSendBuffer::FillBuffer()
{
    wns::simulator::Time duration_ = 0;

    if(txBufferTime() <= txOPTime)
    {
        // The current fill level of the buffer is below the TX opportunity -> fill the buffer
        int size = GetCompoundSize();

        if(size > 0)
        {
            if(CalculateTxTime(size, duration_))
            {
                MESSAGE_SINGLE(NORMAL, logger, "Buffer calculates duration: " << duration_);

                if(DRPQueues->hasQueue(tempBuffer.target) && DRPQueues->queueHasPDUs(tempBuffer.target))
                {
                    wns::ldk::CompoundPtr compound = DRPQueues->getHeadOfLinePDU(tempBuffer.target);
                    tempBuffer.pduQueue.push_back(compound);
                    tempBuffer.timeleft -= duration_;
                    tempBuffer.bits += compound->getLengthInBits();
                    assure(tempBuffer.timeleft >= 0, "Buffer error, time < 0");
                    MESSAGE_SINGLE(NORMAL, logger, "Buffer calculates time: " << tempBuffer.timeleft);

                    return true;
                }

            }
            MESSAGE_SINGLE(NORMAL, logger, "Intermediate Buffer: compound doesn't fit");
        }
        else if(size == 0)
            MESSAGE_SINGLE(NORMAL, logger, "Intermediate Buffer: no more compounds");

        return false;
    }

    else
    {
        // The current fill level of the buffer is above the TX OP -> put compounds back into the Queue
        DRPQueues->putFront(tempBuffer.pduQueue.back() , tempBuffer.target);

        wns::simulator::Time duration_ = CalculateTxTime(tempBuffer.pduQueue.back()->getLengthInBits());
        tempBuffer.timeleft += duration_;
        tempBuffer.bits -= tempBuffer.pduQueue.back()->getLengthInBits();
        tempBuffer.pduQueue.pop_back();

        MESSAGE_SINGLE(NORMAL, logger, "The buffer fill level is " << txBufferTime() << " and therefore above the TXopTime of " << txOPTime << ". Removing last compound from the intermediate buffer!");

        return true;
    }

}

int
TempSendBuffer::CalculateMaxSize()
{

}

int
TempSendBuffer::GetCompoundSize()
{
    int size = 0;

    if(DRPQueues->hasQueue(tempBuffer.target) && DRPQueues->queueHasPDUs(tempBuffer.target) )
        size = DRPQueues->getHeadOfLinePDUbits(tempBuffer.target);

    MESSAGE_SINGLE(NORMAL, logger, "Buffer asked for compound size: " << size);
    return size;
}

Bit
TempSendBuffer::getNextCompoundSize()
{
    if(!(numCompounds() > 1)) return 0;
    else
    {
        return tempBuffer.pduQueue[1]->getLengthInBits();
    }
}

Bit
TempSendBuffer::getCurrentCompoundSize()
{
    if(!(numCompounds() > 0)) return 0;
    else
    {
        return tempBuffer.pduQueue[0]->getLengthInBits();
    }
}

bool
TempSendBuffer::CalculateTxTime(int size, wns::simulator::Time &duration_)
{
    wns::simulator::Time TxDuration = drpscheduler->getManager()->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)size, drpscheduler->getPhyMode(tempBuffer.target, firstMASOfTxOP));

    MESSAGE_SINGLE(NORMAL, logger, "Intermediate Buffer compound time: " << TxDuration);

    TxDuration += TxAckDuration + 2*SIFS;
    MESSAGE_SINGLE(NORMAL, logger, "Intermediate Buffer FT time: " << TxDuration);
    if(TxDuration > tempBuffer.timeleft)
    {
        MESSAGE_SINGLE(NORMAL, logger, "Intermediate Buffer compound doesn't fit: " << TxDuration);
        return false;
    }

    duration_ = TxDuration;

    return true;
}


wns::simulator::Time
TempSendBuffer::CalculateTxTime(int size)
{
    wns::simulator::Time TxDuration = drpscheduler->getManager()->getProtocolCalculator()->getDuration()->MSDU_PPDU((Bit)size, drpscheduler->getPhyMode(tempBuffer.target, firstMASOfTxOP)); // TODO manager->getMASNumber(now) for PCA transmissions that happen in MASs of different set PhyMode
    TxDuration += TxAckDuration + 2*SIFS;
    return TxDuration;
}


bool
TempSendBuffer::IsBufferEmpty() const
{
    return (tempBuffer.pduQueue.empty());
}

void
TempSendBuffer::SendCompounds()
{

    if (!IsBufferEmpty())
    {
        wns::simulator::Time FTDuration =
        CalculateTxTime(tempBuffer.pduQueue.front()->getLengthInBits());
        if( (wns::simulator::getEventScheduler()->getTime() + FTDuration) > (txOPStartTime + txOPTime) )
        {
            MESSAGE_SINGLE(NORMAL, logger, "There is not enough time left to send the next packet");
            drpscheduler->stopPCAtransmission();
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "Buffer is filled or not empty, start sending compounds: ");
            drpscheduler->SendCompounds(tempBuffer.target);
        }
    }

    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "The temporary Buffer is empty");
        drpscheduler->stopPCAtransmission();
    }
}

wns::ldk::CompoundPtr
TempSendBuffer::GetCompound()
{
    return (tempBuffer.pduQueue.front());
}

void
TempSendBuffer::TransmitCompound(const wns::ldk::CompoundPtr& compound)
{
    assure(compound == tempBuffer.pduQueue.front(), "Transmitted compound does not equal the first compound in queue");

    wns::simulator::Time FTDuration =
    CalculateTxTime(tempBuffer.pduQueue.front()->getLengthInBits());
    setTimeout(FTDuration);

    sentCompoundsInLastBufferAction += 1; //DEBUG
    isTransmissionStarted = true;
}

void
TempSendBuffer::onTimeout()
{
    if (isWaitingSIFS)
    {
        isWaitingSIFS = false;
        MESSAGE_SINGLE(NORMAL, logger, "Buffer: waited SIFS after ACK. Proceed with next transmission! ");
        // The acknowledged compound is already deleted from the queue
    }
    else
    {
        // Inform drpscheduler about failed frame for FER evaluation
        drpscheduler->failedAck(tempBuffer.target);

        if(drpscheduler->getIsDroppingAfterRetr() == -1)
        {
            // Infinite retransmissions
            MESSAGE_SINGLE(NORMAL, logger, "Buffer: compound was not acknowledged after " << numOfRetransmissions +1 << " transmissions, retransmit! ");
            numTotalRetransmissions++;
            numOfRetransmissions++;
        }
        else if(drpscheduler->getIsDroppingAfterRetr() == 0)
        {
            // No retransmissions allowed -> drop compound from head of queue
            MESSAGE_SINGLE(NORMAL, logger, "Buffer: compound was not acknowledged, compound lost! ");
            tempBuffer.bits -= tempBuffer.pduQueue.front()->getLengthInBits();
            tempBuffer.pduQueue.pop_front();
            numOfRetransmissions = 0;
        }
        else
        {
            // x retransmissions allowed;
            if (numOfRetransmissions >= drpscheduler->getIsDroppingAfterRetr())
            {
                // Maximum number of retransmissions exceeded -> drop
                MESSAGE_SINGLE(NORMAL, logger, "Buffer: compound was not acknowledged and maximum number of retransmissions of " << drpscheduler->getIsDroppingAfterRetr() << " are used! Dropping compound! ");
                tempBuffer.bits -= tempBuffer.pduQueue.front()->getLengthInBits();
                tempBuffer.pduQueue.pop_front();
                numOfRetransmissions = 0;
            }
            else
            {
                // The unacknowledged compound is still on top of the queue
                numTotalRetransmissions++;
                numOfRetransmissions++;
                MESSAGE_SINGLE(NORMAL, logger, "Buffer: compound was not acknowledged, retransmit " << numOfRetransmissions << " of " << drpscheduler->getIsDroppingAfterRetr());
            }
        }
    }

    if(isTransmissionStarted) SendCompounds();
    else
    {
        MESSAGE_SINGLE(NORMAL, logger, "Buffer: Transmission was stopped! Proceed in next TxOP");
    }
}

void
TempSendBuffer::ImmAck()
{
    MESSAGE_SINGLE(NORMAL, logger, "Buffer: ARQ informs about an ImmAck! ");

    if(hasTimeoutSet())
        cancelTimeout();
    else assure(false, "Buffer: Received ACK, but no TimeOut is set. Transmission already stopped?");
    tempBuffer.bits -= tempBuffer.pduQueue.front()->getLengthInBits();
    tempBuffer.pduQueue.pop_front();
    // wait another SIFS

    successfullCompoundsInLastBufferAction += 1; //DEBUG
    numOfRetransmissions = 0;

    isWaitingSIFS = true;
    setTimeout(SIFS);
}

void
TempSendBuffer::NewArrival()
{
    MESSAGE_SINGLE(NORMAL, logger, "Refill Buffer");
    if(StartBuffering());
}

int
TempSendBuffer::numCompounds()
{
    return(tempBuffer.pduQueue.size());
}

wns::simulator::Time
TempSendBuffer::txBufferTime()
{
    if (!IsBufferEmpty())
    {
        // If Imm-Ack is on
        wns::simulator::Time totalTime_ = 0;
        for (int i = 0; i < tempBuffer.pduQueue.size(); i++)
            totalTime_ += CalculateTxTime(tempBuffer.pduQueue[i]->getLengthInBits());

        return totalTime_;
    }
    else return 0;
}
