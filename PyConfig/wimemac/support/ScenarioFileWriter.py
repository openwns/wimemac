import openwns.geometry.position
import os

#Writes the scenario-configuration (walls (position and orientation), stations) into a textfile

class ScenarioWriter(object):

    def __init__(self, nodelist, objs, directory):
        self.nodelist = nodelist
        self.objs = objs
        self.directory = directory

    def writeScenario(self):
        
        #here, a list of the station positions in 2D coordinates is created
        stationPositions = []
        typeList= []
        for i in range(len(self.nodelist)):
            try:
                sta_x = self.nodelist[i].mobility.mobility.getCoords().x
                sta_y = self.nodelist[i].mobility.mobility.getCoords().y
            except:
                print "Node ID " , i , "cannot be displayed"
            else:
                stationPositions.append([sta_x, sta_y])
        
        #here, a list of the wall positions in 2D coordinates is created
        wallPositions = []
        for i in range(len(self.objs)):
            xStart = self.objs[i].pointA.x
            yStart = self.objs[i].pointA.y
            xEnd = self.objs[i].pointB.x
            yEnd = self.objs[i].pointB.y
            wallPositions.append([xStart, yStart, xEnd, yEnd])
        

        f_out = open("ScenarioConfig", "w")

        for i in range(len(stationPositions)):
            f_out.write('Station:'+str(stationPositions[i])+'\n')
        for i in range(len(wallPositions)):
            f_out.write('Wall:'+str(wallPositions[i])+'\n')
        f_out.close()

