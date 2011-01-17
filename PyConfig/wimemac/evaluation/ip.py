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

from openwns.evaluation import *

def installEvaluation(sim, loggingStations, configuration):

    sourceName = 'ip.endToEnd.packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
    node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="Node.id%d"))
    leafs = node.getLeafs()
    leafs.appendChildren(PDF(name = sourceName,
                            description = 'end to end packet delay [s] PDF',
                            minXValue = 0.0,
                            maxXValue = 0.1,
                            resolution = 1000))
    #leafs.appendChildren(DLRE(mode = 'g', xMin = 0.0, xMax = 0.1, intervalWidth = 0.0001,
    #                                            name = sourceName,
    #                                            description = "End-to-End Packet Delay [s] DLRE"))
    if configuration.createTimeseriesProbes == True:
        leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 6, 
                         name = "TrafficDelay_TimeSeries", 
                         description = "Delay [s]",
                         contextKeys = []))


    for direction in ['incoming', 'outgoing', 'aggregated']:
        sourceName = 'ip.%s.window.%s.bitThroughput' % ('endToEnd', direction)

        # bitThroughput of the RANG as moments probe
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = "Throughput (" + direction + ") [bit/s]"))



