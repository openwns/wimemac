from openwns.evaluation import *

def installEvaluation(sim, loggingStations, configuration):
    sourceName = 'traffic.endToEnd.window.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
    leafs = node.getLeafs()
    leafs.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="Node.id%d"))
    node.getLeafs().appendChildren(Moments(name = sourceName, description = 'average bit rate [Bit/s]'))
    #leafs.appendChildren(Moments(name = sourceName, description = 'system average bit rate [Bit/s]'))

    sourceName = 'traffic.endToEnd.packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(SettlingTimeGuard(configuration.settlingTimeGuard))
    node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="Node.id%d"))
    leafs = node.getLeafs()
    leafs.appendChildren(PDF(name = sourceName,
                            description = 'end to end packet delay [s] PDF',
                            minXValue = 0.0,
                            maxXValue = 0.1,
                            resolution = 1000))
    if configuration.createTimeseriesProbes == True:
        leafs.appendChildren(TimeSeries(format = "fixed", timePrecision = 6, 
                         valuePrecision = 6, 
                         name = "TrafficDelay_TimeSeries", 
                         description = "Delay [s]",
                         contextKeys = []))


