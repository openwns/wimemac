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
import openwns.pyconfig

import wimemac.Logger

class DCFConfig(object):
    """ Constants for the medium access waiting times """
    slotDuration = 9E-6
    """ PHY-dependent, see 9.2.10"""
    aifsDuration = 2*9E-6
    """ PHY-dependent, see 9.2.10"""
    eifsDuration = (2*9.0+10.0+13.125)*1E-6 # AIFS + SIFS + ACK
    """ arbitration interframe space, see 9.2.3.4 """
    cwMin = None
    """ minimum size of the contention window """
    cwMax = None
    """ maximum size of the contention window """
    backoffDisabled = False
    """ disable BO, usefull e.g. for the broadcast-BO in STAs """

    def __init__(self, cwMin = 7, cwMax = 511):
        self.cwMin = cwMin
        self.cwMax = cwMax

class DCF(openwns.FUN.FunctionalUnit):
    """ The basic 802.11 Distributed Control Function (DCF),
        based on an exponential backoff
    """
    __plugin__ = 'wimemac.lowerMAC.timing.DCF'

    csName = None
    """ Name of the channel state FU """
    rxStartEndName = None
    """ Name of the rxStartEnd indicator """
    #arqCommandName = None
    #""" Name of the arq command which contains the local transmission counter """

    drpSchedulerName = None

    myConfig = None

    logger = None
    backoffLogger = None

    def __init__(self, functionalUnitName, commandName, csName, rxStartEndName, drpSchedulerName, config, parentLogger=None, **kw):
        super(DCF, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)
        self.logger = wimemac.Logger.Logger(name = functionalUnitName, parent = parentLogger)
        self.backoffLogger = wimemac.Logger.Logger(name = "Backoff", parent = self.logger)
        assert(config.__class__ == DCFConfig)
        self.myConfig = config
        self.csName = csName
        self.rxStartEndName = rxStartEndName
        self.drpSchedulerName = drpSchedulerName
        #self.arqCommandName = arqCommandName
        openwns.pyconfig.attrsetter(self, kw)
