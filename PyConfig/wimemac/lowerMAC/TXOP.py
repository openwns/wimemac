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

import openwns.FUN
import openwns.pyconfig

import wimemac.Logger

class TXOPConfig(object):
    # Variables are set globally
    sifsDuration = 10E-6
    maximumACKDuration = 13.125E-6

    txopLimit = 1024E-6
    singleReceiver = False
    """ when True, continuous TXOP operation, else TXOP has to be triggered with startTXOP() """
    maxOutTXOP = False
    """ when True, set NAV to cover the rest of the TXOP window, regardless of next frame transmission duration """

    
class TXOP(openwns.Probe.Probe):

    __plugin__ = 'wimemac.lowerMAC.TXOP'
    """ Name in FU Factory """

    logger = None

    protocolCalculatorName = None
    managerName = None
    #raName = None
    txopWindowName = None
    myConfig = None

    TXOPDurationProbeName = None

    def __init__(self, functionalUnitName, commandName, managerName, probePrefix, protocolCalculatorName, txopWindowName, config, parentLogger = None, **kw):
        super(TXOP, self).__init__(name = functionalUnitName, commandName = commandName)
        self.managerName = managerName
        self.protocolCalculatorName = protocolCalculatorName
        self.txopWindowName = txopWindowName
        #self.raName = raName
        assert(config.__class__ == TXOPConfig)
        self.myConfig = config
        self.logger = wimemac.Logger.Logger(name = "TXOP", parent = parentLogger)
        self.TXOPDurationProbeName = probePrefix + '.duration'
        openwns.pyconfig.attrsetter(self, kw)

