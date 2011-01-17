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


class DRPScheduler(openwns.FUN.FunctionalUnit):
    __plugin__ = "wimemac.drp.DRPScheduler"
    datathroughputProbeName = None
    queuesize = None,
    frameduration = None
    managerName = None
    dcfName = None
    txopName = None
    beaconBuilderName = None
    maxPER = None
    patternPEROffset = None
    isDroppingAfterRetr = None
    deleteQueues = None
    perMIBServiceName = None

    def __init__(self,functionalUnitName,queuesize,frameduration,managerName,dcfName,txopName,beaconBuilderName,
                                    maxPER, patternPEROffset,isDroppingAfterRetr,
                                    deleteQueues, perMIBServiceName,
                                    parentLogger = None, commandName = None):
        super(DRPScheduler, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)
        self.queuesize = queuesize
        self.frameduration = frameduration
        self.managerName = managerName
        self.dcfName = dcfName
        self.txopName = txopName
        self.beaconBuilderName = beaconBuilderName
        self.maxPER = maxPER
        self.patternPEROffset = patternPEROffset
        self.isDroppingAfterRetr = isDroppingAfterRetr
        self.deleteQueues = deleteQueues
        self.perMIBServiceName = perMIBServiceName
        self.logger = openwns.logger.Logger("wimemac", "DRPScheduler", True, parentLogger)
        self.logger.level = 2
        self.datathroughputProbeName = "wimemac.data"

