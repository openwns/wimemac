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

class CSMACAComponent(glue.Glue.Component):
    """Component with CSMA/CA MAC

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a
    Stop-and-Wait ARQ and CRC in order to throw away broken
    packets. Furthermore a CSMA/CA MAC controls the medium access"""

    arq = None

    def __init__(self, node, name, phyDataTransmission, phyNotification, phyDataTransmissionFeedbackName, bufferSize = 500*1024*8):
        super(CSMACAComponent, self).__init__(node, name, phyDataTransmission, phyNotification, blocking=False)
        # probes
        perProbe = wns.Probe.ErrorRate(
            name = "errorRate",
            prefix = "glue.packet",
            errorRateProvider = "lowerConvergence",
            commandName = "packetErrorRate")

        # create Buffer, ARQ and CRC
        unicastBuffer = wns.FUN.Node("unicastBuffer", wns.Buffer.Dropping(
            sizeUnit = 'Bit',
            size = bufferSize,
            lossRatioProbeName = 'glue.unicastBufferLoss',
            sizeProbeName = 'glue.unicastBufferSize'))

        broadcastBuffer = wns.FUN.Node("broadcastBuffer", wns.Buffer.Dropping(
            sizeUnit = 'Bit',
            size = bufferSize,
            lossRatioProbeName = 'glue.broadcastBufferLoss',
            sizeProbeName = 'glue.broadcastBufferSize'))

        self.arq = wns.FUN.Node("arq", glue.Glue.StopAndWait(
            phyDataTransmissionFeedbackName = phyDataTransmissionFeedbackName,
            phyNotification = phyNotification,
            parentLogger = self.logger,
            # We wait at least SIFS+SlotTime
            shortResendTimeout = 25E-6,
            longResendTimeout = 44E-6))

        # CRC with 32 Bit (802.11)
        crc = wns.FUN.Node("crc", wns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss', CRCsize = 4*8))
        # 24 Byte header (802.11)
        overhead = wns.Tools.Overhead(overhead = 24*8, commandName = "overhead")
        csmaCAMAC = glue.Glue.CSMACAMAC(commandName = "csmaCAMAC", stopAndWaitARQName = self.arq.commandName, phyNotification = self.phyNotification, parentLogger = self.logger)
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(self.arq)
        self.fun.add(crc)
        self.fun.add(csmaCAMAC)
        self.fun.add(overhead)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(self.arq)
        self.arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(csmaCAMAC)
        csmaCAMAC.connect(overhead)
        overhead.connect(self.lowerConvergence)
