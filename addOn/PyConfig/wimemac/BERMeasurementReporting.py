from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class BERMeasurementReporting(Sealed):
    """BER Measurement Reporting FU"""

    __plugin__ = 'glue.BERMeasurementReporting'
    name = "BERMeasurementReporting"

    logger = None
    """Logger configuration"""

    BERProvider = None
    """Name of the BER Provider FU (friend)"""

    transmissionInterval = 0.01 # seconds
    """Interval for transmitting BER measurement reports"""

    headerSize = 32 # bit
    """Size of the BER Measurement Reporting command"""

    def __init__(self, BERProvider, **kw):
        self.logger = wns.Logger.Logger("GLUE", "BERMeasurementReporting", True)
        self.BERProvider = BERProvider
        attrsetter(self, kw)
