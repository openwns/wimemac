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

from openwns import dB

class BaseClass(object):
    receiverNoiseFigure = None
    frequency = None
    txPower = None
    bandwidth = None
    numberOfSubCarrier = None
    txrxTurnaroundDelay = None

class Basic(BaseClass):
    def __init__(self, frequency, txPower):
        self.frequency = frequency
        self.txPower = txPower

        self.bandwidth = 528
        self.numberOfSubCarrier = 1
        self.receiverNoiseFigure = dB(5)
        self.txrxTurnaroundDelay = 1E-6


