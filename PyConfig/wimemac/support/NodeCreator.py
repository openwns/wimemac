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

import openwns.node
import openwns.pyconfig
from openwns import dBm, dB

import ofdmaphy.Station

import wimemac.Logger
import wimemac.Wimemac
import wimemac.support.Transceiver

import ip.Component
import ip
from ip.AddressResolver import FixedAddressResolver, VirtualDHCPResolver

import constanze.node
import rise.Mobility

import rise.scenario.Propagation

class Station(openwns.node.Node):
    load = None      # Load Generator
    tl = None        # Transport Layer
    nl = None        # Network Layer
    dll = None       # Data Link Layer
    phy = None       # Physical Layer
    mobility = None  # Mobility Component

    id = None        # Identifier

    def __init__(self, name, id):
        super(Station, self).__init__(name)
        self.id = id
        self.logger = wimemac.Logger.Logger(name, parent = None)

class NodeCreator(object):
    __slots__ = (
    'propagation',  # rise propagation object (pathloss, shadowing, fading... for node type pairs)
    'txPower'
    )

    def __init__(self, propagationConfig, **kw):
        self.propagation = rise.scenario.Propagation.Propagation()
        self.propagation.configurePair("STA", "STA", propagationConfig)
        ####
        # self.txPower = dBm(-14)

        openwns.pyconfig.attrsetter(self, kw)

    def createPhyLayer(self, propagationName, txPower, frequency, bandwidth, receiverNoiseFigure, numberOfSubCarrier, parentLogger):
        receiver = ofdmaphy.Receiver.OFDMAReceiver(propagation = self.propagation,
                               propagationCharacteristicName = propagationName,
                               receiverNoiseFigure = receiverNoiseFigure,
                               parentLogger = parentLogger )
        transmitter = rise.Transmitter.Transmitter(propagation = self.propagation,
                               propagationCharacteristicName = propagationName,
                               parentLogger = parentLogger )

        phy = ofdmaphy.Station.OFDMAStation([receiver], [transmitter], parentLogger = parentLogger)
        phy.txFrequency = frequency
        phy.rxFrequency = frequency
        phy.txPower = txPower
        phy.numberOfSubCarrier = numberOfSubCarrier
        phy.bandwidth = bandwidth
        phy.systemManagerName = "ofdma"

        return(phy)

    def createSTA ( self, idGen, config, loggerLevel, dllLoggerLevel, resendTimeout = 0.01) :
        id = idGen.next()
        newSTA = Station("STA" + str(id), id)

        # create Physical Layer
        newSTA.phy = ofdmaphy.Station.OFDMAComponent(newSTA, "PHY STA"+str(id),
                                 self.createPhyLayer(
                                         propagationName="STA",
                                         txPower = config.layer1.txPower,
                                         frequency = config.layer1.frequency,
                                         bandwidth = config.layer1.bandwidth,
                                         receiverNoiseFigure = config.layer1.receiverNoiseFigure,
                                         numberOfSubCarrier = config.layer1.numberOfSubCarrier,
                                         parentLogger = newSTA.logger),
                                 parentLogger = newSTA.logger)

        # create data link layer
        newSTA.dll = wimemac.support.Configuration.BasicDLL(
            newSTA,
            "DLL STA"+str(id),
            config.layer2,
            newSTA.phy.dataTransmission,
            newSTA.phy.notification,
            resendTimeout = resendTimeout)
	                
        newSTA.dll.setStationID ( id )
        newSTA.dll.Manager.setMACAddress(id)
        newSTA.dll.Manager.setPhyDataTransmission(newSTA.phy.dataTransmission)
        newSTA.dll.Manager.setPhyNotification(newSTA.phy.notification)
        newSTA.dll.Manager.setPhyCarrierSense(newSTA.phy.notification)

        # create network layer
        domainName = "sta" + str(id) + ".wimemac.wns.org"
        newSTA.nl = ip.Component.IPv4Component(newSTA, domainName + ".ip",domainName)
        newSTA.nl.addDLL(_name = "wimemac",
                       # Where to get my IP Address
                       _addressResolver = ip.AddressResolver.VirtualDHCPResolver("theOnlySubnet"),
                       # ARP zone
                       _arpZone = "theOnlySubnet",
                       # We can deliver locally
                       _pointToPoint = False,
                       # DLL service names
                       _dllDataTransmission = newSTA.dll.dataTransmission,
                       _dllNotification = newSTA.dll.notification)

        # create load generator
        newSTA.load = constanze.node.ConstanzeComponent ( newSTA, "constanze" )

        newSTA.mobility = rise.Mobility.Component ( node = newSTA,
                                name = "Mobility STA"+str ( id ),
                                mobility = rise.Mobility.No ( openwns.geometry.position.Position() ))
        newSTA.mobility.mobility.setCoords ( config.position )
    
        newSTA.logger.level = loggerLevel
        newSTA.dll.logger.level = dllLoggerLevel


        return newSTA

#################################

    def createRANG(self, listener, loggerLevel,
               listenerWindowSize = 1.0, listenerSampleInterval = 0.5):
        rang = Station('RANG', (256*255)-1)

        # create dll
        rang.dll = wimemac.support.Rang.RANG(rang, parentLogger = rang.logger)
        rang.dll.setStationID((256*255)-1)

        # create network layer
        rang.nl = ip.Component.IPv4Component(rang, "192.168.255.254", "192.168.255.254", probeWindow=1.0)
        rang.nl.addDLL("wime",
            # Where to get IP Adresses
            _addressResolver = FixedAddressResolver("192.168.255.254", "255.255.0.0"),
            # Name of ARP zone
            _arpZone = "theOnlyZone",
            # We can deliver locally without going to the gateway
            _pointToPoint = True,
            # Service names of DLL
            _dllDataTransmission = rang.dll.dataTransmission,
            _dllNotification = rang.dll.notification)
        rang.nl.forwarding.config.isForwarding = True

        # create load generator
        rang.load = constanze.node.ConstanzeComponent(rang, "constanze")

        rang.logger.level = loggerLevel

        if(listener):
            ipListenerBinding = constanze.node.IPListenerBinding(rang.nl.domainName, parentLogger=rang.logger)
            listener = constanze.node.Listener(rang.nl.domainName + ".listener", probeWindow = 0.1, parentLogger=rang.logger)
            rang.load.addListener(ipListenerBinding, listener)
            rang.nl.windowedEndToEndProbe.config.windowSize = listenerWindowSize
            rang.nl.windowedEndToEndProbe.config.sampleInterval = listenerSampleInterval

        return rang

    def createVARP(self, loggerLevel, name = "VARP", zone = "theOnlySubnet"):
        varp = ip.VirtualARP.VirtualARPServer(name, zone)
        varp.logger.level = loggerLevel
        return varp

    def createVDNS(self, loggerLevel, name = "VDNS", root = "ip.DEFAULT.GLOBAL"):
        vdns = ip.VirtualDNS.VirtualDNSServer(name, root)
        vdns.logger.level = loggerLevel
        return vdns

    def createVCIB(self, loggerLevel, name = "VCIB"):
        vcibs = wimemac.management.InformationBases.VirtualCababilityInformationService(name)
        vcibs.logger.level = loggerLevel
        return vcibs
