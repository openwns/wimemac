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

import wimemac.support.Layer1Config
import openwns.pyconfig
from openwns import dBm, dB

import wimemac.Wimemac

class Basic(object):
    position = None

    layer1 = None
    layer2 = None

    def __init__(self, frequency):
        self.layer1 = wimemac.support.Layer1Config.Basic(frequency, txPower = dBm(-14))
        self.layer2 = wimemac.Wimemac.Config(frequency)

class Station(Basic):
    def __init__(self, frequency, position):
        super(Station, self).__init__(frequency)
        self.position = position






