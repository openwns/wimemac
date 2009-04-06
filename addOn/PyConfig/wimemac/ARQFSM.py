from wns.Sealed import Sealed
from wns.PyConfig import attrsetter
import wns.Logger

class ARQFSM(Sealed):
    """Base class for all ARQ FSMs

    ARQFSM.resendTimeout: time after which the ARQ should try to resend a
    PDU if no ACK was received
    """
    resendTimeout = 0.1

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"

    logger = None

    def __init__(self, **kw):
        attrsetter(self, kw)

class StopAndWait(ARQFSM):
    __plugin__ = 'wimemac.arqfsm.stopandwait.FSMFU'
    name = "StopAndWait"

    bitsPerIFrame = 2
    bitsPerRRFrame = 2

    def __init__(self, parent = None, **kw):
        self.logger = wns.Logger.Logger("wimemac", "StopAndWait", True, parent)
        attrsetter(self, kw)

class SelectiveRepeat(ARQFSM):
    __plugin__ = 'wimemac.arqfsm.selectiverepeat.FSMFU'
    name = "SelectiveRepeat"

    bitsPerIFrame = 10
    bitsPerACKFrame = 10

    windowSize = 512
    sequenceNumberSize = 1024

    def __init__(self, parent = None, **kw):
        self.logger = wns.Logger.Logger("wimemac", "SelectiveRepeat", True, parent)
        attrsetter(self, kw)
