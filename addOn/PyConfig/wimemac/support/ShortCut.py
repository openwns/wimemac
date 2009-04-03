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

import glue.Reconfiguration
import glue.Glue
import glue.Trigger
import glue.Routing
import glue.BERMeasurementReporting

class ShortCutComponent(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size and CRC
    in order to throw away broken packets. Best used for testing of
    higher layers"""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20):
        super(ShortCutComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # create Buffer and CRC
        # These two have intentionally no probe configuration
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = bufferSize))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = bufferSize))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss', parentLogger=self.logger))
        # add Buffer and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(crc)

        # connect unicast path
        self.unicastUpperConvergence.connect(self.bottleNeckDetective)
        self.bottleNeckDetective.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(self.lowerConvergence)

class ShortCut(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(ShortCut, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.broadcastBufferLoss', sizeProbeName = 'glue.broadcastBufferSize'))
        topWindowProbe = wns.FUN.Node("topWindowProbe", wns.Probe.Window("glue.topWindowProbe", "glue.unicastTop", windowSize=.25))
        topDelayProbe = wns.FUN.Node("delayProbe", wns.Probe.Packet("glue.topDelayProbe", "glue.unicastTop"))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))
        bottomWindowProbe = wns.FUN.Node("bottomWindowProbe", wns.Probe.Window("glue.bottomWindowProbe", "glue.bottom", windowSize=.25))
        bottomDelayProbe = wns.FUN.Node("bottomDelayProbe", wns.Probe.Packet("glue.bottomDelayProbe", "glue.bottom"))
        # add
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class ShortCutComponentWithMeasurementsMonitor(glue.Glue.MIHComponent):
    """Minimalistic configuration for testing with Measurements Monitor inside

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size and CRC
    in order to throw away broken packets. Best used for testing of
    higher layers"""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20):
        super(ShortCutComponentWithMeasurementsMonitor, self).__init__(node, name, phyDataTransmission, phyNotification)
        # create Buffer and CRC
        # These two have intentionally no probe configuration
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = bufferSize))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = bufferSize))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence"))
        # add Buffer and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(crc)

        # connect unicast path
        self.unicastUpperConvergence.connect(self.bottleNeckDetective)
        self.bottleNeckDetective.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(self.lowerConvergence)

class AcknowledgedModeShortCutComponent(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a Stop-and-Wait ARQ
    and CRC in order to throw away broken packets."""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20, resendTimeout = 0.1):
        super(AcknowledgedModeShortCutComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # probes
        perProbe = wns.Probe.ErrorRate(
            name = "errorRate",
            prefix = "glue.packet",
            errorRateProvider = "lowerConvergence",
            commandName = "packetErrorRate")

        # create Buffer, ARQ and CRC
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.unicastBufferLoss',
            sizeProbeName = 'glue.unicastBufferSize'))

        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.broadcastBufferLoss',
            sizeProbeName = 'glue.broadcastBufferSize'))

        arq = wns.FUN.Node("arq", wns.ARQ.StopAndWait(resendTimeout=resendTimeout))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))

        # add probes
        self.fun.add(perProbe)
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(arq)
        self.fun.add(crc)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(perProbe)
        perProbe.connect(self.lowerConvergence)
