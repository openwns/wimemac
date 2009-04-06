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

class GoBackNARQ(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(GoBackNARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        unicastTopWindowProbe = wns.FUN.Node("unicastTopWindowProbe", wns.Probe.Window("wimemac.unicastTopWindowProbe", "wimemac.unicastTop", windowSize=.25))
        unicastTopDelayProbe = wns.FUN.Node("unicastTopDelayProbe", wns.Probe.Packet("wimemac.unicastTopDelayProbe", "wimemac.unicastTop"))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.broadcastBufferLoss', sizeProbeName = 'wimemac.broadcastBufferSize'))
        broadcastTopWindowProbe = wns.FUN.Node("broadcastTopWindowProbe", wns.Probe.Window("wimemac.broadcastTopWindowProbe", "wimemac.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = wns.FUN.Node("broadcastTopDelayProbe", wns.Probe.Packet("wimemac.broadcastTopDelayProbe", "wimemac.broadcastTop"))
        arq = wns.FUN.Node("arq", wns.ARQ.GoBackN(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))
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

class SARGoBackNARQ(wimemac.wimemac.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARGoBackNARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        unicastTopWindowProbe = wns.FUN.Node("unicastTopWindowProbe", wns.Probe.Window("wimemac.unicastTopWindowProbe", "wimemac.unicastTop", windowSize=.25))
        unicastTopDelayProbe = wns.FUN.Node("unicastTopDelayProbe", wns.Probe.Packet("wimemac.unicastTopDelayProbe", "wimemac.unicastTop"))
        unicastSar = wns.FUN.Node("unicastSar", wns.SAR.Fixed(_sarFragmentSize))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'wimemac.broadcastBufferLoss', sizeProbeName = 'wimemac.broadcastBufferSize'))
        broadcastTopWindowProbe = wns.FUN.Node("broadcastTopWindowProbe", wns.Probe.Window("wimemac.broadcastTopWindowProbe", "wimemac.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = wns.FUN.Node("broadcastTopDelayProbe", wns.Probe.Packet("wimemac.broadcastTopDelayProbe", "wimemac.broadcastTop"))
        broadcastSar = wns.FUN.Node("broadcastSar", wns.SAR.Fixed(_sarFragmentSize))
        arq = wns.FUN.Node("arq", wns.ARQ.GoBackN(useProbe = True, probeName = "wimemac.ARQTransmissionAttempts", resendTimeout = 0.00001))
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
        broadcastBuffer.connect(broadcastTopWindowProbe)
        broadcastTopWindowProbe.connect(broadcastTopDelayProbe)
        broadcastTopDelayProbe.connect(broadcastSar)
        broadcastSar.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)
