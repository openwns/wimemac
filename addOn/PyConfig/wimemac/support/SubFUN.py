import wns.FUN
import wns.Group
import wns.Multiplexer
import wns.Tools

import wimemac.Reconfiguration
import wimemac.InSequenceChecker
import wimemac.Routing

class Reconfiguration(wns.Group.Group):

    def __init__(self, addressProvider, reconfigurationSchemes, masterRM = False, berProvider = None, loggerEnabled = True, parentLogger = None):
        wns.Group.Group.__init__(self, wns.FUN.FUN(), "inSequenceChecker", "sync")

        # create
        inSequenceChecker = wns.FUN.Node("inSequenceChecker", wimemac.InSequenceChecker.InSequenceChecker(loggerEnabled, parentLogger))

        # control plane
        reconfigurationManagerFU = wimemac.Reconfiguration.TransmittingManager(masterRM,
                                                                            reconfigurationSchemes,
                                                                            loggerEnabled,
                                                                            parentLogger,
                                                                            reconfigurationInterval = 0.5,
                                                                            useOptimizedReconfigurationIfPossible = True)
        reconfigurationManager = wns.FUN.Node("reconfigurationManager", reconfigurationManagerFU)
        if berProvider is not None:
            controlCRC = wns.FUN.Node("controlCRC", wns.CRC.CRC(berProvider))

        # user plane
        supportUpper = wns.FUN.Node("supportUpper", wimemac.Reconfiguration.SupportUpper("reconfigurationManager", loggerEnabled, parentLogger))

        drain = wns.FUN.Node("drain", wimemac.Reconfiguration.Drain(loggerEnabled, parentLogger))
        supportLower = wns.FUN.Node("supportLower", wimemac.Reconfiguration.SupportLower("reconfigurationManager", "drain", loggerEnabled, parentLogger))

        # joined plane
        planeDispatcher = wns.FUN.Node("planeDispatcher", wns.Multiplexer.Dispatcher(1))
        routing = wns.FUN.Node("routing", wimemac.Routing.Routing(addressProvider))
        sync = wns.FUN.Node("sync", wns.Tools.Synchronizer())


        # add
        self.fun.add(inSequenceChecker)

        # control plane
        self.fun.add(reconfigurationManager)
        if berProvider is not None:
            self.fun.add(controlCRC)

        # user plane
        self.fun.add(supportUpper)
        for node in reconfigurationSchemes[0].fun.functionalUnit:
            self.fun.add(node)
        self.fun.add(drain)
        self.fun.add(supportLower)

        # joined plane
        self.fun.add(planeDispatcher)
        self.fun.add(routing)
        self.fun.add(sync)


        # connect
        # control plane
        if berProvider is not None:
            reconfigurationManager.connect(controlCRC)
            controlCRC.connect(planeDispatcher)
        else:
            reconfigurationManager.connect(planeDispatcher)

        # user plane
        inSequenceChecker.connect(supportUpper)
        supportUpper.connect(reconfigurationSchemes[0].topFU)
        self.fun.connects += reconfigurationSchemes[0].fun.connects
        reconfigurationSchemes[0].bottomFU.connect(supportLower)
        supportLower.connect(planeDispatcher)

        # joined plane
        planeDispatcher.connect(routing)
        routing.connect(sync)
