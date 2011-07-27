import openwns.geometry.position
import matplotlib
from matplotlib.patches import Circle
#from matplotlib.patches import Rectangle
from matplotlib.patches import ConnectionPatch
from matplotlib.collections import PatchCollection
from matplotlib.lines import Line2D
import random
import pylab
import os

##########
### This script reads all the scenariodata (stations, walls and hops between the station) from textfiles
### This is how the Text Files should look like:
## 'ScenarioConfig'
# Station:[x_pos, y_pos]
# ..insert some more stations...
# Wall:[xStart, yStart, xEnd, yEnd]
# ..insert some more walls...
###
###
## 'wimemac.e2e.hopPath_Text.dat'
# You can write anything as long as the line does not start with '['
# [stationID-stationID-...-stationID]
# ..insert some more hops..
##########

class ScenarioDrawer(object):
    
    def __init__(self, objs, directory):
        self.objs = objs
        self.directory = directory

    #Put all 'name'-files in the current directory (recursive) into a list
    def search(self, path, name):
        for root, dirs, filenames in os.walk(path):
            for filename in filenames:
                if filename == name:
                    yield os.path.join(root, filename)        

    def colorPicker(self, i):
        color = ['#990033', '#0000FF', '#00FF00', '#F79E00', '#ff00ff', '#0080FF', '#FF0000', '#2D0668', '#2EB42E', '#ff6633', '#8000ff', '#666633', '#cc0077', '#099499', '#996633', '#000000']
        return color[i % len(color)]

    def connectionStylePicker(self, i):
        style = ["arc3,rad=0.1","arc3,rad=0.2","arc3,rad=0.3","arc3,rad=0.4","arc3,rad=0.5","arc3,rad=0.6","arc3,rad=0.7","arc3,rad=0.8","arc3,rad=0.9","arc3,rad=-0.1","arc3,rad=-0.2","arc3,rad=-0.3","arc3,rad=-0.4","arc3,rad=-0.5","arc3,rad=-0.6","arc3,rad=-0.7","arc3,rad=-0.8","arc3,rad=-0.9",]
        return style[i % len(style)]

    def lineStylePicker(self, i):
        line = ["solid", "dashed", "dashdot", "dotted"]
        return line[i % len(line)]

    def drawScenario(self):
        
        results = []
        
        #open the scenarioConfig file
        f_in = open('ScenarioConfig', 'rb')
        for line in f_in:
            results.append(line)
        f_in.close()

        stationPositions = []
        wallPositions = []
        wallOrientations = []

        for i in range(len(results)):
            scenario_data = results[i]
            
            lastFoundParameterIndex = 0

            # Get limiters
            lLimit = scenario_data.find("'", lastFoundParameterIndex) + 1
            lastFoundParameterIndex = lLimit
            rLimit = scenario_data.find(":", lastFoundParameterIndex)
            lastFoundParameterIndex = rLimit
            
            Parameter = scenario_data[lLimit:rLimit]

            lLimit = scenario_data.find("[", lastFoundParameterIndex)
            lastFoundParameterIndex = lLimit
            rLimit = scenario_data.find("]", lastFoundParameterIndex) + 1
            lastFoundParameterIndex = rLimit

            ParameterValue = scenario_data[lLimit:rLimit]
            #print "Parameter: ", Parameter, " Value: ", ParameterValue
            
            if Parameter == 'Station':

                lastFoundParameterIndex = 0
                lLimit = ParameterValue.find("[", lastFoundParameterIndex) + 1
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find(",", lastFoundParameterIndex)
                sta_x = float(ParameterValue[lLimit:rLimit])

                lLimit = rLimit + 2
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find("]", lastFoundParameterIndex)
                sta_y = float(ParameterValue[lLimit:rLimit])

                stationPositions.append([sta_x, sta_y])        
            elif Parameter == 'Wall':

                lastFoundParameterIndex = 0
                lLimit = ParameterValue.find("[", lastFoundParameterIndex) + 1
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find(",", lastFoundParameterIndex)
                xStart = float(ParameterValue[lLimit:rLimit])

                lLimit = rLimit + 2
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find(",", lastFoundParameterIndex)
                yStart = float(ParameterValue[lLimit:rLimit])

                lLimit = rLimit + 2
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find(",", lastFoundParameterIndex)
                xEnd = float(ParameterValue[lLimit:rLimit])

                lLimit = rLimit + 2
                lastFoundParameterIndex = lLimit
                rLimit = ParameterValue.find("]", lastFoundParameterIndex)
                yEnd = float(ParameterValue[lLimit:rLimit])
                
                wallPositions.append([xStart, yStart, xEnd, yEnd])
            else:
                print 'Unknown Parameter : ' + str(Parameter)

        #evaluate min and max coordinates for a proper scenario scaling
        xpos = []
        ypos = []

        for position in stationPositions:
            xpos.append(position[0])
            ypos.append(position[1])
        for position in wallPositions:
            xpos.append(position[0])
            ypos.append(position[1])
            xpos.append(position[2])
            ypos.append(position[3])

        xlen = max(xpos) - min(xpos)
        ylen = max(ypos) - min(ypos)

        wallThickness = 0.01 * max(xlen,ylen)

          
        paddingx = xlen * 0.1
        paddingy = ylen * 0.1

        #assure that neither paddingx nor paddingy is zero:
        if xlen < 0.01:
          paddingx = paddingx + 1
        if ylen < 0.01:
          paddingy = paddingy + 1


        pylab.xlim(min(xpos) - paddingx, max(xpos) + paddingx)
        pylab.ylim(min(ypos) - paddingy, max(ypos) + paddingy)

        stationNumber = 0

        #at first, the stations are placed:
        for position in stationPositions:
            c=Circle((position[0], position[1]), 0.01 * xlen, facecolor="white")
            pylab.gca().add_patch(c)
            #pylab.gca().annotate(str(stationNumber), xy=(position[0], position[1]), xycoords='data', xytext=(position[0]+random.random(), position[1]+random.random()), textcoords='data', arrowprops=dict(arrowstyle="->", connectionstyle="arc3"), color='black', backgroundcolor='white')
            stationNumber+=1

        #now the walls are placed:
        for position in wallPositions:
          wall = Line2D([position[0],position[2]],[position[1],position[3]],lw=5.0, c="black")
          pylab.gca().add_line(wall)

        #get all the node files:
        hopPathFiles = list(self.search(self.directory, 'wimemac.e2e.hopPath_Text.dat'))           
        
        pictureNumber = 0

        #Get hops from Files:
        for files in hopPathFiles:
            f_hop = open(files, "rb")

            hops = []
            for line in f_hop:
                if line[0] == '[':
                    hops.append(((line.replace('\n','')).replace('[','')).replace(']',''))
            
            f_hop.close()

            lstHops = []
            for lines in hops:
                lstHops.append([int(x) for x in lines.split('-')])

            pickColor = 0
            #now the hops are drawn
            for stationHops in lstHops:
                pickLineStyle = random.randint(0,3)
                for hops in range(len(stationHops)-1):
                    xyA = (stationPositions[stationHops[hops]-1][0], stationPositions[stationHops[hops]-1][1])
                    xyB = (stationPositions[stationHops[hops+1]-1][0], stationPositions[stationHops[hops+1]-1][1])
                    coordsA = "data"
                    coordsB = "data"
                    if (hops == 0) or (hops == (len(stationHops)-2)):
                        arrow=ConnectionPatch(xyA, xyB, coordsA, coordsB, arrowstyle="->" ,shrinkA=5, shrinkB=5, mutation_scale=20, fc="w", connectionstyle=self.connectionStylePicker(random.randint(0,17)), color=self.colorPicker(pickColor), linestyle=self.lineStylePicker(pickLineStyle))
                    else: 
                        arrow=ConnectionPatch(xyA, xyB, coordsA, coordsB, arrowstyle="->", shrinkA=5, shrinkB=5, mutation_scale=20, fc="w", color="black")
                    pylab.gca().add_patch(arrow)

                pickColor+=1
            self.picture(pictureNumber)
            pictureNumber+=1

        if len(hopPathFiles) == 0:
            self.picture(pictureNumber)
    def picture(self, i):
        filename = self.directory + "/Scenario" + str(i) + ".pdf"
        pylab.grid()
        pylab.savefig(filename)
