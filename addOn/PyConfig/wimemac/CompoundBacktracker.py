from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class CompoundBacktracker(Sealed):
    """Compound Backtracker FU"""

    __plugin__ = 'wimemac.CompoundBacktracker'
    name = "CompoundBacktracker"

    logger = None
    """Logger configuration"""

    backtraceIncoming = True
    """Show backtrace of incoming compounds"""

    backtraceOutgoing = True
    """Show backtrace of outgoing compounds"""

    def __init__(self, parentLogger = None, **kw):
        self.logger = wns.Logger.Logger("wimemac", "CompoundBacktracker", True, parentLogger)
        attrsetter(self, kw)
