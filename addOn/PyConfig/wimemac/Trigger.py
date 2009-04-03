from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class Trigger(Sealed):
    """Trigger"""

    __plugin__ = 'glue.Trigger'
    name = "Trigger"

    logger = None
    """Logger configuration"""

    BERProvider = None
    """Name of the BER provider"""

    SAR = None
    """Name of the SAR that shall be reconfigured"""

    packetHeaderSize = 64
    """Size of the packet header"""

    def __init__(self, BERProvider, SAR, **kw):
        self.logger = wns.Logger.Logger("GLUE", "Trigger", True)
        self.BERProvider = BERProvider
        self.SAR = SAR
        attrsetter(self, kw)
