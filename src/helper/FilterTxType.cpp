/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WIMEMAC/helper/FilterTxType.hpp>
#include <DLL/compoundSwitch/CompoundSwitch.hpp>
#include <WNS/Exception.hpp>

using namespace wimemac::helper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FilterTxType,
                                    dll::compoundSwitch::Filter,
                                    "wimemac.helper.FilterTxType",
                                    dll::compoundSwitch::CompoundSwitchConfigCreator);

FilterTxType::FilterTxType(dll::compoundSwitch::CompoundSwitch* compoundSwitch, wns::pyconfig::View& config) :
    dll::compoundSwitch::Filter(compoundSwitch, config)
{
    this->commandReader = this->compoundSwitch_->getFUN()->getCommandReader(config.get<std::string>("commandName"));

    // set accepting tx type
    std::string txTypeString = config.get<std::string>("txType");

    // convert to lower case
    std::transform(txTypeString.begin(), txTypeString.end(), txTypeString.begin (), (int(*)(int)) std::tolower);

    // .compare == 0 --> no differences between the strings
    if(txTypeString.compare("drp") == 0)
    {
        this->acceptingTxType = DRP;
        return;
    }

    if(txTypeString.compare("pca") == 0)
    {
        this->acceptingTxType = PCA;
        return;
    }

    wns::Exception e;
    e << "Unknown tx type \"" << txTypeString << " - forgot to implement conversion from string to tx type for new type?";
    throw wns::Exception(e);
}
FilterTxType::~FilterTxType()
{
}

void
FilterTxType::onFUNCreated()
{
}

bool
FilterTxType::filter(const wns::ldk::CompoundPtr& compound) const
{
    return(this->commandReader->readCommand<wimemac::IKnowsTxTypeCommand>(compound->getCommandPool())->getTxType() == this->acceptingTxType);
}
