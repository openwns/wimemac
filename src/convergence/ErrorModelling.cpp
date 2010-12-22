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

#include <WIMEMAC/convergence/PhyMode.hpp>
#include <WIMEMAC/convergence/ErrorModelling.hpp>
#include <WIMEMAC/lowerMAC/Manager.hpp>

using namespace wimemac::convergence;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::convergence::ErrorModelling,
    wns::ldk::FunctionalUnit,
    "wimemac.convergence.ErrorModelling",
    wns::ldk::FUNConfigCreator);


ErrorModelling::ErrorModelling(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _config) :
    wns::ldk::fu::Plain<ErrorModelling, ErrorModellingCommand>(fun),
    config(_config),
    logger(config.get<wns::pyconfig::View>("logger")),
    phyUserCommandName(config.get<std::string>("phyUserCommandName")),
    phyUserName(config.get<std::string>("phyUserName")),
    managerName(config.get<std::string>("managerName")),
    managerCommandName(config.get<std::string>("managerCommandName")),
    dis(),
    perProbe_(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()), "wimemac.errormodelling.PER"))
{

    // read the localIDs from the config
    wns::probe::bus::ContextProviderCollection localContext(&fun->getLayer()->getContextProviderCollection());

    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        localContext.addProvider( wns::probe::bus::contextprovider::Constant(key, value) );
    }

    lossRatio = wns::probe::bus::collector(localContext, config, "lossRatioProbeName");


    // Fill the error mapping from the config
    wns::pyconfig::View configview = config.getView("snr2pmean_100");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_100_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_100_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_100_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_150");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_150_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_150_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_150_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_200");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_200_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_200_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_200_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_300");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_300_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_300_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_300_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_375");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_375_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_375_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_375_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_600");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_600_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_600_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_600_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }


    configview = config.getView("snr2pmean_750");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_750_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_750_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_750_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }

    configview = config.getView("snr2pmean_900");
    for(int i = 0; i < configview.len("mapping"); ++i)
    {
        wns::pyconfig::View mo = configview.getView("mapping", i);
        if(i == (configview.len("mapping") - 1))
        {
            snr2pmean_900_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(100), mo.get<float>("pmean"));
        }
        else
        {
            if(i == 0)
            {
                snr2pmean_900_.insert(SNR2PmeanMap::IntervalType::FromIncluding(-10).ToExcluding(mo.get<float>("snr")), 1);
            }
            wns::pyconfig::View nextMo = configview.getView("mapping", i+1);
            snr2pmean_900_.insert(SNR2PmeanMap::IntervalType::FromIncluding(mo.get<float>("snr")).ToExcluding(nextMo.get<float>("snr")), mo.get<float>("pmean"));
        }
    }
}

void ErrorModelling::onFUNCreated()
{
    friends.phyuser = getFUN()->findFriend<wimemac::convergence::PhyUser*>(phyUserName);
    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);

    // Fill the registry with the snr2pmean maps together with their corresponding phymodes as key
    // get lowest phymode and start filling the registry
    wimemac::convergence::PhyMode phyMode_ = friends.phyuser->getPhyModeProvider()->getPreamblePhyMode(friends.phyuser->getPhyModeProvider()->getDefaultPhyMode());
    assure(friends.phyuser->getPhyModeProvider()->hasLowestMCS(phyMode_), "Couldn't retrieve the lowest MCS");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_100_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_150_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_200_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_300_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_375_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_600_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(!friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too less MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_750_);

    // get the next higher MCS and proceed
    friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
    assure(friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_), "There are too many MCSs defined");
    snr2pmeanReg_.insert(phyMode_.getMCS(), &snr2pmean_900_);
}

