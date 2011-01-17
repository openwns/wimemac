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
import wimemac.convergence.PhyMode

class PhyUser(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wimemac.convergence.PhyUser'
    """Name in FunctionalUnitFactory"""

    logger = None
    initFrequency = None #3432
    initBandwidthMHz = None #528
    txrxTurnaroundDelay = None #1E-6

    managerName = None
    txDurationCommandName = None
    phyModesDeliverer = None

    def __init__(self,
             functionalUnitName,
             commandName,
             managerName,
             txDurationCommandName,
             phyconfig,
             parentLogger = None,
             enabled = True,
             #**kw
             ):
        super(PhyUser, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)
        self.logger = openwns.logger.Logger("PhyUser", enabled, parentLogger)
        self.managerName = managerName
        self.txDurationCommandName = txDurationCommandName
        self.phyModesDeliverer = wimemac.convergence.PhyMode.ECMA368(phyconfig.defPhyMode)
        self.initBandwidthMHz = phyconfig.bandwidth
        self.initFrequency = phyconfig.frequency
        self.txrxTurnaroundDelay = phyconfig.txrxTurnaroundDelay


