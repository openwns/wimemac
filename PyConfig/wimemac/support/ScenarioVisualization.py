import openwns.geometry.position
import matplotlib
from matplotlib.patches import Circle
from matplotlib.patches import Rectangle
from matplotlib.collections import PatchCollection
import pylab
import os




#stationPositions = []
#stationPositions.append([-1.0, 0.0])
#stationPositions.append([2.0, 1.0])
#stationPositions.append([2.0, -1.0])

#wallPositions = []
#wallPositions.append([1.0, -0.5, 1.0, 0.5])
#wallPositions.append([-1.0, -1.0, 0.5, -1.0])

class ScenarioDrawer(object):

    def __init__(self, nodelist, objs, directory):
        self.nodelist = nodelist
        self.objs = objs
        self.directory = directory

    def drawScenario(self):
        
        #here, a list of the station positions in 2D coordinates is created
        stationPositions = []
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
        
        
        
        #for a proper positioning of the wall, the wall orientation (i.e. vertical or horizontal) is stored in the wallOrientations list
        wallOrientations = []
        for wall in wallPositions:
          xdif = abs(wall[2] - wall[0])
          ydif = abs(wall[3] - wall[1])
          if xdif > ydif:
            wallOrientations.append('h')
          else:
            wallOrientations.append('v')


        xpos = []
        ypos = []

        for position in stationPositions:
            xpos.append(position[0])
            ypos.append(position[1])

        xlen = max(xpos) - min(xpos)
        ylen = max(ypos) - min(ypos)

        wallThickness = 0.02 * xlen

          
        paddingx = xlen * 0.1
        paddingy = ylen * 0.1

        #assure that neither paddingx nor paddingy is zero:
        if xlen < 0.01:
          paddingx = paddingx + 1
        if ylen < 0.01:
          paddingy = paddingy + 1


        pylab.xlim(min(xpos) - paddingx, max(xpos) + paddingx)
        pylab.ylim(min(ypos) - paddingy, max(ypos) + paddingy)


        #at first, the stations are placed:
        for position in stationPositions:
            c=Circle((position[0], position[1]), 0.02 * xlen, facecolor="blue")

            pylab.gca().add_patch(c)


        #now the walls are placed:
        for position in wallPositions:
          orientation = wallOrientations[0]
          wallOrientations.remove(orientation)
          if orientation == 'v':
            wall=Rectangle((position[0] - wallThickness/2, position[1]), wallThickness, position[3] - position[1], facecolor="black")
          else:
            wall=Rectangle((position[0], position[1] - wallThickness/2), position[2] - position[0], wallThickness, facecolor="black")
          
          pylab.gca().add_patch(wall)

            
        filename = self.directory + "/Scenario.png"
        pylab.grid()
        pylab.savefig(filename)
