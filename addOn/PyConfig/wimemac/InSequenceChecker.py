from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class InSequenceChecker(Sealed):
    """In Sequence Checker FU"""

    __plugin__ = 'wimemac.reconfiguration.InSequenceChecker'
    name = "InSequenceChecker"

    logger = None
    """Logger configuration"""

    def __init__(self, enabled = True, parentLogger = None, **kw):
        self.logger = wns.Logger.Logger("wimemac", "Reconfiguration_InSequenceChecker", enabled, parentLogger)
        attrsetter(self, kw)
