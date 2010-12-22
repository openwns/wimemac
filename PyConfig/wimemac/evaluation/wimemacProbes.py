from openwns.evaluation import *

def installEvaluation(sim, loggingStations, configuration):

    if configuration.createSNRProbes == True:
        sourceName = 'wimemac.phyuser.rxPower'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'received signal power [dBm]'))

        sourceName = 'wimemac.phyuser.interferencePower'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'received interference power [dBm]'))

        sourceName = 'wimemac.phyuser.SINR'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        node.getLeafs().appendChildren(Moments(name = sourceName,
                                               description = 'SINR [dB]'))

    if configuration.createChannelUsageProbe:
        sourceName = 'wimemac.drpscheduler.pcaPortion'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(PDF(minXValue = 0.0, maxXValue = 1.0, resolution=1,
                                               name = "wimemac.drpscheduler.pcaPortion",
                                               description = "Portion of the PCA Channel Access"))
                                               
        sourceName = 'wimemac.bb.numberOfMAS'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'numberOfMAS'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 1, 
                         name = "numberOfMAS_TimeSeries", 
                         description = "numberOfMAS",
                         contextKeys = []))

    
    if configuration.createMCSProbe:
        sourceName = 'wimemac.manager.mcs'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'PhyMode [Mb/s]'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 1, 
                         name = "PhyMode_TimeSeries", 
                         description = "PhyMode [Mb/s]",
                         contextKeys = []))

    if configuration.createPERProbe:
        sourceName = 'wimemac.errormodelling.PER'
        node = openwns.evaluation.createSourceNode(sim, sourceName)
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'packet error rate []'))
        if configuration.createTimeseriesProbes == True:
            leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 3, 
                         name = "PER_TimeSeries", 
                         description = "packet error rate []",
                         contextKeys = []))               

    if configuration.createTimeseriesProbes == True:
        sourceName = 'wimemac.traffic.incoming.throughput'
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
        leafs = node.getLeafs()
        node.getLeafs().appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
        node.getLeafs().appendChildren(Moments(name = sourceName, description = 'throughput [Bit/s]'))    
        leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                     valuePrecision = 1, 
                     name = "TrafficpSF_TimeSeries", 
                     description = "Incoming throughput per SF [Bit/s]",
                     contextKeys = []))

