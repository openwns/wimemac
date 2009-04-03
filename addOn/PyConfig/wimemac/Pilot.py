import wns.Sealed

class Pilot(wns.Sealed.Sealed):

    transmissionTimeout = None
    logger = None
    startEnabled = None

    def __init__(self, transmissionTimeout, logger, startEnabled = True):
        self.transmissionTimeout = transmissionTimeout
        self.logger = logger
        self.startEnabled = startEnabled

class PilotTest(Pilot):

    def __init__(self):
        super(PilotTest, self).__init__(transmissionTimeout = 0.01,
                                        logger = wns.Logger.Logger("GLUE", "Pilot", True),
                                        startEnabled = False)
