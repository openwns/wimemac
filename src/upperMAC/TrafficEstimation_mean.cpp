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

#include <WIMEMAC/upperMAC/TrafficEstimation_mean.hpp>

using namespace wimemac::upperMAC;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::upperMAC::TrafficEstimation_mean,
    wns::ldk::FunctionalUnit,
    "wimemac.upperMAC.TrafficEstimation_mean",
    wns::ldk::FUNConfigCreator);


TrafficEstimation_mean::TrafficEstimation_mean(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
    wns::ldk::fu::Plain<TrafficEstimation_mean, wns::ldk::EmptyCommand>(fun),
    managerName(config.get<std::string>("managerName")),
    logger(config.get("logger")),
    averageOverSFs(config.get<int>("averageOverSFs")),
    datathroughputProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()), "wimemac.traffic.incoming.throughput")),
    bitsPerSF(0)
{
    trafficEstimationConfig.CompoundspSF = config.get<int>("CompoundspSF");
    trafficEstimationConfig.BitspSF = config.get<int>("BitspSF");
    trafficEstimationConfig.MaxCompoundSize = config.get<int>("MaxCompoundSize");
    trafficEstimationConfig.overWriteEstimation = config.get<bool>("overWriteEstimation");

    friends.keyReader = fun->getProxy()->getCommandReader("upperConvergence");
    MESSAGE_SINGLE(NORMAL, this->logger, "Created instance of TrafficEstimation_mean");

    measuringSince = wns::simulator::getEventScheduler()->getTime();
}


TrafficEstimation_mean::~TrafficEstimation_mean()
{

}

void
TrafficEstimation_mean::onFUNCreated()
{
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::IManagerServices*>(managerName);
    scheduler = wns::simulator::getEventScheduler();

    wns::simulator::Time SFDuration = 65.536E-3;
    startPeriodicTimeout(SFDuration, SFDuration);
}

bool
TrafficEstimation_mean::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
    // Accept everytime to estimate the traffic correctly
    return true;
}


void
TrafficEstimation_mean::doSendData(const wns::ldk::CompoundPtr& compound)
{
    wns::ldk::CommandPool* commandPool = compound->getCommandPool();
    dll::UpperCommand* unicastcommand =
    friends.keyReader->readCommand<dll::UpperCommand>(commandPool);
    wns::service::dll::UnicastAddress currentAddress = unicastcommand->peer.targetMACAddress;

    if(friends.manager->getDRPchannelAccess()) // If DRP channel access is off don't estimate
    {
        if (windowedTrafficPerAddress.find(currentAddress) == windowedTrafficPerAddress.end())//current address is a new address
        {
            //wns::events::scheduler::Callable call = boost::bind(&TrafficEstimation_mean::QueueEval, this, currentAddress);
            //wns::simulator::Time FrameDuration = 65.536E-3;
            //scheduler->scheduleDelay(call, 10*FrameDuration);
            
            MESSAGE_SINGLE(NORMAL, this->logger, "Beginning traffic measurement for " << averageOverSFs << " frames for address " << currentAddress);

            //Initialization of the measurement variables
            measurementDatapSF currentData;
            currentData.sentCompounds = 1;
            currentData.maxCompoundSize = compound->getLengthInBits();
            currentData.bitsTotal = compound->getLengthInBits();

            windowedTrafficPerAddress[currentAddress].push_back(currentData);
        }
        else    //current address is already in addressList
        {
            //adjustment of the measurement variables
            int size = compound->getLengthInBits();
            (windowedTrafficPerAddress[currentAddress].back()).sentCompounds++;
            (windowedTrafficPerAddress[currentAddress].back()).bitsTotal += size;
            if(size > (windowedTrafficPerAddress[currentAddress].back()).maxCompoundSize)
                (windowedTrafficPerAddress[currentAddress].back()).maxCompoundSize = size;
                
            //MESSAGE_SINGLE(NORMAL, this->logger, "Address " << currentAddress << "; sent compounds: " << addressList.find(currentAddress)->second.sentCompounds << "; biggest compound: " << addressList.find(currentAddress)->second.maxCompoundSize);
        }
    }

    if(this->getConnector()->hasAcceptor(compound))
    {
        this->getConnector()->getAcceptor(compound)->sendData(compound);
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, this->logger, "Dropping compound. Receiver is not ready");
    }
}

