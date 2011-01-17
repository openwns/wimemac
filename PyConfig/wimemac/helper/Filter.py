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

#
# WiMeMac-specific filter for use by dll::CompoundSwitch
#

import dll.CompoundSwitch

class TxType(dll.CompoundSwitch.Filter):
    __plugin__ = "wimemac.helper.FilterTxType"
    commandName = None
    txType = None

    def __init__(self, txType, commandName, **kw):
        super(TxType, self).__init__('Type '+txType)
        self.commandName = commandName
        self.txType = txType

class TxOPType(dll.CompoundSwitch.Filter):
    __plugin__ = "wimemac.helper.FilterTxOPType"
    commandName = None
    txOPType = None

    def __init__(self, txOPType, commandName, **kw):
        super(TxOPType, self).__init__('Type '+txOPType)
        self.commandName = commandName
        self.txOPType = txOPType

