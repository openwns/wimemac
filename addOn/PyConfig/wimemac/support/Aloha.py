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

class AlohaComponent(wimemac.wimemac.Component):
    """Component with Aloha MAC

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a
    Stop-and-Wait ARQ and CRC in order to throw away broken
    packets. Furthermore a Aloha MAC 'controls' the medium access"""

    arq = None

    def __init__(
        self,
        node,
        name,
        phyDataTransmission,
        phyNotification,
        arqResendTimeout = 0.01,
        alohaWaitingTime = 0.05,
        bufferSize = 500*1024*8):

        super(AlohaComponent, self).__init__(node, name, phyDataTransmission, phyNotification, blocking=False)
        # create Buffer, ARQ and CRC
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(sizeUnit = 'Bit', size = bufferSize, lossRatioProbeName = 'wimemac.unicastBufferLoss', sizeProbeName = 'wimemac.unicastBufferSize'))
        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(sizeUnit = 'Bit', size = bufferSize, lossRatioProbeName = 'wimemac.broadcastBufferLoss', sizeProbeName = 'wimemac.broadcastBufferSize'))
        self.arq = wns.FUN.Node("arq", wns.ARQ.StopAndWait(parentLogger = self.logger, resendTimeout = arqResendTimeout))
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='wimemac.crcLoss'))
        alohaMAC = wimemac.wimemac.Aloha(commandName = "alohaMAC", maximumWaitingTime = alohaWaitingTime, parentLogger = self.logger)
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(self.arq)
        self.fun.add(crc)
        self.fun.add(alohaMAC)

        # connect unicast path
        self.unicastUpperConvergence.connect(self.bottleNeckDetective)
        self.bottleNeckDetective.connect(unicastBuffer)
        unicastBuffer.connect(self.arq)
        self.arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(alohaMAC)
        alohaMAC.connect(self.lowerConvergence)


