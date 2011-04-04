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

#ifndef WIMEMAC_HELPER_CONTEXTPROVIDER_COMMANDINFORMATION_HPP
#define WIMEMAC_HELPER_CONTEXTPROVIDER_COMMANDINFORMATION_HPP

// declaration of the commands which are read by the specific command readers
//#include <WIMEMAC/pathselection/BeaconLinkQualityMeasurement.hpp>
#include <DLL/UpperConvergence.hpp>
#include <WIMEMAC/convergence/PhyMode.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/probe/bus/CommandContextProvider.hpp>

namespace wimemac { namespace helper { namespace contextprovider {


    /**
	 * @brief Context provider for a given compound: 1 if the compound is a
	 *   unicast transmission, 0 otherwise
	 *
	 * The status is determined by checking if the target address is valid
     *
	 */
    class IsUnicast:
        virtual public wns::probe::bus::CommandContextProvider<dll::UpperCommand>
    {
    public:
        IsUnicast(wns::ldk::fun::FUN* fun, std::string ucCommandName):
            wns::probe::bus::CommandContextProvider<dll::UpperCommand>(fun, ucCommandName, "MAC.CompoundIsUnicast")
            {};
    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const dll::UpperCommand* command) const
            {
                if(command->peer.targetMACAddress.isValid())
                {
                    c.insertInt(getKey(), 1);
                }
                else
                {
                    c.insertInt(getKey(), 0);
                }
            };
    };

    /**
     * @brief Context provider for a given compound: Filters by the source
     *  address given in the upperConvergenceComand
     */
    class SourceAddress:
        virtual public wns::probe::bus::CommandContextProvider<dll::UpperCommand>
    {
    public:
        SourceAddress(wns::ldk::fun::FUN* fun, std::string ucCommandName):
            wns::probe::bus::CommandContextProvider<dll::UpperCommand>(fun, ucCommandName, "MAC.CompoundSourceAddress")
            {};

    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const dll::UpperCommand* command) const
            {
                if(command->peer.sourceMACAddress.isValid())
                {
                    // if the command is activated, we add the tx address to the
                    // context
                    c.insertInt(this->key,
                                command->peer.sourceMACAddress.getInteger());
                }
            }
    };

    /**
     * @brief Context provider for a given compound: Filters by the target
     *  address given in the upperConvergenceComand
     */
    class TargetAddress:
        virtual public wns::probe::bus::CommandContextProvider<dll::UpperCommand>
    {
    public:
        TargetAddress(wns::ldk::fun::FUN* fun, std::string ucCommandName):
            wns::probe::bus::CommandContextProvider<dll::UpperCommand>(fun, ucCommandName, "MAC.CompoundTargetAddress")
            {};

    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const dll::UpperCommand* command) const
            {
                if(command->peer.targetMACAddress.isValid())
                {
                    // if the command is activated, we add the tx address to the
                    // context
                    c.insertInt(this->key,
                                command->peer.targetMACAddress.getInteger());
                }
            }
    };

    /**
     * @brief Context provider for a given compound: Filters by the index of the
     *    MCS with which the compound was send (will be send)
     *
     * The information is read from the phyUserCommand
     */
    class DataBitsPerSymbol:
        virtual public wns::probe::bus::CommandContextProvider<wimemac::IKnowsFrameTypeCommand>
    {
    public:
        DataBitsPerSymbol(wns::ldk::fun::FUN* fun, std::string managerCommandName):
            wns::probe::bus::CommandContextProvider<wimemac::IKnowsFrameTypeCommand>(fun, managerCommandName, "MAC.CompoundDBPS")
            {};

    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const wimemac::IKnowsFrameTypeCommand* command) const
            {
                wimemac::convergence::PhyMode phymode = command->getPhyMode();
                c.insertInt(this->key, phymode.getDataRate());
            }
    };

    /**
     * @brief Context provider for a given compound: Filters by the number of
     *    spatial streams with which the compound was send (will be send)
     *
     * The information is read from the phyUserCommand
     */
    class SpatialStreams :
        virtual public wns::probe::bus::CommandContextProvider<wimemac::IKnowsFrameTypeCommand>
    {
    public:
        SpatialStreams(wns::ldk::fun::FUN* fun, std::string managerCommandName):
            wns::probe::bus::CommandContextProvider<wimemac::IKnowsFrameTypeCommand>(fun, managerCommandName, "MAC.CompoundSpatialStreams")
            {};

    private:
        virtual void
        doVisitCommand(wns::probe::bus::IContext& c, const wimemac::IKnowsFrameTypeCommand* command) const
            {
                wimemac::convergence::PhyMode mcs = command->getPhyMode();
                c.insertInt(this->key, mcs.getNumberOfSpatialStreams());
            }
    };
}}}

#endif //WIMEMAC_HELPER_CONTEXTPROVIDER_COMMANDINFORMATION_HPP
