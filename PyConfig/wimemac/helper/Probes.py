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

import openwns.Probe
import wimemac.Logger

################################
# Special Probes for the WiMeMAC
################################



class ForwardingE2EWindowProbe(openwns.Probe.WindowProbeBus):
    __plugin__ = 'wimemac.helper.ForwardingE2EWindowProbe'
    aggregatedBitThroughputProbeName = None
    outgoingBitThroughputProbeName = None
    incomingBitThroughputProbeName = None
    upperConvergenceName = None

    """ Required to read the hop-count """
    forwardingCommandName = None
    upperConvergenceName = None
    
    def __init__(self, name, prefix, commandName,upperConvergenceName, forwardingCommandName, windowSize = 1.0, sampleInterval = None, parentLogger = None, moduleName = 'WiMeMAC', **kw):
        super(openwns.Probe.WindowProbeBus, self).__init__(name, prefix, commandName, windowSize, sampleInterval, parentLogger, moduleName, **kw)
        self.forwardingCommandName = forwardingCommandName
        self.upperConvergenceName = upperConvergenceName
        self.aggregatedBitThroughputProbeName = prefix + ".window.aggregated.bitThroughput"
        self.aggregatedBitThroughputRelativeProbeName = prefix + ".window.aggregated.bitThroughput.relative"
        self.outgoingBitThroughputProbeName = prefix + ".window.outgoing.bitThroughput"
        self.incomingBitThroughputProbeName = prefix + ".window.incoming.bitThroughput"
        self.logger = openwns.logger.Logger("wimemac", "ForwardingE2EWindowProbe", True, parentLogger)
        self.logger.level = 2
	


