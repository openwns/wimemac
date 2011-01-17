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

def installEvaluation(sim, loggingStations):

    sourceName = 'wimemac.ARQTransmissionAttempts'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Transmission attempts',
                            minXValue = 0.0,
                            maxXValue = 15.0,
                            resolution = 14)) 

    sourceName = 'wimemac.timeBufferEmpty'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Transmission attempts',
                            minXValue = 0.0,
                            maxXValue = 0.02,
                            resolution = 20000)) 

    sourceName = 'wimemac.packetErrorRate'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Packet error rate',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 1000)) 

    sourceName = 'wimemac.crcLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Loss ratio in CRC',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 1000)) 

    sourceName = 'wimemac.unicastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Loss ratio in unicast buffer',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 1000)) 

    sourceName = 'wimemac.broadcastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Loss ratio in broadcast buffer',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 1000)) 

    sourceName = 'wimemac.unicastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Unicast buffer size',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 20)) 

    sourceName = 'wimemac.broadcastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                            description = 'Broadcast buffer size',
                            minXValue = 0.0,
                            maxXValue = 1.0,
                            resolution = 20)) 

    for where in [ 'unicastTop', 'broadcastTop', 'bottom' ]:
        for direction in [ 'incoming', 'outgoing', 'aggregated' ]:
            for what in [ 'bit', 'compound' ]:

                sourceName = 'wimemac.%s.window.%s.%sThroughput' % (where, direction, what)
                node = openwns.evaluation.createSourceNode(sim, sourceName)
                node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
                node.getLeafs().appendChildren(TimeSeries())

    for where in [ 'unicastTop', 'broadcastTop' ]:

        sourceName = 'wimemac.%s.packet.incoming.delay' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet delay (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 0.001,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.outgoing.delay' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet delay (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 0.001,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.incoming.bitThroughput' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming bit throughput (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 800000000.0,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.incoming.size' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet size (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 15000.0,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.outgoing.size' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet size (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 15000.0,
                                resolution = 1000)) 

    for where in [ 'bottom' ]:

        sourceName = 'wimemac.%s.packet.incoming.delay' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet delay (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 0.000001,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.outgoing.delay' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet delay (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 0.000001,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.incoming.bitThroughput' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming bit throughput (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 1000000000.0,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.incoming.size' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet size (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 16000.0,
                                resolution = 1000)) 

        sourceName = 'wimemac.%s.packet.outgoing.size' % where
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
        node.getLeafs().appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet size (%s)' % where,
                                minXValue = 0.0,
                                maxXValue = 16000.0,
                                resolution = 1000)) 

def installMIHEvaluation(sim, loggingStations):

    sourceName = 'wimemac.linkDetectedTriggerLevel'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
    node.getLeafs().appendChildren(TimeSeries())

    sourceName = 'wimemac.linkDownTriggerLevel'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
    node.getLeafs().appendChildren(TimeSeries())

    sourceName = 'wimemac.berLevel'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
    node.getLeafs().appendChildren(TimeSeries())
