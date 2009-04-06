libname = 'wimemac-addOn'
srcFiles = dict()

srcFiles = [
    'src/WIMEMAC.cpp',
    'src/Component.cpp',
    ##'src/MIHComponent.cpp',
    #wimemac
    'src/frame/BeaconCollector.cpp',
    'src/frame/DataCollector.cpp',
    'src/frame/BeaconScheduler.cpp',
    'src/frame/BeaconBuilder.cpp',
    'src/frame/BeaconEvaluator.cpp',
    'src/drp/DRPManager.cpp',
    'src/drp/DRPmap.cpp',
    'src/helper/Queues.cpp',
    'src/drp/DRPScheduler.cpp',
    

    ## convergence
    'src/convergence/Upper.cpp',
    #'src/convergence/Lower2Copper.cpp',

    #'src/Routing.cpp',
    'src/BERProvider.cpp',
    'src/BERConsumer.cpp',
    #'src/BERMeasurementReporting.cpp',
    #'src/Pilot.cpp',

    ## macs
    #'src/mac/Aloha.cpp',
    #'src/mac/CSMACA.cpp',
    #'src/mac/Backoff.cpp',

    ##reconfiguration
    #'src/reconfiguration/Drain.cpp',
    #'src/reconfiguration/SupportUpperConnector.cpp',
    #'src/reconfiguration/SupportLowerDeliverer.cpp',
    #'src/reconfiguration/SupportLowerReceptor.cpp',
    #'src/reconfiguration/SupportUpper.cpp',
    #'src/reconfiguration/SupportLower.cpp',
    #'src/reconfiguration/SupportLowerCTISetter.cpp',
    #'src/reconfiguration/Manager.cpp',
    #'src/reconfiguration/SimpleManager.cpp',
    #'src/reconfiguration/TransmittingManager.cpp',
    #'src/reconfiguration/utils.cpp',
    #'src/reconfiguration/InSequenceChecker.cpp',
    #'src/reconfiguration/CompoundBacktracker.cpp',

    #'src/trigger/Trigger.cpp',

    ##MIH
    #'src/MIH/MeasurementsMonitor.cpp',
    #'src/MIH/tests/MeasurementsMonitorTest.cpp',
    #'src/MIH/LinkCommandProcessor.cpp',
    #'src/MIH/CapabilityDiscoveryProvider.cpp',

    #'src/arqfsm/stopandwait/FSMFU.cpp',
    #'src/arqfsm/stopandwait/BaseState.cpp',
    #'src/arqfsm/stopandwait/ReadyForTransmission.cpp',
    #'src/arqfsm/stopandwait/WaitingForACK.cpp',

    #'src/arqfsm/selectiverepeat/FSMFU.cpp',
    #'src/arqfsm/selectiverepeat/BaseState.cpp',
    #'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.cpp',
    #'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferPartlyFilled.cpp',
    #'src/arqfsm/selectiverepeat/WaitingForACKsBufferFull.cpp',

    #'src/tests/RoutingTest.cpp',
    #'src/tests/BERProviderConsumerTest.cpp',
    #'src/tests/BERMeasurementReportingTest.cpp',
    #'src/tests/PilotTest.cpp',
    #'src/tests/StamperTest.cpp',

    #'src/trigger/tests/TriggerTest.cpp',

    #'src/convergence/tests/UnicastBroadcastTest_Copper.cpp',

    #'src/mac/tests/AlohaTest.cpp',
    #'src/mac/tests/BackoffTest.cpp',
    #'src/mac/tests/CSMACATest.cpp',

    #'src/arqfsm/stopandwait/tests/StopAndWaitTest.cpp',
    #'src/arqfsm/selectiverepeat/tests/SelectiveRepeatFSMTest.cpp',
    ]

