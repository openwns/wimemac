import wns.Sealed

class Stamper(wns.Sealed.Sealed):
    logger = None
    logMessage = None

    def __init__(self, logger, logMessage):
        self.logger = logger
        self.logMessage = logMessage

class StamperTest(Stamper):

    def __init__(self):
        super(StamperTest, self).__init__(logger = wns.Logger.Logger("wimemac", "Pilot", True),
                                          logMessage = "Stamper stamped a compound")
