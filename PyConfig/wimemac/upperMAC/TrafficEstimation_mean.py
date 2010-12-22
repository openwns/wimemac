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

import openwns.logger

class TrafficEstimation_mean(openwns.FUN.FunctionalUnit):

    managerName = None
    logger = None
    overWriteEstimation = None
    CompoundspSF = None
    BitspSF = None
    MaxCompoundSize = None
    averageOverSFs = None
    
    __plugin__ = 'wimemac.upperMAC.TrafficEstimation_mean'

    def __init__(self, managerName, averageOverSFs = 10, CompoundspSF = 0, BitspSF = 0, MaxCompoundSize = 0, overWriteEstimation = False, parentLogger = None):
        super(TrafficEstimation_mean, self).__init__(commandName=None)
        self.managerName = managerName
        self.CompoundspSF = CompoundspSF
        self.BitspSF = BitspSF
        self.MaxCompoundSize = MaxCompoundSize
        self.averageOverSFs = averageOverSFs
        self.overWriteEstimation = overWriteEstimation
        self.logger = openwns.logger.Logger("wimemac", "TrafficEstimation_mean", True, parentLogger)
        self.logger.level = 2
	    

