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

class ManagerConfig(object):
    sifsDuration = 10E-6 #16E-6
    expectedACKDuration = 13.125E-6

    msduLifetimeLimit = 0
    """ maximum msdu lifetime, set to zero for unlimited lifetime """

    reservationBlocks = 1
    useRandomPattern = False
    useRateAdaptation = False
    useDRPchannelAccess = True
    usePCAchannelAccess = False

class Manager(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wimemac.lowerMAC.Manager'

    logger = None
    myMACAddress = None
    phyDataTransmission = None
    phyNotification = None
    phyCarrierSense = None
    phyUserName = None
    drpSchedulerName = None
    errorModellingName = None
    protocolCalculatorName = None
    channelStateName = None
    upperConvergenceCommandName = None
 
    myConfig = None

    def __init__(self,
             functionalUnitName,
             commandName,
             phyUserName,
             channelStateName,
             upperConvergenceCommandName,
             drpSchedulerName,
             errorModellingName,
             protocolCalculatorName,
             config,
             macaddress,
             parentLogger = None, **kw):
        super(Manager, self).__init__(functionalUnitName=functionalUnitName,
                              commandName=commandName)
        self.logger = wimemac.Logger.Logger(name = "Manager", parent = parentLogger)
        self.phyUserName = phyUserName
        self.channelStateName = channelStateName
        self.upperConvergenceCommandName = upperConvergenceCommandName
        self.drpSchedulerName = drpSchedulerName
        self.errorModellingName = errorModellingName
        self.protocolCalculatorName = protocolCalculatorName

        assert(config.__class__ == ManagerConfig)
        self.myConfig = config

        openwns.pyconfig.attrsetter(self, kw)

    def setMACAddress(self, address):
        if self.myMACAddress is not None: raise AssertionError, "Do you really want to re-set the MACAddress?"
        self.myMACAddress = address

    def setPhyDataTransmission(self, serviceName):
        self.phyDataTransmission = serviceName

    def setPhyNotification(self, serviceName):
        self.phyNotification = serviceName

    def setPhyCarrierSense(self, serviceName):
        self.phyCarrierSense = serviceName