hppFiles = [

 #wimemac
    'src/WIMEMAC.hpp',
    'src/frame/BeaconCollector.hpp',
    'src/frame/DataCollector.hpp'
    'src/frame/BeaconScheduler.hpp',
    'src/frame/BeaconBuilder.hpp', 
    'src/frame/BeaconEvaluator.hpp',
    'src/drp/DRPManager.hpp',
    'src/frame/BeaconCommand.hpp',
    'src/drp/DRPmap.hpp',
    'src/drp/DRPScheduler.hpp',
    'src/helper/Queues.hpp',
    'src/helper/IDRPQueueInterface.hpp', 
    'src/helper/IQueueInterface.hpp',


#'src/arqfsm/ARQBaseState.hpp',
#'src/arqfsm/InSignals.hpp',
#'src/arqfsm/OutSignals.hpp',
#'src/arqfsm/selectiverepeat/BaseState.hpp',
#'src/arqfsm/selectiverepeat/FSMFU.hpp',
#'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.hpp',
#'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferPartlyFilled.hpp',
#'src/arqfsm/selectiverepeat/WaitingForACKsBufferFull.hpp',
#'src/arqfsm/stopandwait/BaseState.hpp',
#'src/arqfsm/stopandwait/FSMFU.hpp',
#'src/arqfsm/stopandwait/InSignals.hpp',
#'src/arqfsm/stopandwait/OutSignals.hpp',
#'src/arqfsm/stopandwait/ReadyForTransmission.hpp',
#'src/arqfsm/stopandwait/tests/StopAndWaitTest.hpp',
#'src/arqfsm/stopandwait/WaitingForACK.hpp',
'src/BERConsumer.hpp',
#'src/BERMeasurementReporting.hpp',
'src/BERProvider.hpp',
'src/Component.hpp',
#'src/convergence/Lower2Copper.hpp',
'src/convergence/Lower.hpp',
'src/convergence/Upper.hpp',
#'src/wimemac.hpp',
#'src/mac/Aloha.hpp',
#'src/mac/Backoff.hpp',
#'src/mac/CSMACA.hpp',
#'src/MIH/CapabilityDiscoveryProvider.hpp',
#'src/MIHComponent.hpp',
#'src/MIH/LinkCommandProcessor.hpp',
#'src/MIH/MeasurementsMonitor.hpp',
#'src/Pilot.hpp',
#'src/reconfiguration/CompoundBacktracker.hpp',
#'src/reconfiguration/Drain.hpp',
#'src/reconfiguration/InSequenceChecker.hpp',
#'src/reconfiguration/Manager.hpp',
#'src/reconfiguration/SimpleManager.hpp',
#'src/reconfiguration/SupportLowerCTISetter.hpp',
#'src/reconfiguration/SupportLowerDeliverer.hpp',
#'src/reconfiguration/SupportLower.hpp',
#'src/reconfiguration/SupportLowerReceptor.hpp',
#'src/reconfiguration/SupportUpperConnector.hpp',
#'src/reconfiguration/SupportUpper.hpp',
#'src/reconfiguration/TransmittingManager.hpp',
#'src/reconfiguration/utils.hpp',
#'src/Routing.hpp',
#'src/Stamper.hpp',
#'src/trigger/FunctionalUnitLight.hpp',
#'src/trigger/Trigger.hpp',
]

pyconfigs = [
##wimemac
'wimemac/frame.py',
#'wimemac/BeaconBuilder.py',
#
'wimemac/Routing.py',
#'wimemac/BERMeasurementReporting.py',
#'wimemac/MIH.py',
#'wimemac/InSequenceChecker.py',
#'wimemac/Stamper.py',
#'wimemac/Trigger.py',
#'wimemac/ARQFSM.py',
#'wimemac/Pilot.py',
#'wimemac/Reconfiguration.py',
'wimemac/Wimemac.py',
#'wimemac/__init__.py',
#'wimemac/support/SubFUN.py',
'wimemac/support/ShortCutWithFrame.py',
#'wimemac/support/Aloha.py',
#'wimemac/support/tests/ConfigurationTest.py',
#'wimemac/support/tests/__init__.py',
#'wimemac/support/StopAndWait.py',
#'wimemac/support/GoBackN.py',
#'wimemac/support/CSMACA.py',
#'wimemac/support/Configuration.py',
#'wimemac/support/SelectiveRepeat.py',
#'wimemac/support/__init__.py',
#'wimemac/support/ShortCut.py',
#'wimemac/CompoundBacktracker.py',
#'wimemac/evaluation/__init__.py',
#'wimemac/evaluation/default.py',
#'wimemac/evaluation/csma.py',
#'wimemac/evaluation/acknowledgedModeShortCut.py'
]
dependencies = []
Return('libname srcFiles hppFiles pyconfigs dependencies')
