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

#include <WIMEMAC/Component.hpp>
#include <WIMEMAC/convergence/PhyUser.hpp>
#include <WIMEMAC/helper/contextprovider/CommandInformation.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/utils.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/Exception.hpp>
#include <sstream>

using namespace wimemac;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Component,
	wns::node::component::Interface,
	"wimemac.Component",
	wns::node::component::ConfigCreator
	);

Component::Component(
	wns::node::Interface* _node,
	const wns::pyconfig::View& _config) :
	dll::Layer2(_node, _config, NULL),
	lowerConvergence(NULL)
{
} // Component

void
Component::doStartup()
{
    dll::Layer2::doStartup();
}

Component::~Component()
{
} // ~Component

void
Component::onNodeCreated()
{
    // Initialize management and control services
    getMSR()->onMSRCreated();
    getCSR()->onCSRCreated();

    fun->onFUNCreated();

    MESSAGE_BEGIN(NORMAL, logger, m, fun->getName());
    m << ": onNodeCreated(), setting physical layer";
    MESSAGE_END();

    // set services in lower convergence to communicate with lower layer
    lowerConvergence = getFUN()->findFriend<convergence::IPhyServices*>(
        getConfig().get<std::string>("phyuser.functionalUnitName"));
    assure(lowerConvergence, "No lowerConvergence available");

    lowerConvergence->setDataTransmissionService(
        getService<wns::service::Service*>(
            getConfig().get<std::string>("phyDataTransmission")));

    lowerConvergence->setNotificationService(
        getService<wns::service::Service*>(
            getConfig().get<std::string>("phyNotification")));


    // Add compound-based context providers
    // TODO: Reading names from configuration
    getNode()->getContextProviderCollection().addProvider(
        wimemac::helper::contextprovider::SourceAddress(fun, "upperConvergence"));
    getNode()->getContextProviderCollection().addProvider(
        wimemac::helper::contextprovider::TargetAddress(fun, "upperConvergence"));
    getNode()->getContextProviderCollection().addProvider(
        wimemac::helper::contextprovider::IsUnicast(fun, "upperConvergence"));
    getNode()->getContextProviderCollection().addProvider(
        wimemac::helper::contextprovider::DataBitsPerSymbol(fun, "ManagerCommand"));
    getNode()->getContextProviderCollection().addProvider(
        wimemac::helper::contextprovider::SpatialStreams(fun, "ManagerCommand"));
    //getNode()->getContextProviderCollection().addProvider(
      //  wimemac::helper::contextprovider::IsForMe(fun, "upperConvergence"));

} // onNodeCreated

void
Component::onWorldCreated()
{
} // onWorldCreated


