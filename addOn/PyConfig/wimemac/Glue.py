import wns.Module
import wns.PyConfig
import wns.Node
import wns.Buffer
import wns.ARQ
import wns.CRC
import wns.Probe
import wns.FUN
import wns.Logger
import wns.SAR
import wns.Tools
import wns.Multiplexer
import wns.ARQ

import glue.Reconfiguration
import glue.MIH
import glue.evaluation

class Logger(wns.Logger.Logger):
    """A special Logger for GLUE

    The Logger's Module name is set to GLUE"""

    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("GLUE", name, enabled, parent, **kw)


class Glue(wns.Module.Module):
    """GLUE Module Configuration

    Needed in order to load this Module by WNS."""
    def __init__(self):
        # The probes config might disappear with pyconfig probes ...
        super(Glue, self).__init__("Glue", "glue")

class Component(wns.Node.Component):
    """Represents a generic data link layer in a wns.Node.Node"""

    nameInComponentFactory = 'glue.Component'
    """In C++ the node will ask the ComponentFactory with this name
    to build a Component (of this special type)"""

    logger = None
    """Logger configuration"""

    loggerEnabled = True
    """Logger enabled/disabled"""

    unicastDataTransmission = None
    """FQSN to the unicastDataTransmission service I'm offering """

    broadcastDataTransmission = None
    """FQSN to the broadcastDataTransmission service I'm offering """

    unicastNotification = None
    """FQSN to the unicastNotification service I'm offering """

    broadcastNotification = None
    """FQSN to the broadcastNotification service I'm offering """

    phyDataTransmission = None
    """FQSN (Fully Qualified Service Name) of a physical layer to be
    used for communication"""

    phyNotification = None
    """FQSN (Fully Qualified Service Name) of a physical layer
    to be used for notification"""

    fun = None
    """Functional Unit Network"""

    unicastUpperConvergence = None
    """This is a special Functional Unit in the 'fun' which is used to
    communicate with higher layers."""

    broadcastUpperConvergence = None
    """This is a special Functional Unit in the 'fun' which is used to
    communicate with higher layers."""

    dispatcher = None

    lowerConvergence = None
    """ This is a special Functional Unit in the 'fun' which is used
    to communicate with the physical layer. This Functional Unit must
    fit to the physical layer. It is expected that this Functional
    Unit can be exchanged in order to use another physical layer and
    other parts of the protocol stack can remain"""

    bottleNeckDetective = None
    """ Prints messages showing where the Compounds in a FUN a
    currently located """

    layerName = None
    """(msg) Can this be removed (read name instead)?"""

    address = None

    nextAddress = 1

    def __init__(self, node, name, phyDataTransmission, phyNotification, blocking = True, **kw):
        """ Create Logger and FUN.

        The FUN created here is initialized with three Functional Units:
        lowerConvergence, unicastUpperConvergence and broadcastUpperConvergence. By default
        lowerConvergence is set to Lower2Copper(), unicastUpperConvergence
        is set to UnicastUpperConvergence() and broadcastUpperConvergence is set to
        BroadcastUpperConvergence(). _node is set as Loggers parent."""

        super(Component, self).__init__(node, name)
        self.logger = Logger("Glue", self.loggerEnabled, node.logger)
        self.logger.level = 2
        self.address = Component.nextAddress
        Component.nextAddress += 1
        self.phyDataTransmission = phyDataTransmission
        self.phyNotification = phyNotification

        self.fun = wns.FUN.FUN()
        self.unicastUpperConvergence = wns.FUN.Node("unicastUpperConvergence", UnicastUpperConvergence(self.logger, self.loggerEnabled))
        self.broadcastUpperConvergence = wns.FUN.Node("broadcastUpperConvergence", BroadcastUpperConvergence(self.logger, self.loggerEnabled))
        self.dispatcher = wns.FUN.Node("dispatcher", wns.Multiplexer.Dispatcher(opcodeSize = 1, parentLogger=self.logger))
        self.lowerConvergence = wns.FUN.Node(
            "lowerConvergence",
            Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = blocking,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))

        self.bottleNeckDetective = wns.FUN.Node("bottleNeckDetective", wns.Tools.BottleNeckDetective(0.0, 1.0, self.logger))

        self.unicastDataTransmission = name + ".dllUnicastDataTransmission"
        self.unicastNotification = name + ".dllUnicastNotification"

        self.broadcastDataTransmission = name + ".dllBroadcastDataTransmission"
        self.broadcastNotification = name + ".dllBroadcastNotification"

        self.layerName = name

        wns.PyConfig.attrsetter(self, kw)
        # placed after the attrsetter in order to allow
        # unicastUpperConvergence, broadcastUpperConvergence and lowerConvergence to be set from
        # constructor
        self.fun.add(self.unicastUpperConvergence)
        self.fun.add(self.broadcastUpperConvergence)
        self.fun.add(self.dispatcher)
        self.fun.add(self.lowerConvergence)
        self.fun.add(self.bottleNeckDetective)

class UpperConvergence(wns.PyConfig.Sealed):
    """(msg) should may be be renamed to Service.DataLinkLayer?"""

    logger = None
    """Logger configuration"""

    def __init__(self, parentLogger = None, enabled = True):
        self.logger = Logger("UpperConvergence", enabled, parentLogger)

class UnicastUpperConvergence(UpperConvergence):
    __plugin__ = 'glue.convergence.UnicastUpper'
    """Name in FunctionalUnitFactory"""

    def __init__(self, parentLogger = None, enabled = True):
        super(UnicastUpperConvergence, self).__init__(parentLogger, enabled)