void ErrorModelling::processIncoming(const wns::ldk::CompoundPtr& compound)
{

    // Calculate PER
    wns::Power rxPower_ = getFUN()->getCommandReader(phyUserCommandName)->
        readCommand<wimemac::convergence::PhyUserCommand>(compound->getCommandPool())->local.rxPower;
    wns::Power interference_ = getFUN()->getCommandReader(phyUserCommandName)->
        readCommand<wimemac::convergence::PhyUserCommand>(compound->getCommandPool())->local.interference;

    wns::Ratio sinr_ = wns::Ratio::from_dB(rxPower_.get_dBm() - interference_.get_dBm());

    //MESSAGE_BEGIN(NORMAL, logger, m, "DEBUG : Errormodelling " << sinr_);
    //m << " | rx " << rxPower_;
    //m << " | interference " << interference_;
    //MESSAGE_END();

    wimemac::convergence::PhyMode phyMode_ = getFUN()->getCommandReader(managerCommandName)->
        readCommand<wimemac::lowerMAC::ManagerCommand>(compound->getCommandPool())->getPhyMode();

    ErrorModellingCommand* emc = activateCommand(compound->getCommandPool());
    emc->local.sinr = sinr_;

    float pmean_ = NULL;

    SNR2PmeanMap* snr2pmeanMap_ = snr2pmeanReg_.find(phyMode_.getMCS());

    if (!snr2pmeanMap_->has(float(sinr_.get_dB())))
    {
        if(sinr_.get_dB() < -10) pmean_ = 0;
        else if(sinr_.get_dB() > 100) pmean_ = snr2pmeanMap_->get(float(sinr_.get_dB()));
        else
        {
            // Needed to prevent rangemap bug if asking for an INT value
            MESSAGE_SINGLE(NORMAL, logger, "DEBUG: SINR before" << sinr_.get_dB());
            sinr_.set_dB(sinr_.get_dB() + 0.001);
            MESSAGE_SINGLE(NORMAL, logger, "DEBUG: SINR after" << sinr_.get_dB());
        }
    }
    else pmean_ = snr2pmeanMap_->get(float(sinr_.get_dB()));

    // get frame length; check if compound is preamble or psdu
    Bit compsize_ = compound->getLengthInBits();

    double q = 1 / (pmean_ - 7 + 2); // K = 7 is constraint length of the convolutional code
    if (pmean_ - 7 + 2 <= 0) q = 1;
    emc->local.per = 1 - pow( (1 - q), (compsize_-(7-1)) );

    // If rxPower is below the minimum rxPower set PER = 1
    if (rxPower_ < friends.manager->getPhyMode(compound->getCommandPool()).getMinRxPower())
        emc->local.per = 1;

    // Put per into probe if it's a payload compound
    if (!friends.manager->isPreamble(compound->getCommandPool()) &&
        !friends.manager->isBeacon(compound->getCommandPool()) &&
        friends.manager->isForMe(compound->getCommandPool()))
        perProbe_->put(compound, emc->local.per);


    wns::service::dll::UnicastAddress tx = friends.manager->getTransmitterAddress(compound->getCommandPool());
    MESSAGE_BEGIN(NORMAL, logger, m, "New compound with SNR " << sinr_);
    m << " | CompSize " << compsize_;
    m << " | NIBP6S " << phyMode_.getInfoBitsPer6Symbols();
    m << " | Pmean " << pmean_;
    m << " | PER " << emc->local.per;
    m << " | from address " << tx;
    MESSAGE_END();

    // Mark compound according to per evaluation
    if(!lastPreambleWasDecoded.knows(tx)) lastPreambleWasDecoded.insert(tx, false);
    if ( dis() < emc->local.per ){

        if (lossRatio != NULL)
        {
            lossRatio->put(compound, 1);
        }
        emc->local.checkOK = false;
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " CRCheck - failed - compound marked";
        MESSAGE_END();

        if (friends.manager->isPreamble(compound->getCommandPool()))
            lastPreambleWasDecoded.update(tx, false);
    }
    else
    {
        if (friends.manager->isPreamble(compound->getCommandPool()))
        {
            lastPreambleWasDecoded.update(tx, true);
            emc->local.checkOK = true;
        }
        else
        {
            // Check if before received preamble could be decoded, otherwise mark payload compound as bad
            if(lastPreambleWasDecoded.find(tx) == true)
            {
                emc->local.checkOK = true;
                if (lossRatio != NULL)
                {
                    lossRatio->put(compound, 0);
                }
                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " CRCheck - success";
                MESSAGE_END();

            }
            else
            {
                emc->local.checkOK = false;
                if (lossRatio != NULL)
                {
                    lossRatio->put(compound, 1);
                }
                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " CRCheck - failed because of bad preamble - compound marked";
                MESSAGE_END();
            }
        }

    }

}

void ErrorModelling::processOutgoing(const wns::ldk::CompoundPtr& /*compound*/)
{
    //MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    //m << " passing through Errormodelling outgoing";
    //MESSAGE_END();
}

