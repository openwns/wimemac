from openwns.evaluation import *

def installEvaluation(sim, loggingStations):

    sourceName = 'glue.packetErrorRate'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Separate(by = 'wns.node.Node.id', forAll = loggingStations, format="wns.node.Node.id%d"))
    node.getLeafs().appendChildren(Moments())

    sourceName = 'glue.crcLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in CRC',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'glue.unicastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in unicast buffer',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'glue.broadcastBufferLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Loss ratio in broadcast buffer',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 1000)) 

    sourceName = 'glue.unicastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Unicast buffer size',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 20)) 
    
    sourceName = 'glue.broadcastBufferSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Accept(by = 'wns.node.Node.id', ifIn = loggingStations))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                       description = 'Broadcast buffer size',
                                       minXValue = 0.0,
                                       maxXValue = 1.0,
                                       resolution = 20)) 
