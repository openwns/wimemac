from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class Routing(Sealed):
    """Routing FU"""

    __plugin__ = 'wimemac.Routing'
    name = "Routing"

    logger = None
    """Logger configuration"""

    addressProvider = None
    """Name of the address provider FU (friend)"""

    allowRouteChange = False
    """Allow change of routing information"""

    def __init__(self, addressProvider, **kw):
        self.logger = wns.Logger.Logger("wimemac", "Routing", True)
        self.addressProvider = addressProvider
        attrsetter(self, kw)
