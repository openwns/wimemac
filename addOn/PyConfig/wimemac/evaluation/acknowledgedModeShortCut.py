from openwns.evaluation import *

def installEvaluation(sim, loggingStations):

    sourceName = 'wimemac.packetErrorRate'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
    node.getLeafs().appendChildren(Moments())

    sourceName = 'wimemac.crcLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in CRC',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'wimemac.unicastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in unicast buffer',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'wimemac.broadcastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in broadcast buffer',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'wimemac.unicastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Unicast buffer size',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 20)) 
    
    sourceName = 'wimemac.broadcastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Broadcast buffer size',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 20)) 
