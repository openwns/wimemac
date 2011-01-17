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

from openwns import dBm, dB, fromdB

import wimemac.Logger

class MCS(object):
    #modulation = None
    #codingRate = None
    nIBP6S = None
    minRX = None
    minSINR = None

#    def __init__(self, modulation, codingRate, minSINR):
    def __init__(self, nIBP6S, minRX, minSINR):
        #self.modulation = modulation
        #self.codingRate = codingRate
        self.nIBP6S = nIBP6S
        self.minRX = minRX
        self.minSINR = minSINR

class PhyMode(MCS):
    numberOfSpatialStreams = None
    numberOfDataSubcarriers = None
    plcpMode = None
    guardIntervalDuration = None

#    def __init__(self, modulation, codingRate, numberOfSpatialStreams, numberOfDataSubcarriers, plcpMode, guardIntervalDuration, minSINR):
    def __init__(self, nIBP6S, numberOfSpatialStreams, numberOfDataSubcarriers, plcpMode, guardIntervalDuration, minRX, minSINR):
        #super(PhyMode, self).__init__(modulation=modulation, codingRate=codingRate, minSINR=minSINR)
        super(PhyMode, self).__init__(nIBP6S=nIBP6S, minRX=minRX, minSINR=minSINR)
        self.numberOfSpatialStreams = numberOfSpatialStreams
        self.numberOfDataSubcarriers = numberOfDataSubcarriers
        self.plcpMode = plcpMode
        self.guardIntervalDuration = guardIntervalDuration

    def __str__(self):
        return "(%s)*%d*%d" % (self.nIBP6S, self.numberOfDataSubcarriers, self.numberOfSpatialStreams)

class PhyModesDeliverer(object):
    """ Super class for all phy modes """
    defaultPhyMode = None
    phyModePreamble = None
    MCSs = None
    switchingPointOffset = None

class ECMA368(PhyModesDeliverer):
    """ Deliverer class for the basic phy modes of ECMA-368 """

    def __init__(self, defPhyMode = 0):
        self.MCSs = [MCS(100,  dBm(-80.8), dB(6.0)), #TODO minSINR
                     MCS(150,  dBm(-78.9), dB(8.7)),
                     MCS(200,  dBm(-77.8), dB(8.8)),
                     MCS(300, dBm(-75.9), dB(12.0)),
                     MCS(375, dBm(-74.5), dB(15.4)),
                     MCS(600, dBm(-72.8), dB(18.8)),
                     MCS(750, dBm(-71.5), dB(23.5)),
                     MCS(900, dBm(-70.4), dB(24.8))]
        self.switchingPointOffset = dB(1.0) # TODO Check switchingPointOffset
        self.phyModePreamble = makeBasicPhyMode(100, dBm(-80.8), dB(6.0))
        #self.defaultPhyMode = makeBasicPhyMode(900, dBm(-70.4), dB(24.8))
        self.defaultPhyMode = makeBasicPhyMode(self.MCSs[defPhyMode].nIBP6S, self.MCSs[defPhyMode].minRX, self.MCSs[defPhyMode].minSINR)
        
    def getLowest(self):
        return(makeBasicPhyMode(100, dBm(-80.8) ,dB(6.0)))

#def makeBasicPhyMode(modulation, codingRate, minSINR):
def makeBasicPhyMode(nIBP6S, minRX, minSINR):
    return PhyMode(#modulation = modulation,
                   #codingRate = codingRate,
                   nIBP6S = nIBP6S,
                   numberOfSpatialStreams = 1,
                   numberOfDataSubcarriers = 1,
                   plcpMode = "STANDARD",
                   guardIntervalDuration = 0.8e-6,
                   minRX = minRX,
                   minSINR = minSINR)