class BroadcastUpperConvergence(UpperConvergence):
    __plugin__ = 'glue.convergence.BroadcastUpper'
    """Name in FunctionalUnitFactory"""

    def __init__(self, parentLogger = None, enabled = True):
        super(BroadcastUpperConvergence, self).__init__(parentLogger, enabled)

class Lower2Copper(wns.PyConfig.Sealed):
    """(msg) should may be be renamed to User.Copper?"""

    __plugin__ = 'glue.convergence.Lower2Copper'
    """Name in FunctionalUnitFactory"""

    unicastRouting = None
    """Functional Unit Friend: provides the peer instance to
    communicate with"""

    broadcastRouting = None
    """Functional Unit Friend: provides the peer instance to
    communicate with"""

    logger = None
    """Logger configuration"""

    blocking = None
    """ Defines whether the FU is accepting if the Copper below is
    free or not (it True, FU is not accepting if Copper is not free
    ...)

    """

    def __init__(self, unicastRouting, broadcastRouting, blocking = True, parentLogger = None, enabled = True):
        self.unicastRouting = unicastRouting
        self.broadcastRouting = broadcastRouting
        self.logger = Logger("LowerConvergence", enabled, parentLogger)
        self.blocking = blocking

class MIHComponent(Component):
    """Represents a generic data link layer plus MIH functionality"""

    nameInComponentFactory = 'glue.MIHComponent'
    """In C++ the node will ask the ComponentFactory with this name
    to build a Component (of this special type)"""

    linkEventNotification = None
    """FQSN to the Link Event Notification Service"""

    linkCommandProcessorService = None
    """FQSN to the Link Command Processor Interface"""

    capabilityDiscoveryService = None
    """FQSN to the Link Command Processor Interface"""

    measurementsMonitor = None
    """This is a Functional Unit in the 'fun' which is used to send
    Link Event Notifications to the MIH Function"""

    linkCommandProcessor = None
    """This is a Functional Unit in the 'fun' which is used to receive
    Link Commands from the MIH Function."""

    capabilityDiscoveryProvider = None
    """This is a Functional Unit in the 'fun' which is used to receive
    Link Commands from the MIH Function."""

    logger = None

    mihCapable = True

    dllTechnologyName = "Glue"

    def __init__(self, node, name, phyDataTransmission, phyNotification, **kw):
        super(MIHComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        self.logger = Logger("GlueWithMeasurementsMonitor", self.loggerEnabled, node.logger)
        self.linkEventNotification =  name + ".dllLinkEventNotification"
        self.linkCommandProcessorService =  name + ".dllLinkCommandProcessor"
        self.capabilityDiscoveryService = name + ".capabilityDiscoveryService"
        # Measurements Monitor needs a to have a BER Provider
        # In this case this is the Lower Convergence of the Glue component
        self.measurementsMonitor = wns.FUN.Node("measurementsMonitor",glue.MIH.MeasurementsMonitor("lowerConvergence", "glue.",self.address, self.logger))
        self.linkCommandProcessor =wns.FUN.Node("linkCommandProcessor",glue.MIH.LinkCommandProcessor(self.logger))
        self.capabilityDiscoveryProvider= wns.FUN.Node("capabilityDiscoveryProvider", glue.MIH.CapabilityDiscoveryProvider(self.address, self.dllTechnologyName, self.logger))
        self.fun.add(self.measurementsMonitor)
        self.fun.add(self.linkCommandProcessor)
        self.fun.add(self.capabilityDiscoveryProvider)
        wns.PyConfig.attrsetter(self, kw)

class CSMACAMAC(wns.FUN.FunctionalUnit):
    """ DON'T USE RIGHT NOW

    This FU is not tested (see CSMACATest why).
    """
    logger = None
    __plugin__ = 'glue.CSMACA'

    sifsLength = None
    slotLength = None
    ackLength = None
    stopAndWaitARQName = None
    phyNotification = None
    backoffLogger = None

    def __init__(self, commandName=None, sifsLength=16E-6, slotLength=9E-6, ackLength=44E-6, stopAndWaitARQName=None, phyNotification=None, parentLogger=None):
        super(CSMACAMAC, self).__init__(commandName=commandName)
        self.logger = Logger(name = "CSMACA", enabled = True, parent = parentLogger)
        self.backoffLogger = Logger(name = "Backoff", enabled = True, parent = self.logger)
        self.sifsLength = sifsLength
        self.slotLength = slotLength
        self.ackLength = ackLength
        self.stopAndWaitARQName = stopAndWaitARQName
        self.phyNotification = phyNotification

class StopAndWait(wns.ARQ.StopAndWait):
    """ Special version of StopAndWait for CSMACAMAC """

    __plugin__ = 'glue.StopAndWait'
    """ Name in FU Factory """

    phyDataTransmissionFeedback = None
    """ The name of the phy service telling us when a transmission has taken place """

    phyNotification = None
    """ The ARQ needs to know when the PHY starts to receive the ACK frame """

    shortResendTimeout = None
    longResendTimeout = None
    
    def __init__(self, phyDataTransmissionFeedbackName, phyNotification, **kw):
        super(StopAndWait, self).__init__(**kw)
        self.phyDataTransmissionFeedback = phyDataTransmissionFeedbackName
        self.phyNotification = phyNotification

class Aloha(wns.FUN.FunctionalUnit):

    logger = None
    __plugin__ = 'glue.mac.Aloha'

    maximumWaitingTime = None
    """ Maximum time (in seconds) to wait before medium access, the
    actual time will be uniformly distributed ..."""

    def __init__(self, commandName, maximumWaitingTime = 0.01, parentLogger = None):
        super(Aloha, self).__init__(commandName=commandName)
        self.maximumWaitingTime = maximumWaitingTime
        self.logger = Logger(name = "Aloha", enabled = True, parent = parentLogger)
