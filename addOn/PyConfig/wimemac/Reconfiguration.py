"""Reconfiguration Manager (highly EXPERIMENTAL :-)"""

from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
from wns.Logger import Logger


class SimpleManager(Sealed):
    __plugin__= 'glue.reconfiguration.SimpleManager'
    name = "SimpleManager"

    master = False
    """The master initiates and controls the reconfiguration process.
    Only one of both peer-entities may be a master."""

    reconfigurationInterval = 0.1 # seconds
    """Interval between two reconfiguration processes"""

    maxReconfigurationDelay = 0.01 # seconds
    """Max. random delay for the first reconfiguration process"""

    peerNodeName = None
    """Name of peer node"""

    reconfigurationSchemes = None
    """List of reconfiguration schemes"""

    logger = None
    """Logger configuration"""

    def __init__(self, reconfigurationSchemes, enabled = True, parentLogger = None, **kw):
        self.reconfigurationSchemes = reconfigurationSchemes
        self.logger = Logger("GLUE", "Reconfiguration_SimpleManager", enabled, parentLogger)
        attrsetter(self, kw)


class TransmittingManager(Sealed):
    __plugin__= 'glue.reconfiguration.TransmittingManager'
    name = "TransmittingManager"

    master = False
    """The master initiates and controls the reconfiguration process.
    Only one of both peer-entities may be a master."""

    reconfigurationInterval = 0.1 # seconds
    """Interval between two reconfiguration processes"""

    maxReconfigurationDelay = 0.01 # seconds
    """Max. random delay for the first reconfiguration process"""

    reconfigurationSchemes = None
    """List of reconfiguration schemes"""

    retransmissionTimeoutReconfigurationRequest = 0.05
    """Retransmission timeout for ReconfigurationRequest"""

    useOptimizedReconfigurationIfPossible = False
    """Use optimized reconfiguration mechanism if supported by new FU path configuration"""

    retransmissionTimeoutOptimizedReconfiguration = 0.05
    """Retransmission timeout for ProceedToReconfigure"""

    ctiSequenceSize = 4
    """CTI sequence size"""

    reconfigurationRequestFrameSize = 6 # bit
    """Size of a ReconfigurationRequest frame"""

    frameSize = 2 # bit
    """Size of all other frame types"""

    logger = None
    """Logger configuration"""

    def __init__(self, master, reconfigurationSchemes, enabled = True, parentLogger = None, **kw):
        self.master = master
        self.reconfigurationSchemes = reconfigurationSchemes
        self.logger = Logger("GLUE", "Reconfiguration_TransmittingManager", enabled, parentLogger)
        attrsetter(self, kw)


class SupportUpper(Sealed):
    __plugin__= 'glue.reconfiguration.SupportUpper'
    name = "SupportUpper"

    logger = None
    """Logger configuration"""

    reconfigurationManager = None
    """Reconfiguration Manager (friend)"""

    def __init__(self, reconfigurationManager, enabled = True, parentLogger = None, **kw):
        self.reconfigurationManager = reconfigurationManager
        self.logger = Logger("GLUE", "Reconfiguration_SupportUpper", enabled, parentLogger)
        attrsetter(self, kw)


class SupportLower(Sealed):
    __plugin__= 'glue.reconfiguration.SupportLower'
    name = "SupportLower"

    logger = None
    """Logger configuration"""

    ctiLogger = None
    """Logger configuration for the CTI setter"""

    useSuspendProbe = False
    """CTI Setter FU (Suspend Support)"""

    suspendProbeName = "CTISetterProbe"

    reconfigurationManager = None
    """Reconfiguration Manager (friend)"""

    drainFU = None
    """Drain FU (friend)"""

    ctiSize = 3
    """Size of the Compound Type Identifier (CTI) command"""

    def __init__(self, reconfigurationManager, drainFU, enabled = True, parentLogger = None, **kw):
        self.reconfigurationManager = reconfigurationManager
        self.drainFU = drainFU
        self.logger = Logger("GLUE", "Reconfiguration_SupportLower", enabled, parentLogger)
        self.ctiLogger = Logger("GLUE", "Reconfiguration_CTISetter", enabled, parentLogger)
        attrsetter(self, kw)


class Drain(Sealed):
    __plugin__= 'glue.reconfiguration.Drain'
    name = "Drain"

    logger = None
    """Logger configuration"""

    def __init__(self, enabled = True, parentLogger = None):
        self.logger = Logger("GLUE", "Reconfiguration_Drain", enabled, parentLogger)
