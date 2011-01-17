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

import wimemac.convergence
import wimemac.lowerMAC
import wimemac.upperMAC
import wimemac.frame
import wimemac.management
import wimemac.management.InformationBases
import wimemac.protocolCalculator

from openwns.pyconfig import Sealed

class Basic(Sealed):
    logger = None

    transceiverAddress = None
    probeLocalIDs = None
    names = None

    def __init__(self,
             logger,
             transceiverAddress,
             upperConvergenceName):

        self.logger = logger
        self.transceiverAddress = transceiverAddress

        self.probeLocalIDs = {}
        self.probeLocalIDs['MAC.TransceiverAddress'] = transceiverAddress

        self.names = dict()
        self.names['upperConvergence'] = upperConvergenceName
        self.names['perMIB'] = 'perMIB'
        self.names.update(wimemac.convergence.names)
        self.names.update(wimemac.frame.names)
        self.names.update(wimemac.management.names)
        self.names.update(wimemac.lowerMAC.names)
        self.names.update(wimemac.upperMAC.names)
        self.names.update(wimemac.protocolCalculator.names)

    def createUpperMAC(self, config, myFUN):
        return(wimemac.upperMAC.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createLowerMAC(self, config, myFUN):
        return(wimemac.lowerMAC.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createConvergence(self, config, myFUN):
        return(wimemac.convergence.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createFrame(self, config, myFUN):
        return(wimemac.frame.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createManagement(self, config, myFUN):
        return(wimemac.management.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createManagementServices(self, config):
        myServices = []
        myServices.append(wimemac.protocolCalculator.ProtocolCalculator(
                                        serviceName = self.names['protocolCalculator'] + str(self.transceiverAddress),
                                        config = wimemac.protocolCalculator.Config(),
                                        parentLogger = self.logger))

        myServices.append(wimemac.management.InformationBases.PER(serviceName = self.names['perMIB'] + str(self.transceiverAddress),
                                        config = wimemac.management.InformationBases.PERConfig(),
                                        parentLogger = self.logger))
        
        return(myServices)

