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
import wns.FCF

import glue.Reconfiguration
import glue.Glue
import glue.Trigger
import glue.Routing
import glue.BERMeasurementReporting
import glue.frame






class AcknowledgedModeShortCutFrame(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a Stop-and-Wait ARQ
    and CRC in order to throw away broken packets."""


    FrameBuilder = None
    BeaconCollector = None
    DataCollector = None
    DRPScheduler = None

    


    def __init__(self, node, name, phyDataTransmission, phyNotification, beaconSlot, bufferSize = 20, resendTimeout = 0.1):
        super(AcknowledgedModeShortCutFrame, self).__init__(node, name, phyDataTransmission, phyNotification)
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
        
        

        #BeaconBuilder
        self.BeaconBuilder =  wns.FUN.Node(
            "BeaconBuilder",glue.frame.BeaconBuilder("BeaconBuilder",self.logger))

        #DRPScheduler
        self.DRPScheduler = wns.FUN.Node("DRPScheduler", glue.frame.DRPScheduler("DRPScheduler",self.logger))
        
        #FrameBuilder
        self.FrameBuilder = wns.FUN.Node(
            "FrameBuilder",wns.FCF.FrameBuilder(0,wns.FCF.TimingControl(), 
             frameDuration = 256*256E-6,
             symbolDuration = 0.0))

        self.BeaconCollector = wns.FUN.Node(
            "Beacon", glue.frame.BeaconCollector("Beacon", 6*256E-6,beaconSlot,self.logger))
        
        self.DataCollector = wns.FUN.Node(
            "Data",glue.frame.DataCollector("Data", 249*256E-6, self.logger))


        #add FrameBuilder
        self.fun.add(self.FrameBuilder)
        self.fun.add(self.BeaconCollector)
        self.fun.add(self.DataCollector)
        self.fun.add(self.DRPScheduler)
        
        #add BeaconBuilder
        self.fun.add(self.BeaconBuilder)

            
        # add probes
        self.fun.add(perProbe)
        
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        


        self.fun.add(broadcastBuffer)
        self.fun.add(arq)
        self.fun.add(crc)

           
        #connectt unicast path
        #self.unicastUpperConvergence.connect(unicastBuffer)
        self.unicastUpperConvergence.connect(self.DRPScheduler)

       #unicastBuffer.connect(arq)
       # unicastBuffer.connect(self.DRPScheduler)
        
        #arq.connect(self.DRPScheduler)
        self.DRPScheduler.connect(self.dispatcher)

        #arq.connect(self.dispatcher)

        # connect broadcast path
#        self.broadcastUpperConvergence.connect(broadcastBuffer)
 #       broadcastBuffer.connect(self.dispatcher)


        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(perProbe)
        perProbe.connect(self.FrameBuilder)
 
        #perProbe.connect(self.BeaconCollector)
        self.BeaconBuilder.connect(self.BeaconCollector)
        self.BeaconCollector.connect(self.FrameBuilder)
        self.DataCollector.connect(self.FrameBuilder)
        self.FrameBuilder.connect(self.lowerConvergence)

        self.FrameBuilder.config.add(wns.FCF.BasicPhaseDescriptor("Beacon"))
        self.FrameBuilder.config.add(wns.FCF.BasicPhaseDescriptor("Data"))

