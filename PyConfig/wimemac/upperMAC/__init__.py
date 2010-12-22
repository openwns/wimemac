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
import openwns.FUN

from wimemac.upperMAC.TrafficEstimation_mean import TrafficEstimation_mean

names = dict()
names['trafficEstimation'] = 'TrafficEstimation_mean'

def getFUN(transceiverAddress, names, config, myFUN, logger, probeLocalIDs):
    FUs = []
    FUs.append(openwns.FUN.Node(names['trafficEstimation'], TrafficEstimation_mean(
                                                        managerName = names['manager'] + str(transceiverAddress),
                                                        averageOverSFs = config.averageOverSFs,
                                                        CompoundspSF = config.CompoundspSF,
                                                        BitspSF = config.BitspSF,
                                                        MaxCompoundSize = config.MaxCompoundSize,
                                                        overWriteEstimation = config.overWriteEstimation,
                                                        parentLogger = logger)))
    for fu in FUs:
            myFUN.add(fu)

    # connect FUs with each other
    for num in xrange(0, len(FUs)-1):
            FUs[num].connect(FUs[num+1])

    return([FUs[0], FUs[-1]])