wimemac::convergence::MCS
ErrorModelling::getMaxPosMCS(wns::Ratio sinr_, Bit maxCompoundSize_, double per_)
{
    // Calculate minimum allowed pmean
    double pow_ = pow( (1-per_) , (1 / ((double)maxCompoundSize_ - 7 + 1)) );
    float pmean_ = 7 - 2 + 1 / ( 1 - pow_ );

    // Search through the mapping registry for the highest MCS with a pmean greater than the calculated one
    wimemac::convergence::PhyMode phyMode_ = friends.phyuser->getPhyModeProvider()->getPreamblePhyMode(friends.phyuser->getPhyModeProvider()->getDefaultPhyMode());
    assure(friends.phyuser->getPhyModeProvider()->hasLowestMCS(phyMode_), "Couldn't retrieve the lowest MCS");

    bool foundMax_ = false;
    while (!foundMax_)
    {
        SNR2PmeanMap* snr2pmeanMap_ = snr2pmeanReg_.find(phyMode_.getMCS());
        // Check if pmean for this MCS and SNR is greater than the min pmean

        float pmeanMapped_;
        if (!snr2pmeanMap_->has(float(sinr_.get_dB())))
        {
            if(sinr_.get_dB() < -10) pmeanMapped_ = 0;
            else if(sinr_.get_dB() > 100) pmeanMapped_ = snr2pmeanMap_->get(float(sinr_.get_dB()));
            // Needed to prevent rangemap bug if asking for an INT value
            sinr_.set_dB(sinr_.get_dB() + 0.001);
            MESSAGE_SINGLE(NORMAL, logger, "DEBUG: SINR after" << sinr_.get_dB());
        }
        else pmeanMapped_ = snr2pmeanMap_->get(float(sinr_.get_dB()));

        if(pmean_ <= pmeanMapped_)
        {
            // Is the maximum MCS reached -> use this
            if(friends.phyuser->getPhyModeProvider()->hasHighestMCS(phyMode_))
            {
                foundMax_ = true;
            }
            // Increment MCS to check if next one also meets the min pmean
            else friends.phyuser->getPhyModeProvider()->mcsUp(phyMode_);
        }
        else
        {
            // The MCS did not meet the requirements -> use next lower MCS
            if (friends.phyuser->getPhyModeProvider()->hasLowestMCS(phyMode_))
            {
                MESSAGE_SINGLE(NORMAL, logger, "LinkEval : Even lowest PhyMode is not sufficient! Returning lowest MCS to proceed");
            }
            else friends.phyuser->getPhyModeProvider()->mcsDown(phyMode_);
            foundMax_ = true;
        }
    }

    // MESSAGE_SINGLE(NORMAL, logger, "Calculate MaxMCS : " <<" Returning Phymode with "<< phyMode_.getDataRate() << " MBit/s" );
    return phyMode_.getMCS();
}

double 
ErrorModelling::getErrorRateForCommandFrames(wns::Ratio sinr_, Bit maxCompoundSize_)
{
   
  float pmean;
  wimemac::convergence::PhyMode phyMode_ = friends.phyuser->getPhyModeProvider()->getPreamblePhyMode();
  SNR2PmeanMap* snr2pmeanMap_ = snr2pmeanReg_.find(phyMode_.getMCS());

    if (!snr2pmeanMap_->has(float(sinr_.get_dB())))
    {
        if(sinr_.get_dB() < -10) pmean = 0;
        else if(sinr_.get_dB() > 100) pmean = snr2pmeanMap_->get(float(sinr_.get_dB()));
        else
        {
            // Needed to prevent rangemap bug if asking for an INT value
            MESSAGE_SINGLE(NORMAL, logger, "DEBUG: SINR before" << sinr_.get_dB());
            sinr_.set_dB(sinr_.get_dB() + 0.001);
            MESSAGE_SINGLE(NORMAL, logger, "DEBUG: SINR after" << sinr_.get_dB());
        }
    }
    else pmean = snr2pmeanMap_->get(float(sinr_.get_dB()));

        double q = 1 / (pmean - 7 + 2); // K = 7 is constraint length of the convolutional code
    if (pmean - 7 + 2 <= 0) q = 1;
    return(1 - pow( (1 - q), (maxCompoundSize_-(7-1)) ));
}