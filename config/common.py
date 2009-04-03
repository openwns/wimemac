import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'wimemac--main--0.1',
                            AUTODEPS       = ['wns'],
                            PROJMODULES    = ['TEST', 'BASE'],
                            LIBRARY        = True,
                            SHORTCUTS      = True,
                            FLATINCLUDES   = False,
			    REVISIONCONTROL = RCS.Bazaar('../', 'WiMeMAC--main--0.1', 'deprecated', 'deprecated'), 
                            )
Return('commonEnv')
