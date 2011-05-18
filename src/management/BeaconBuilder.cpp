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
 
#include <WIMEMAC/management/BeaconBuilder.hpp>
#include <DLL/Layer2.hpp>



STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wimemac::management::BeaconBuilder,
    wns::ldk::FunctionalUnit,
    "wimemac.management.BeaconBuilder",
    wns::ldk::FUNConfigCreator );

using namespace wimemac::management;

BeaconBuilder::BeaconBuilder( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ ) :
        wns::ldk::fu::Plain<BeaconBuilder, BeaconCommand>(fun),
        logger(config_.get("logger")),
        currentBeacon(),
        beaconPhyMode(config_.getView("beaconPhyMode")),
        managerName(config_.get<std::string>("managerName")),
        MASProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()), "wimemac.bb.numberOfMAS")),
        allocatedMASProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&fun->getLayer()->getContextProviderCollection()), "wimemac.allocatedMAS"))
{
    friends.keyReader = fun->getProxy()->getCommandReader("upperConvergence");
    BeaconEvaluator::SetLogger(logger);
    
}

BeaconBuilder::~BeaconBuilder()
{
    // Write allocated MASs into text probe
    std::stringstream outputStr;
    outputStr << " STA ID: " <<  tmpID << " allocated MASs: [ ";
    bool hasMASallocated = false;

    Vector drpMap_ = getAllocatedMASs();
    for (int i = 0; i < drpMap_.size(); i++)
    {
        if (drpMap_[i] == true)
        {
            outputStr << i << ", ";
            hasMASallocated = true;
        }
    }
    outputStr << "]";
    if (hasMASallocated) allocatedMASProbe->put(0.0, boost::make_tuple("wimemac.allocatedMAS", outputStr.str()));
}

void
BeaconBuilder::doOnData( const wns::ldk::CompoundPtr& compound )
{

    wns::ldk::CommandPool* commandPool = compound->getCommandPool();

    wns::service::dll::UnicastAddress iam
    = getFUN()->findFriend<dll::UpperConvergence*>("upperConvergence")->getMACAddress();

    dll::UpperCommand* uc =
        friends.keyReader->readCommand<dll::UpperCommand>(commandPool);
    wns::service::dll::UnicastAddress tx = uc->peer.sourceMACAddress;

    //evaluate beacon, only Beacon Command is necessary
    if(tx != iam)
    {
        BeaconEvaluator::BeaconExamination(tx, iam, getCommand(compound->getCommandPool()),logger);
    }
}

bool
BeaconBuilder::doIsAccepting( const wns::ldk::CompoundPtr& ) const
{
}

void
BeaconBuilder::doSendData( const wns::ldk::CompoundPtr& compound )
{
    if(!getConnector()->hasAcceptor(compound)){
        MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: can't send Beacon!");

    }
    else{
        getConnector()->getAcceptor(compound)->sendData(compound);
        //MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Beacon sent, waiting for Phy to finish!");
    }
}

void
BeaconBuilder::onFUNCreated()
{
    friends.queueInterface=
    getFUN()->findFriend<wimemac::helper::IDRPQueueInterface*>("DRPScheduler");

    friends.Upper =
    getFUN()->findFriend<dll::UpperConvergence*>("upperConvergence");

    friends.manager = getFUN()->findFriend<wimemac::lowerMAC::Manager*>(managerName);
    tmpID = friends.manager->getMACAddress();

    BeaconEvaluator::setFriend(friends.queueInterface);
    BeaconEvaluator::setManagerFriend(friends.manager);
    
}

void
BeaconBuilder::doWakeup()
{

    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Wakeup called!");

    //Evaluate PER for established connections
    evaluatePERforConnections();

    //Inform DRPmapManager about a new BP start
    DRPmapManager->onBPStarted();
    friends.manager->onBPStart(BPDuration);
    
    //Evaluate unestablished connection patterns
    BeaconEvaluator::EvaluateConnection();
    //create new drp maps if nessecary
    BeaconEvaluator::CreateDRPMaps();

    //Create beacon compound and send beacon
    doSendData(CreateBeacon());
    // Clear Availability Bitmap to ensure the received map will only be used during the SF of reception to avoid changes not being recognized
    BeaconEvaluator::ClearAvailabilityBitmap();

}

