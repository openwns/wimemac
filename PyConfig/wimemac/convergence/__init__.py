###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2008
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 16, D-52074 Aachen, Germany
# phone: ++49-241-80-27910,
# fax: ++49-241-80-22242
# email: info@openwns.org
# www: http://www.openwns.org
# _____________________________________________________________________________
#
# openWNS is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 2 as published by the
# Free Software Foundation;
#
# openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

from wimemac.convergence.PreambleGenerator import PreambleGenerator
from wimemac.convergence.TxDurationSetter import TxDurationSetter
from wimemac.convergence.ChannelState import *
from wimemac.convergence.FrameSynchronization import *
from wimemac.convergence.ErrorModelling import ErrorModelling
from wimemac.convergence.PhyUser import PhyUser

names = dict()
names['preambleGenerator'] = 'Preamble'
names['preambleGeneratorCMD'] = 'PreambleCommand'
names['txDurationSetter'] = 'TxDuration'
names['txDurationSetterCMD'] = 'TxDurationCommand'
names['channelState'] = 'ChannelState'
names['channelStateCMD'] = 'ChannelStateCommand'
names['frameSynchronization'] = 'FrameSynchronization'
names['frameSynchronizationCMD'] = 'FrameSynchronizationCommand'
names['errorModelling'] = 'ErrorModelling'
names['errorModellingCMD'] = 'ErrorModelCommand'
names['phyUser'] = 'PhyUser'
names['phyUserCMD'] = 'PhyUserCommand'

def getFUN(transceiverAddress, names, config, myFUN, logger, probeLocalIDs):
    FUs = []
    FUs.append(PreambleGenerator(name = names['preambleGenerator'] + str(transceiverAddress),
                                 commandName = names['preambleGeneratorCMD'],
                                 phyUserName = names['phyUser'] + str(transceiverAddress),
                                 managerName = names['manager'] + str(transceiverAddress),
                                 parentLogger = logger))


    FUs.append(TxDurationSetter(name = names['txDurationSetter'] + str(transceiverAddress),
                                commandName = names['txDurationSetterCMD'],
                                managerName = names['manager'] + str(transceiverAddress),
                                parentLogger = logger))

    FUs.append(ChannelState(name = names['channelState'] + str(transceiverAddress),
                            commandName = names['channelStateCMD'],
                            managerName = names['manager'] + str(transceiverAddress),
                            phyUserCommandName = names['phyUserCMD'],
                            crcCommandName = names['errorModellingCMD'],
                            txStartEndName = names['phyUser'] + str(transceiverAddress),
                            rxStartEndName = names['frameSynchronization'] + str(transceiverAddress),
                            probePrefix = 'wimemac.channelState',
                            config = ChannelStateConfig(),
                            parentLogger = logger))


    FUs.append(FrameSynchronization(name = names['frameSynchronization'] + str(transceiverAddress),
                                    commandName = names['frameSynchronizationCMD'],
                                    managerName = names['manager'] + str(transceiverAddress),
                                    crcCommandName = names['errorModellingCMD'],
                                    config = FrameSynchronizationConfig(),
                                    parentLogger = logger))


    FUs.append(ErrorModelling(name = names['errorModelling'] + str(transceiverAddress),
                              commandName = names['errorModellingCMD'],
                              phyUserCommandName = names['phyUserCMD'],
                              phyUserName = names['phyUser'] + str(transceiverAddress),
                              lossRatioProbeName='wimemac.crcLoss',
                              managerCommandName = names['managerCMD'],
                              managerName = names['manager'] + str(transceiverAddress),
                              channelModel = config.channelModel,
                              parentLogger = logger))
                             
    """ This is a special Functional Unit in the 'fun' which is used
    to communicate with the physical layer. This Functional Unit must
    fit to the physical layer. It is expected that this Functional
    Unit can be exchanged in order to use another physical layer and
    other parts of the protocol stack can remain""" 
                                    
    FUs.append(PhyUser(functionalUnitName = names['phyUser'] + str(transceiverAddress),
                           commandName = names['phyUserCMD'],
                           managerName = names['manager'] + str(transceiverAddress),
                           txDurationCommandName = names['txDurationSetterCMD'],
                           phyconfig = config,
                           parentLogger = logger))

                                    
    # add created FUs to FUN
    for fu in FUs:
        myFUN.add(fu)

    for num in xrange(0, len(FUs)-1):
        FUs[num].connect(FUs[num+1])

    return([FUs[0], FUs[-1]])


