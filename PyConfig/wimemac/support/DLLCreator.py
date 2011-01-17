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

import wimemac.Wimemac

class BasicDLL(wimemac.Wimemac.Component):
    """Minimalistic configuration"""
    Manager = None

    def __init__(self, node, name, config, phyDataTransmission, phyNotification, resendTimeout = 0.1):
        super(BasicDLL, self).__init__(node, name, config, phyDataTransmission, phyNotification)
       
        ###################################
        # Create FUN groups from template
        funTemplate = config.funTemplate(logger = self.logger,
                                         transceiverAddress = node.id,
                                         upperConvergenceName = self.upperConvergenceName)
        
        [upperMACTop, upperMACBottom] = funTemplate.createUpperMAC(config, self.fun)
        [lowerMACTop, lowerMACBottom, lowerMACManager] = funTemplate.createLowerMAC(config, self.fun)
        [managementTop, managementBottom] = funTemplate.createManagement(config, self.fun)
        [frameBottom, frameTopData, frameTopBeacon] = funTemplate.createFrame(config, self.fun)
        [convergenceTop, convergenceBottom] = funTemplate.createConvergence(config, self.fun)
      
        self.Manager = lowerMACManager
        self.phyuser = convergenceBottom

        ###################################
        # connect FUs with each other
        self.upperConvergence.connect(upperMACTop)
        upperMACBottom.connect(lowerMACTop)
        lowerMACBottom.connect(frameTopData)
        managementBottom.connect(frameTopBeacon)
        frameBottom.connect(convergenceTop)
          
        ##########
        # Services
        self.managementServices.extend(funTemplate.createManagementServices(config))


