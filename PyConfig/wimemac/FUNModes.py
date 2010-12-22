import wimemac.convergence
import wimemac.lowerMAC
import wimemac.upperMAC
import wimemac.frame
import wimemac.management
import wimemac.management.InformationBases
import wimemac.protocolCalculator

from openwns.pyconfig import Sealed

class Basic(Sealed):
    logger = None

    transceiverAddress = None
    probeLocalIDs = None
    names = None

    def __init__(self,
             logger,
             transceiverAddress,
             upperConvergenceName):

        self.logger = logger
        self.transceiverAddress = transceiverAddress

        self.probeLocalIDs = {}
        self.probeLocalIDs['MAC.TransceiverAddress'] = transceiverAddress

        self.names = dict()
        self.names['upperConvergence'] = upperConvergenceName
        self.names['perMIB'] = 'perMIB'
        self.names.update(wimemac.convergence.names)
        self.names.update(wimemac.frame.names)
        self.names.update(wimemac.management.names)
        self.names.update(wimemac.lowerMAC.names)
        self.names.update(wimemac.upperMAC.names)
        self.names.update(wimemac.protocolCalculator.names)

    def createUpperMAC(self, config, myFUN):
        return(wimemac.upperMAC.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createLowerMAC(self, config, myFUN):
        return(wimemac.lowerMAC.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createConvergence(self, config, myFUN):
        return(wimemac.convergence.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createFrame(self, config, myFUN):
        return(wimemac.frame.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createManagement(self, config, myFUN):
        return(wimemac.management.getFUN(self.transceiverAddress, self.names, config, myFUN, self.logger, self.probeLocalIDs))

    def createManagementServices(self, config):
        myServices = []
        myServices.append(wimemac.protocolCalculator.ProtocolCalculator(
                                        serviceName = self.names['protocolCalculator'] + str(self.transceiverAddress),
                                        config = wimemac.protocolCalculator.Config(),
                                        parentLogger = self.logger))

        myServices.append(wimemac.management.InformationBases.PER(serviceName = self.names['perMIB'] + str(self.transceiverAddress),
                                        config = wimemac.management.InformationBases.PERConfig(),
                                        parentLogger = self.logger))
        
        return(myServices)

