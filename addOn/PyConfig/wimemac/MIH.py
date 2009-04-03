import wns.PyConfig
import glue.Glue
import wns.FUN
from wns.PyConfig import attrsetter

class MeasurementsMonitor(wns.FUN.Node):

    __plugin__ = 'glue.mih.MeasurementsMonitor'
    """Name in FunctionalUnitFactory"""

    name='MeasurementsMonitor'

    address = None

    berProvider = "none"

    linkDetectedThreshold = None

    linkDownThreshold = None

    linkGoingDownThreshold = None

    periodicTimeoutPeriod = None

    periodicTimeoutDelay = None

    linkEventMeasurementWindow = None

    berLevelProbeName = None

    useBERLevelProbe = True

    probeNamePrefix = None

    linkDetectedProbeName = None

    useLinkDetectedProbe = True

    linkDownProbeName = None

    useLinkDownProbe = True

    logger = None
    """Logger configuration"""



    def __init__(self, berProvider, prefix, address, parentLogger = None, **kw):
        self.address = address
	self.berProvider = berProvider
        self.probeNamePrefix = prefix
        self.berLevelProbeName = prefix + "berLevel"
        self.linkDetectedProbeName = self.probeNamePrefix + "linkDetectedTriggerLevel"
        self.linkDownProbeName = self.probeNamePrefix + "linkDownTriggerLevel"
	self.logger = glue.Glue.Logger('MeasurementsMonitor', True, parentLogger)
	attrsetter(self, kw)

class LinkCommandProcessor(wns.FUN.Node):

    __plugin__ = 'glue.mih.LinkCommandProcessor'
    """Name in FunctionalUnitFactory"""

    logger = None
    """Logger configuration"""

    def __init__(self, parentLogger = None):
        self.logger = glue.Glue.Logger('LinkCommandProcessor', True, parentLogger)

class CapabilityDiscoveryProvider(wns.FUN.Node):

    __plugin__ = 'glue.mih.CapabilityDiscoveryProvider'
    """Name in FunctionalUnitFactory"""

    address = None

    dllTechnologyName = None

    logger = None
    """Logger configuration"""
    
    #Supported Events List
    linkUp = True
    linkDown = True
    linkDetected = True
    linkGoingDown = True

    def __init__(self, _address, _dllTechnologyName, parentLogger = None):
        self.address = _address
        self.dllTechnologyName = _dllTechnologyName
        self.logger = glue.Glue.Logger('LinkCommandProcessor', True, parentLogger)
