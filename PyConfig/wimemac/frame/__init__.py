###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2011
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
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

import math

from wimemac.frame.frameBuilder import *
import openwns.FCF
import openwns.FUN

names = dict()
names['dataCollector'] = 'DataCollector'
names['beaconCollector'] = 'Beacon'
names['frameBuilder'] =  'FrameBuilder'
names['frameBuilderCMD'] =  'FrameBuilderCommand'


def getFUN(transceiverAddress, names, config, myFUN, logger, probeLocalIDs):
    
    ### First beacon slot is emtpy, therefore another MAS is needed to get a valid result in any case
    BPSlots = math.ceil(float(config.numberOfStations)/3.0) +1

    FUs = []
    
    FUs.append(openwns.FCF.FrameBuilder(0,openwns.FCF.TimingControl(), 
             frameDuration = 256*256E-6,
             symbolDuration = 0.0,
             commandName = names['frameBuilderCMD'],
             functionalUnitName = names['frameBuilder']))

        
        
    FUs.append(openwns.FUN.Node(names['dataCollector'], DataCollector("Data", ((256-BPSlots)*256E-6 - 1E-12), logger)))

    FUs.append(openwns.FUN.Node(names['beaconCollector'], BeaconCollector(name = names['beaconCollector'], 
                                                         duration = BPSlots*256E-6,
                                                         beaconSlot = transceiverAddress,
                                                         beaconSlotDuration = 85E-6,
                                                         managerName = names['manager'] + str(transceiverAddress),
                                                         parentLogger = logger)))
                                                         
  
    FUs[0].add(openwns.FCF.BasicPhaseDescriptor(names['beaconCollector']))
    FUs[0].add(openwns.FCF.BasicPhaseDescriptor(names['dataCollector']))

  
    # add created FUs to FUN
    for fu in FUs:
        myFUN.add(fu)

    FUs[1].connect(FUs[0])
    FUs[2].connect(FUs[0])
    
    ######################
    # Return FrameBottom, FrameTopData, FrameTopBeacon
    return([FUs[0], FUs[1], FUs[2]])


