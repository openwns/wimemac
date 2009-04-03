from wns.PyConfig import attrsetter
from wns import Sealed
import wns.Logger
import wns.FUN
import wns.FCF
import dll.Services
from wns.Sealed import Sealed

class ConfigFrame:#(Sealed):
    
    #Slot Duration
    SlotLength = 256E-6
    
    #Slots per MAC Frame
    FrameSlots = 256
    
    
    #MAC Frame Duration
    FrameLength = FrameSlots * SlotLength
    
    
    #BeaconSlot Duration
    BeaconSlotLength = 85E-6


    def __init__(self, **kw):
	    wns.PyConfig.attrsetter(self, kw)






#class DataCollector(wns.FCF.CompoundCollector):
#	__plugin__ = "glue.frame.DataCollector"
#	
#	logger = None
#	duration = None
#	
#	def __init__(self, name, duration, parentLogger = None):
#		super(DataCollector, self).__init__(name)
#		self.duration = duration
#		self.logger = wns.Logger.Logger("Glue", "Data", True)
#        	self.logger.level = 2
		



class DRPScheduler:
	__plugin__ = "glue.wimemac.drp.DRPScheduler"
	
	logger = None
               
	
	def __init__(self,name,parentLogger = None):
            self.logger = wns.Logger.Logger("Glue", "DRPScheduler", True)
            self.logger.level = 2


class BeaconEvaluator:
	__plugin__ = "glue.wimemac.frame.BeaconEvaluator"
	
	logger = None
               
	
	def __init__(self,name,parentLogger = None):
            self.logger = wns.Logger.Logger("Glue", "BeaconEvaluator", True)
            self.logger.level = 2



class BeaconCollector(wns.FCF.CompoundCollector):
	__plugin__ = "glue.wimemac.frame.BeaconCollector"
	
	logger = None
	duration = None
	
	def __init__(self, name, duration, beaconSlot, parentLogger = None):
		super(BeaconCollector, self).__init__(name)
		self.duration = duration
                self.BeaconSlot = beaconSlot
		self.logger = wns.Logger.Logger("Glue", "Beacon", True)
        	self.logger.level = 2



class BeaconBuilder:
	__plugin__ = "glue.wimemac.frame.BeaconBuilder"
	
	logger = None
        broadcastRouting = None
        
	
	def __init__(self,name,parentLogger = None):
            self.logger = wns.Logger.Logger("Glue", "BeaconBuilder", True)
            self.logger.level = 2
            self.broadcastRouting = 'UpperCommand'


class DataCollector(wns.FCF.CompoundCollector):
	__plugin__ = "glue.wimemac.frame.DataCollector"
	
	logger = None
	duration = None
	
	def __init__(self, name, duration, parentLogger = None):
		super(DataCollector, self).__init__(name)
		self.duration = duration
                self.logger = wns.Logger.Logger("Glue", "Data", True)
        	self.logger.level = 2


#class BeaconScheduler:#(Sealed):
#	__plugin__ = "glue.frame.BeaconScheduler"
#	
#	name = None
#	logger = None
#	upperConvergenceCommandName = None
#	myConfig = None
#	
#	
#
#	def __init__(self, name, config, upperConvergenceCommandName):
#		self.BeaconSlot = 1
#		self.logger = wns.Logger.Logger("Glue", "BeaconScheduler", True)
#	       	self.logger.level = 2
#		self.upperConvergenceCommandName = 'unicastUpperConvergence'
#		self.myConfig = config
#		self.name = name
