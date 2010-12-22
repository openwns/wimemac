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

import openwns.FCF


class BeaconCollector(openwns.FCF.CompoundCollector):
    __plugin__ = "wimemac.frame.BeaconCollector"
    duration = None
    BeaconSlotDuration = None
    managerName = None

    def __init__(self, name, duration, beaconSlot, beaconSlotDuration, managerName, parentLogger = None):
        super(BeaconCollector, self).__init__(name)
        self.duration = duration
        self.BeaconSlot = beaconSlot
        self.BeaconSlotDuration = beaconSlotDuration
        self.managerName = managerName
        self.logger = openwns.logger.Logger("wimemac", "BeaconCollector", True, parentLogger)
        self.logger.level = 2

class DataCollector(openwns.FCF.CompoundCollector):
    __plugin__ = "wimemac.frame.DataCollector"
    duration = None

    def __init__(self, name, duration, parentLogger = None):
        super(DataCollector, self).__init__(name)
        self.duration = duration
        self.logger = openwns.logger.Logger("wimemac", "DataCollector", True, parentLogger)
        self.logger.level = 2