void
TrafficEstimation_mean::doWakeup()
{
    // simply forward the wakeup call
    this->getReceptor()->wakeup();
}

void
TrafficEstimation_mean::periodically()
{
    // Put traffic received in last SF into probe
    wns::simulator::Time now_ = wns::simulator::getEventScheduler()->getTime();

    datathroughputProbe->put( bitsPerSF / (now_ - measuringSince) );
    bitsPerSF = 0;
    measuringSince = now_;
    
    // Evaluate Traffic each SF
    std::map<wns::service::dll::UnicastAddress, std::deque<measurementDatapSF> >::iterator it;
    for(it = windowedTrafficPerAddress.begin(); it != windowedTrafficPerAddress.end() ;++it)
    {
        wns::service::dll::UnicastAddress rx = (*it).first;
        
        // initialize with false if key does not yet exist
        if(NeedsNewEstimatePerAddress.find(rx) == NeedsNewEstimatePerAddress.end()) NeedsNewEstimatePerAddress[rx] = false;
        
        // There is no estimation done yet or a new estimation needs to be done
        if((lastSetTrafficPerAddress.find(rx) == lastSetTrafficPerAddress.end()) ||
            (NeedsNewEstimatePerAddress[rx] == true))
        {
            // Delete the first evaluation since the traffic may have started other than at the beginning of the SF
            if(NeedsNewEstimatePerAddress[rx] == false)
            {
                // This means this is the first evaluation for the specified address
                MESSAGE_SINGLE(NORMAL, logger, "First window element is deleted due to different start of traffic generation and SF");
                while((*it).second.size() > 0)
                    (*it).second.pop_front();
                    
                NeedsNewEstimatePerAddress[rx] = true;
            }
        
            // If enough SFs have been evaluated -> estimate traffic
            int windowedSFs = (*it).second.size();
            if(windowedSFs >= averageOverSFs)
            {
                MESSAGE_SINGLE(NORMAL, logger, "Enough SFs for evaluation: " << windowedSFs << " for address " << rx);
                measurementDatapSF currentData;
                currentData.sentCompounds = 0;
                currentData.maxCompoundSize = 0;
                currentData.bitsTotal = 0;

                // Sum up windowed values and calculate mean traffic
                for(int i = 0; i < windowedSFs; i++)
                {
                    measurementDatapSF tmpData = (*it).second.at(i);
                    currentData.sentCompounds += tmpData.sentCompounds;
                    currentData.bitsTotal += tmpData.bitsTotal;
                    if(tmpData.maxCompoundSize > currentData.maxCompoundSize)
                        currentData.maxCompoundSize = tmpData.maxCompoundSize;
                }

                estimatedTraffic currentTraffic;
                currentTraffic.CompoundspSF = ceil((double) currentData.sentCompounds / (double) windowedSFs);
                currentTraffic.BitspSF = ceil((double) currentData.bitsTotal / (double) windowedSFs);
                currentTraffic.MaxCompoundSize = currentData.maxCompoundSize;

                if(currentTraffic.CompoundspSF > 0)
                {
                
                    if(friends.manager->getDRPchannelAccess()) // If DRP channel access is off don't prepare a connection
                    {
                        if(!trafficEstimationConfig.overWriteEstimation)
                        {
                            if(lastSetTrafficPerAddress.find(rx) == lastSetTrafficPerAddress.end())
                            {
                                // This is the first traffic estimation for this target
                                MESSAGE_SINGLE(NORMAL, logger, "Traffic measurement finished for this connection. Number of compounds: "
                                << currentTraffic.CompoundspSF
                                <<"; Bits per SF: "
                                << currentTraffic.BitspSF
                                << "; Maximum compound size : "
                                << currentTraffic.MaxCompoundSize);
                            
                                friends.manager->prepareDRPConnection(rx, currentTraffic.CompoundspSF, currentTraffic.BitspSF, currentTraffic.MaxCompoundSize);
                            }
                            else
                            {
                                MESSAGE_SINGLE(NORMAL, logger, "Traffic measurement updated for this connection. Number of compounds: "
                                << currentTraffic.CompoundspSF
                                <<"; Bits per SF: "
                                << currentTraffic.BitspSF
                                << "; Maximum compound size : "
                                << currentTraffic.MaxCompoundSize);
                                
                                friends.manager->updateDRPConnection(rx, currentTraffic.CompoundspSF, currentTraffic.BitspSF, currentTraffic.MaxCompoundSize);
                            }
                        }
                        else
                        {
                            MESSAGE_SINGLE(NORMAL, logger, "Traffic measurement overwritten by manual values: "
                            << trafficEstimationConfig.CompoundspSF
                            <<"; Bits per SF: "
                            << trafficEstimationConfig.BitspSF
                            << "; Maximum compound size : "
                            << trafficEstimationConfig.MaxCompoundSize);
                        
                            friends.manager->prepareDRPConnection(rx, trafficEstimationConfig.CompoundspSF, trafficEstimationConfig.BitspSF, trafficEstimationConfig.MaxCompoundSize);
                        }
                        
                        // Save last set traffic characteristics
                        lastSetTrafficPerAddress[rx] = currentTraffic;
                    }
                    
                    // Mark estimation as done
                    NeedsNewEstimatePerAddress[rx] = false;
                }
                
                else
                {
                    assure(false, "Traffic estimation for " << rx << " at STA " << friends.manager->getMACAddress() << " was started, but afterwards 0 compounds were estimated! Too low traffic!?");
                }
                
                
                // Once filled keep window at constant size
                while((*it).second.size() >= averageOverSFs)
                    (*it).second.pop_front();
                
                
            }
            else MESSAGE_SINGLE(NORMAL, logger, "So far only " << windowedSFs << " SFs are evaluated for address " << rx);
            
        }

        // A traffic estimation was already done 
        else
        {
            // check if traffic differs from estimated traffic
            measurementDatapSF thisSFData = (*it).second.back();
            estimatedTraffic lastSetData = lastSetTrafficPerAddress[rx];
            if((thisSFData.sentCompounds > lastSetData.CompoundspSF*1.05) || (thisSFData.bitsTotal > lastSetData.BitspSF*1.05))
            {
                MESSAGE_SINGLE(NORMAL, logger, "ThisSF SentCompd " << thisSFData.sentCompounds << " | LastSet ComppSF " << lastSetData.CompoundspSF);
                //MESSAGE_SINGLE(NORMAL, logger, "ThisSF TotalBits " << thisSFData.bitsTotal << " | LastSet BitspSF " << lastSetData.BitspSF);
            
                MESSAGE_SINGLE(NORMAL, logger, "The current traffic differs from the last set traffic characteristic by " << ((float)thisSFData.sentCompounds / (float)lastSetData.CompoundspSF -1.0)*100 << " % in compounds and " << ((float)thisSFData.bitsTotal / (float)lastSetData.BitspSF -1.0)*100 << " % in total bits");
                
                // The current SF has different characteristic than the last set traffic characteristic
                NeedsNewEstimatePerAddress[rx] = true;
                // Remove all evaluated SFs from the window so that the estimation is done when enough SFs are evaluated again
                while((*it).second.size() > 0)
                    (*it).second.pop_front();
            }
        }

        // initialize a new window element
        measurementDatapSF emptyData;
        emptyData.sentCompounds = 0;
        emptyData.maxCompoundSize = 0;
        emptyData.bitsTotal = 0;
        (*it).second.push_back(emptyData);
    
    }
}

void
TrafficEstimation_mean::doOnData(const wns::ldk::CompoundPtr& compound)
{
    bitsPerSF += compound->getLengthInBits();

    this->getDeliverer()->getAcceptor(compound)->onData(compound);
//     MESSAGE_SINGLE(NORMAL, this->logger, "Receiving compound");
}
