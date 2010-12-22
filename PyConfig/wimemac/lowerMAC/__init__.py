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

import dll.CompoundSwitch

from Manager import *

from wimemac.lowerMAC.ARQ import ARQ
from wimemac.lowerMAC.DCF import *
from wimemac.lowerMAC.Manager import *
from wimemac.lowerMAC.TXOP import *
from wimemac.lowerMAC.DRPScheduler import DRPScheduler
import wimemac.helper.Filter

names = dict()
names['drpScheduler'] = 'DRPScheduler'
names['drpSchedulerCMD'] = 'DRPSchedulerCommand'
names['txTypeSwitch'] = 'TxTypeSwitch'
names['txTypeSwitchCMD'] = 'TxTypeSwitchCommand'
names['txOP'] = 'TxOP'
names['txOPCMD'] = 'TxOPCommand'
names['continueTxOPSwitch'] = 'ContinueTxOPSwitch'
names['continueTxOPSwitchCMD'] = 'ContinueTxOPSwitchCommand'
names['dcf'] = 'DCF'
names['dcfCMD'] = 'DCFCommand'
names['dispatcher'] = 'Dispatcher'
names['dispatcherCMD'] = 'DispatcherCommand'
names['arq'] = 'ARQ'
names['manager'] = 'Manager'
names['managerCMD'] = 'ManagerCommand'

def getFUN(transceiverAddress, names, config, myFUN, logger, probeLocalIDs):
    ManagerFU = None

    [FUs, ManagerFU] = __getTopBlock__(transceiverAddress, names, config, myFUN, logger, probeLocalIDs)
    
    for fu in FUs:
            myFUN.add(fu)

    # connect FUs with each other
    for num in xrange(0, len(FUs)-1):
            FUs[num].connect(FUs[num+1])

    pcaBottom = __appendPCABlock__(transceiverAddress, names, config, myFUN, FUs[-1], logger, probeLocalIDs)

    arq = ARQ(names['arq'], 
              names['manager'] + str(transceiverAddress),
              logger)
    FUs.append(arq)       
    myFUN.add(arq)

    pcaBottom.connect(arq)

    return([FUs[0], FUs[-1], ManagerFU])



def __getTopBlock__(transceiverAddress, names, config, myFUN, logger, probeLocalIDs):
    FUs = []

    FUs.append(DRPScheduler(functionalUnitName = names['drpScheduler'],
                            commandName = names['drpSchedulerCMD'],
                            queuesize = 1E9,
                            frameduration = 256*256E-6,
                            managerName = names['manager'] + str(transceiverAddress),
                            dcfName = 'DCF' + str(transceiverAddress),
                            txopName = 'TxOP' + str(transceiverAddress),
                            beaconBuilderName = names['beaconBuilder'],
                            maxPER = config.maxPER,
                            patternPEROffset = config.patternPEROffset,
                            isDroppingAfterRetr = config.isDroppingAfterRetr,
                            deleteQueues = config.deleteQueues,
                            perMIBServiceName = 'perMIB' + str(transceiverAddress),
                            parentLogger = logger))

    ManagerFU = Manager(functionalUnitName = names['manager'] + str(transceiverAddress),
                                 commandName = names['managerCMD'],
                                 phyUserName = names['phyUser'] + str(transceiverAddress),
                                 channelStateName = names['channelState'] + str(transceiverAddress),
                                 upperConvergenceCommandName = "upperConvergence",
                                 drpSchedulerName = names['drpScheduler'],
                                 errorModellingName = names['errorModelling'] + str(transceiverAddress),
                                 protocolCalculatorName = names['protocolCalculator'] + str(transceiverAddress),
                                 config = wimemac.lowerMAC.ManagerConfig(),
                                 macaddress = transceiverAddress,
                                 reservationBlocks = config.reservationBlocks,
                                 useRandomPattern = config.useRandomPattern,
                                 useRateAdaptation = config.useRateAdaptation,
                                 useDRPchannelAccess = config.useDRPchannelAccess,
                                 usePCAchannelAccess = config.usePCAchannelAccess,
                                 parentLogger = logger)
    FUs.append(ManagerFU)
    
    return ([FUs, ManagerFU])


def __appendPCABlock__(transceiverAddress, names, config, myFUN, bottomFU, logger, probeLocalIDs):
    
    txTypeSwitch = dll.CompoundSwitch.CompoundSwitch(functionalUnitName = names['txTypeSwitch'] + str(transceiverAddress),
                                                    commandName = names['txTypeSwitchCMD'],
                                                    logName = 'TxTypeSwitch',
                                                    moduleName = 'WiMeMAC',
                                                    parentLogger = logger,
                                                    mustAccept = False)


    txop = TXOP(functionalUnitName = names['txOP'] + str(transceiverAddress),
                                 commandName = names['txOPCMD'],
                                 managerName = names['manager'] + str(transceiverAddress),
                                 protocolCalculatorName = names['protocolCalculator'] + str(transceiverAddress),
                                 txopWindowName = names['drpScheduler'],
                                 probePrefix = 'wimemac.txop',
                                 config = TXOPConfig(),
                                 parentLogger = logger)
                                    
                                    
    txOPSwitch = dll.CompoundSwitch.CompoundSwitch(functionalUnitName = names['continueTxOPSwitch'] + str(transceiverAddress),
                                                    commandName = names['continueTxOPSwitchCMD'],
                                                    logName = 'ContinueTxOPSwitch',
                                                    moduleName = 'WiMeMAC',
                                                    parentLogger = logger,
                                                    mustAccept = False)


    dcf = DCF(functionalUnitName = names['dcf'] + str(transceiverAddress),
                           commandName = names['dcfCMD'],
                           csName = names['channelState'] + str(transceiverAddress),
                           rxStartEndName = names['frameSynchronization'] + str(transceiverAddress),
                           drpSchedulerName = names['drpScheduler'],
                           config = DCFConfig(cwMin = 7, cwMax = 511),
                           parentLogger = logger)
                                                    
    dispatcher = openwns.Multiplexer.Dispatcher(commandName = names['dispatcherCMD'],
                                          functionalUnitName = names['dispatcher'] + str(transceiverAddress),
                                          opcodeSize = 0,
                                          parentLogger = logger,
                                          logName = 'Dispatcher',
                                          moduleName = 'WiMeMAC')
    
    for fu in [txTypeSwitch, txop, txOPSwitch, dcf, dispatcher]:
            myFUN.add(fu)
    
    txTypeSwitch.connectOnDataFU(bottomFU, dll.CompoundSwitch.FilterAll('All'))
    txTypeSwitch.connectSendDataFU(dispatcher, wimemac.helper.Filter.TxType('DRP', names['drpSchedulerCMD']))
    txTypeSwitch.connectSendDataFU(txop, wimemac.helper.Filter.TxType('PCA', names['drpSchedulerCMD']))
        
    txOPSwitch.connectOnDataFU(txop, dll.CompoundSwitch.FilterAll('All'))
    txOPSwitch.connectSendDataFU(dispatcher, wimemac.helper.Filter.TxOPType('DATA_TXOP', names['managerCMD']))
    txOPSwitch.connectSendDataFU(dcf, wimemac.helper.Filter.TxOPType('DATA', names['managerCMD']))
        
    dcf.connect(dispatcher)
    
    return(dispatcher)


