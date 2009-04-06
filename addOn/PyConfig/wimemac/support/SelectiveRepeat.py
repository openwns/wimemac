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

import wimemac.Reconfiguration
import wimemac.wimemac
import wimemac.Trigger
import wimemac.Routing
import wimemac.BERMeasurementReporting

class SelectiveRepeatARQ(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SelectiveRepeatARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.broadcastBufferLoss', sizeProbeName = 'wimemac.broadcastBufferSize'))
        unicastTopWindowProbe = wns.FUN.Node("unicastTopWindowProbe", wns.Probe.Window("wimemac.unicastTopWindowProbe", "wimemac.unicastTop", windowSize=.25))
        unicastTopDelayProbe = wns.FUN.Node("unicastTopDelayProbe", wns.Probe.Packet("wimemac.unicastTopDelayProbe", "wimemac.unicastTop"))
        broadcastTopWindowProbe = wns.FUN.Node("broadcastTopWindowProbe", wns.Probe.Window("wimemac.broadcastTopWindowProbe", "wimemac.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = wns.FUN.Node("broadcastTopDelayProbe", wns.Probe.Packet("wimemac.broadcastTopDelayProbe", "wimemac.broadcastTop"))
        arq = wns.FUN.Node("arq", wns.ARQ.SelectiveRepeat(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='wimemac.crcLoss'))
        bottomWindowProbe = wns.FUN.Node("bottomWindowProbe", wns.Probe.Window("wimemac.bottomWindowProbe", "wimemac.bottom", windowSize=.25))
        bottomDelayProbe = wns.FUN.Node("bottomDelayProbe", wns.Probe.Packet("wimemac.bottomDelayProbe", "wimemac.bottom"))
        # add
        self.fun.add(unicastBuffer)
        self.fun.add(unicastTopWindowProbe)
        self.fun.add(unicastTopDelayProbe)
        self.fun.add(broadcastBuffer)
        self.fun.add(broadcastTopWindowProbe)
        self.fun.add(broadcastTopDelayProbe)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(unicastTopWindowProbe)
        unicastTopWindowProbe.connect(unicastTopDelayProbe)
        unicastTopDelayProbe.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(broadcastTopWindowProbe)
        broadcastTopWindowProbe.connect(broadcastTopDelayProbe)
        broadcastTopDelayProbe.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQ(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        unicastTopWindowProbe = wns.FUN.Node("unicastTopWindowProbe", wns.Probe.Window("wimemac.unicastTopWindowProbe", "wimemac.unicastTop", windowSize=.25))
        unicastTopDelayProbe = wns.FUN.Node("unicastTopDelayProbe", wns.Probe.Packet("wimemac.unicastTopDelayProbe", "wimemac.unicastTop"))
        unicastSar = wns.FUN.Node("unicastSar", wns.SAR.Fixed(_sarFragmentSize))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.broadcastBufferLoss', sizeProbeName = 'wimemac.broadcastBufferSize'))
        broadcastTopWindowProbe = wns.FUN.Node("broadcastTopWindowProbe", wns.Probe.Window("wimemac.broadcastTopWindowProbe", "wimemac.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = wns.FUN.Node("broadcastTopDelayProbe", wns.Probe.Packet("wimemac.broadcastTopDelayProbe", "wimemac.broadcastTop"))
        broadcastSar = wns.FUN.Node("broadcastSar", wns.SAR.Fixed(_sarFragmentSize))
        arq = wns.FUN.Node("arq", wns.ARQ.SelectiveRepeat(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='wimemac.crcLoss'))
        bottomWindowProbe = wns.FUN.Node("bottomWindowProbe", wns.Probe.Window("wimemac.bottomWindowProbe", "wimemac.bottom", windowSize=.25))
        bottomDelayProbe = wns.FUN.Node("bottomDelayProbe", wns.Probe.Packet("wimemac.bottomDelayProbe", "wimemac.bottom"))
        # add
        self.fun.add(unicastBuffer)
        self.fun.add(unicastTopWindowProbe)
        self.fun.add(unicastTopDelayProbe)
        self.fun.add(unicastSar)
        self.fun.add(broadcastBuffer)
        self.fun.add(broadcastTopWindowProbe)
        self.fun.add(broadcastTopDelayProbe)
        self.fun.add(broadcastSar)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(unicastTopWindowProbe)
        unicastTopWindowProbe.connect(unicastTopDelayProbe)
        unicastTopDelayProbe.connect(unicastSar)
        unicastSar.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(broadcastTopWindowProbe)
        broadcastTopWindowProbe.connect(broadcastTopDelayProbe)
        broadcastTopDelayProbe.connect(broadcastSar)
        broadcastSar.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQTrigger(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQTrigger, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        buffer = wns.FUN.Node("buffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        topWindowProbe = wns.FUN.Node("topWindowProbe", wns.Probe.Window("wimemac.topWindowProbe", "wimemac.unicastTop", windowSize=.25))
        topDelayProbe = wns.FUN.Node("topDelayProbe", wns.Probe.Packet("wimemac.topDelayProbe", "wimemac.unicastTop"))
        sar = wns.FUN.Node("sar", wns.SAR.Fixed(_sarFragmentSize))
        arq = wns.FUN.Node("arq", wns.ARQ.SelectiveRepeat(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='wimemac.crcLoss'))
        bottomWindowProbe = wns.FUN.Node("bottomWindowProbe", wns.Probe.Window("wimemac.bottomWindowProbe", "wimemac.bottom", windowSize=.25))
        bottomDelayProbe = wns.FUN.Node("bottomDelayProbe", wns.Probe.Packet("wimemac.bottomDelayProbe", "wimemac.bottom"))
        trigger = wns.FUN.Node("trigger", wimemac.Trigger.Trigger("lowerConvergence", "sar"))
        # add
        self.fun.add(buffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(sar)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(trigger)
        # connect
        self.unicastUpperConvergence.connect(buffer)
        buffer.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(sar)
        sar.connect(arq)
        arq.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQTriggerReporting(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQTriggerReporting, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        # control plane
        trigger = wns.FUN.Node("trigger", wimemac.Trigger.Trigger("BERMeasurementReporting", "sar"))
        berMeasurementReporting = wns.FUN.Node("BERMeasurementReporting", wimemac.BERMeasurementReporting.BERMeasurementReporting("lowerConvergence"))

        # user plane
        buffer = wns.FUN.Node("buffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        topWindowProbe = wns.FUN.Node("topWindowProbe", wns.Probe.Window("wimemac.topWindowProbe", "wimemac.unicastTop", windowSize=.25))
        topDelayProbe = wns.FUN.Node("topDelayProbe", wns.Probe.Packet("wimemac.topDelayProbe", "wimemac.unicastTop"))
        sar = wns.FUN.Node("sar", wns.SAR.Fixed(_sarFragmentSize))
        arq = wns.FUN.Node("arq", wns.ARQ.SelectiveRepeat(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))

        planeDispatcher = wns.FUN.Node("planeDispatcher", wns.Multiplexer.Dispatcher(1))
        routing = wns.FUN.Node("routing", wimemac.Routing.Routing("unicastUpperConvergence", allowRouteChange = True))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='wimemac.crcLoss'))
        bottomWindowProbe = wns.FUN.Node("bottomWindowProbe", wns.Probe.Window("wimemac.bottomWindowProbe", "wimemac.bottom", windowSize=.25))
        bottomDelayProbe = wns.FUN.Node("bottomDelayProbe", wns.Probe.Packet("wimemac.bottomDelayProbe", "wimemac.bottom"))

        # add
        # control plane
        self.fun.add(trigger)
        self.fun.add(berMeasurementReporting)

        # user plane
        self.fun.add(buffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(sar)
        self.fun.add(arq)

        self.fun.add(planeDispatcher)
        self.fun.add(routing)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)

        # connect
        # control plane
        trigger.connect(berMeasurementReporting)
        berMeasurementReporting.connect(planeDispatcher)

        # user plane
        self.unicastUpperConvergence.connect(buffer)
        buffer.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(sar)
        sar.connect(arq)
        arq.connect(planeDispatcher)

        planeDispatcher.connect(routing)
        routing.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)
