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

    if configuration.createSNRProbes == True:
        sourceName = 'wimemac.phyuser.rxPower'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'received signal power [dBm]'))

        sourceName = 'wimemac.phyuser.interferencePower'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'received interference power [dBm]'))

        sourceName = 'wimemac.phyuser.SINR'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'SINR [dB]'))

    if configuration.createChannelUsageProbe:
        sourceName = 'wimemac.drpscheduler.pcaPortion'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(PDF(minXValue = 0.0, maxXValue = 1.0, resolution=1,
                                               name = "wimemac.drpscheduler.pcaPortion",
                                               description = "Portion of the PCA Channel Access"))
                                               
        sourceName = 'wimemac.bb.numberOfMAS'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'numberOfMAS'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 1, 
                         name = "numberOfMAS_TimeSeries", 
                         description = "numberOfMAS",
                         contextKeys = []))

    if configuration.createMCSProbe:
        sourceName = 'wimemac.manager.mcs'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'PhyMode [Mb/s]'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 1, 
                         name = "PhyMode_TimeSeries", 
                         description = "PhyMode [Mb/s]",
                         contextKeys = []))

    if configuration.createPERProbe:
        sourceName = 'wimemac.errormodelling.PER'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'calculated packet error rate due to SINR'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 3, 
                         name = "PER_TimeSeries", 
                         description = "calculated packet error rate due to SINR",
                         contextKeys = []))               

    if configuration.createTimeseriesProbes == True:
        sourceName = 'wimemac.traffic.incoming.throughput'
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'throughput [Bit/s]'))    
        leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                     valuePrecision = 1, 
                     name = "TrafficpSF_TimeSeries", 
                     description = "Incoming throughput per SF [Bit/s]",
                     contextKeys = []))

