import openwns.module
import openwns.pyconfig
import openwns.node
import openwns.Buffer
import openwns.ARQ
import openwns.CRC
import openwns.Probe
import openwns.FUN
import openwns.logger
import openwns.SAR
import openwns.Tools
import openwns.Multiplexer
from openwns import dBm, dB

import wimemac.FUNModes
import wimemac.convergence
import wimemac.convergence.PhyMode

import dll.Layer2
import dll.UpperConvergence
import dll.Services

from openwns.pyconfig import Sealed

class Logger(openwns.logger.Logger):
    """A special Logger for wimemac

    The Logger's Module name is set to wimemac"""

    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("wimemac", name, enabled, parent, **kw)


class WiMeMac(openwns.module.Module):
    """wimemac Module Configuration
    Needed in order to load this Module by WNS."""
    def __init__(self):
        # The probes config might disappear with pyconfig probes ...
        super(WiMeMac, self).__init__("wimemac", "wimemac")

class Component(dll.Layer2.Layer2):
    """Represents a generic data link layer in a wns.Node.Node"""

    loggerEnabled = True
    """Logger enabled/disabled"""
    phyuser = None
    layerName = None
    """(msg) Can this be removed (read name instead)?"""

    nextAddress = 1

    def __init__(self, node, name, phyconfig, phyDataTransmission, phyNotification,blocking = True, **kw):
        super(Component, self).__init__(node, name)
        self.logger = Logger("wimemac", self.loggerEnabled, node.logger)
        
        self.nameInComponentFactory = 'wimemac.Component'
        """In C++ the node will ask the ComponentFactory with this name
        to build a Component (of this special type)"""
        
        self.stationType = "UT"
        self.ring = 3
     
        self.logger.level = 2
        self.address = Component.nextAddress
        Component.nextAddress += 1
        self.phyDataTransmission = phyDataTransmission
        self.phyNotification = phyNotification

        self.fun = openwns.FUN.FUN()

        self.upperConvergence = dll.UpperConvergence.UT(parent = self.logger, commandName = self.upperConvergenceName)
        self.layerName = name

        openwns.pyconfig.attrsetter(self, kw)
        # placed after the attrsetter in order to allow
        # unicastUpperConvergence, broadcastUpperConvergence and lowerConvergence to be set from
        # constructor
        self.fun.add(self.upperConvergence)

class Config(Sealed):
    funTemplate = None
    frequency = None
    bandwidth = 528
    txrxTurnaroundDelay = 1E-6

    numberOfStations = None
    channelModel = 2
    defPhyMode = 7
    reservationBlocks = 1
    useRandomPattern = False
    useRateAdaptation = False
    useDRPchannelAccess = True
    usePCAchannelAccess = False
    maxPER = 0.03
    patternPEROffset = 0.0
    isDroppingAfterRetr = -1
    deleteQueues = False
    averageOverSFs = 10
    overWriteEstimation = False
    CompoundspSF = 0
    BitspSF = 0
    MaxCompoundSize = 0

    def __init__(self, initFrequency):
        self.frequency = initFrequency

        self.funTemplate = wimemac.FUNModes.Basic