bool
BeaconBuilder::isBeacon(const wns::ldk::CommandPool* commandPool) const
{
    return getFUN()->getCommandReader("BeaconCommand")->commandIsActivated(commandPool);
}

void
BeaconBuilder::calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    commandPoolSize = 0;
    sduSize = 8*8; // Beacon Parameter size
    sduSize += 8*(1+1+1+ ceil(NumberOfBPSlots/4) + 2*NumberOfBPSlots); // mandatory BPOIE

    if(getCommand(commandPool)->HasAvailabilityIE())
    {
        // add size for DRP Availability IE
        sduSize += 8*34; // assume maximum size; Actually it differs between 2-34 octets
    }
    if(getCommand(commandPool)->HasDRPIE())
    {
        // add size for DRP IE(s)
        sduSize += 8*10 * getCommand(commandPool)->GetDRPIESize(); // assume 1 DRP allocation per DRP IE. Each DRP Allocation adds 4 octets
    }
    if(getCommand(commandPool)->HasProbeIE())
    {
        // add size for Probe IE
        sduSize += 5*8;
    }
}

wns::ldk::CompoundPtr
BeaconBuilder::CreateBeacon()
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Create beacon");

    wns::ldk::CompoundPtr compound = friends.manager->createCompound(friends.manager->getMACAddress(), wns::service::dll::UnicastAddress(), BEACON, true, 0.0);
    friends.manager->setPhyMode(compound->getCommandPool(), beaconPhyMode);

    BeaconCommand* bc = activateCommand(compound->getCommandPool());

    //ask for established and new drp connections
    if(BeaconEvaluator::CreateDRPIE(getCommand(compound->getCommandPool())))
    {
        //DRPIE is included
        getCommand(compound->getCommandPool())->peer.HasDRPIE = true;
        MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: insert a DRPIE ");//at least one DRPIE inserted
    }
    if(BeaconEvaluator::CreateBPOIE(getCommand(compound->getCommandPool())))
    {
      MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: insert a Beacon Period Occupancy IE ");
    }

    // attach DRP Availability IE to beacon if it's requested by a Probe IE
    if(requestedProbes.drpAvailability > 0)
    {
        Vector drpAvailability_ (256, false);
        DRPmapManager->GetGlobalPattern(drpAvailability_);

        requestedProbes.drpAvailability--;
        MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: insert a DRP Availability IE | replying to probe the next " << requestedProbes.drpAvailability << " following beacons");

        // Switch values so that available slots are marked as true
        for (int i = 0; i < 256; i++)
        {
            assure(i < drpAvailability_.size(), "Vector boundary exeeded; loop error!");
            drpAvailability_[i] = !(drpAvailability_[i]);
        }

        getCommand(compound->getCommandPool())->peer.availabilityBitmap = drpAvailability_;
        getCommand(compound->getCommandPool())->peer.HasAvailabilityIE = true;

    }

    if(BeaconEvaluator::CreateProbeIE(getCommand(compound->getCommandPool())))
    {
        //Probe IE is included
        MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: insert a Probe IE "); //for one or more IEs and receivers
    }

    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Beacon has a size of : " << compound->getLengthInBits());
    return compound;
}

void
BeaconBuilder::BuildDTPmap()
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: BP is over and now build a new DTP map!");
    int numberOfMASReserved = BeaconEvaluator::CollectDRPmaps(BPDuration);
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: Reserved Slots : " << numberOfMASReserved);
    MASProbe->put(numberOfMASReserved);
    
    //to do: it's a workaround to catch the BP end signal, this function call is at the wrong position here
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: BP is over and now find possible parallel users!");
    
    friends.manager->UpdateDRPMap(DRPmapManager->GetGlobalHardDRPmap());
}

void
BeaconBuilder::prepareDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: prepareDRPConnection!");
    wimemac::convergence::PhyMode defPhyMode_ = friends.manager->getDefaultPhyMode();
    BeaconEvaluator::CreateDRPManager(rx,CompoundspSF,BitspSF,MaxCompoundSize, defPhyMode_);
}

void
BeaconBuilder::updateDRPConnection(wns::service::dll::UnicastAddress rx, int CompoundspSF, int BitspSF, int MaxCompoundSize)
{
    MESSAGE_SINGLE(NORMAL, logger, "BeaconBuilder: updateDRPConnection!");
    BeaconEvaluator::UpdateDRPManager(rx,CompoundspSF,BitspSF,MaxCompoundSize);
}

