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

import wimemac.Logger


from openwns import dB, fromdB

class FrameSynchronizationConfig(object):
    # idle Capture Threshold, >= dB(0) for OFDM
    idleCapture = dB(0)
    # SenderLastClear Capture Threshold, >= idleCapture
    slcCapture = dB(10)
    # SenderLastGarbled Capture Threshold, >= slcCapture
    slgCapture = dB(18)
    # Below this threshold, no preamble will even be detected
    detectionThreshold = dB(-5)

    # optionally, the frameSync can signal all rxErrors, even if we were not synchronized to the frame!
    signalRxErrorAlthoughNotSynchronized = False

class FrameSynchronization(openwns.Probe.Probe):
    """ This FU is responsible for passing only correctly received preambles and frames, i.e.
        * With no CRC error as set by the crc, and
        * With a valid preamble in case of a frame/psdu
    """
    __plugin__ = 'wimemac.convergence.FrameSynchronization'

    logger = None

    myConfig = None

    managerName = None
    crcCommandName = None
    phyUserCommandName = None

    """ Preset values are according to Lee et al., "An Experimental Study on the Capture Effect in 802.11a" """
    def __init__(self,
             name,
             commandName,
             managerName,
             crcCommandName,
             config,
             parentLogger=None, **kw):
             super(FrameSynchronization, self).__init__(name=name, commandName=commandName)

        # Check consistency of config
             assert(config.__class__ == FrameSynchronizationConfig)
             assert(fromdB(config.detectionThreshold) <= fromdB(config.idleCapture))
             assert(0 <= fromdB(config.idleCapture))
             assert(fromdB(config.idleCapture) < fromdB(config.slcCapture))
             assert(fromdB(config.slcCapture) <= fromdB(config.slgCapture))
             self.myConfig = config

             self.logger = wimemac.Logger.Logger(name = "FrameSync", parent = parentLogger)

             self.managerName = managerName
             self.crcCommandName = crcCommandName
         
             openwns.pyconfig.attrsetter(self, kw)

