###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2009
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
# phone: ++49-241-80-27910,
# fax: ++49-241-80-22242
# email: info@openwns.org
# www: http://www.openwns.org
# _____________________________________________________________________________
#
# openWNS is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 2 as published by the
# Free Software Foundation;
#
# openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

"""ErrorModelling implementation

   the class ErrorModelling configures the ErrorModelling implementation,
   which performs a Mapping from PHYMode, CIR to Bit Error Rate (BER) and
   calculate the Packet Error Rate (PER).
"""

import openwns.FUN
import wimemac.Logger
from openwns.pyconfig import Frozen, Sealed, attrsetter

class MappingObject(Sealed):
    snr = None
    pmean = None

    def __init__(self, _snr, _pmean):
        self.snr = _snr
        self.pmean = _pmean


class SNR2PmeanMapping(Sealed):
    mapping = None

    def __init__(self, rawMapping):
        self.mapping = []
        for ii in rawMapping:
            self.mapping.append(MappingObject(ii[0], ii[1]))

#### ChannelModel 1
rawCM1NIBP6S100 = [ [ -4, 12.9815789473684],
                 [ -3, 12.9815789473684],
                 [ -2, 22.7783251231527],
                 [ -1, 51.3580562659846],
                 [  0, 157.622065727700],
                 [  1, 868.610169491526],
                 [  2, 7157.22566371682],
                 [  3, 85996.3899082569],
                 [  4, 1228719.02536232],
                 [  5, 21772819.8571429],
                 [  6, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S150 = [ [ -4, 6.77777777777777],
                 [ -3, 6.77777777777777],
                 [ -2, 9.50000000000000],
                 [ -1, 15.0093896713615],
                 [  0, 26.9481641468683],
                 [  1, 58.6531440162272],
                 [  2, 161.255597014925],
                 [  3, 683.007858546168],
                 [  4, 3762.34864300626],
                 [  5, 31689.0436363636],
                 [  6, 379952.274004684],
                 [  7, 6094619.34567901],
                 [  8, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S200 = [ [ -4, 6],
                 [ -3, 6],
                 [ -2, 6.78571428571429],
                 [ -1, 10.6348448687351],
                 [  0, 16.7060606060606],
                 [  1, 30.4162303664921],
                 [  2, 58.3575000000000],
                 [  3, 195.823677581864],
                 [  4, 719.810344827587],
                 [  5, 3682.05277777778],
                 [  6, 27432.2771084337],
                 [  7, 223795.873846154],
                 [  8, 2087240.91250000],
                 [  9, 22709316.4615385],
                 [ 10, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S300 = [ [ -4, 101.172972972973],
                 [ -3, 101.172972972973],
                 [ -2, 101.172972972973],
                 [ -1, 101.172972972973],
                 [  0, 101.172972972973],
                 [  1, 101.172972972973],
                 [  2, 101.172972972973],
                 [  3, 101.172972972973],
                 [  4, 101.172972972973],
                 [  5, 101.172972972973],
                 [  6, 273.806930693070],
                 [  7, 855.879828326181],
                 [  8, 3724.17031630171],
                 [  9, 19030.3421750663],
                 [ 10, 136754.287917738],
                 [ 11, 1188505.51843318],
                 [ 12, 12900630.4400000],
                 [ 13, 1000000100.00000],
                 [ 14, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S375 = [ [ -4, 6],
                 [ -3, 6],
                 [ -2, 6],
                 [ -1, 6],
                 [  0, 6],
                 [  1, 6.67500000000000],
                 [  2, 9.38915094339623],
                 [  3, 12.3397683397684],
                 [  4, 19.8938428874735],
                 [  5, 27.9844290657439],
                 [  6, 55.0976562500000],
                 [  7, 115.017077798861],
                 [  8, 270.492537313433],
                 [  9, 779.081593927893],
                 [ 10, 2748.65771812081],
                 [ 11, 12253.3831578947],
                 [ 12, 62452.5819070905],
                 [ 13, 363003.839108911],
                 [ 14, 2088230.05273834],
                 [ 15, 12280291.3469388],
                 [ 16, 68805912.3529412],
                 [ 17, 1000000100.00000],
                 [ 18, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S600 = [ [ -4, 31.5811403508772],
                 [ -3, 31.5811403508772],
                 [ -2, 31.5811403508772],
                 [ -1, 31.5811403508772],
                 [  0, 31.5811403508772],
                 [  1, 31.5811403508772],
                 [  2, 31.5811403508772],
                 [  3, 31.5811403508772],
                 [  4, 31.5811403508772],
                 [  5, 31.5811403508772],
                 [  6, 31.5811403508772],
                 [  7, 31.5811403508772],
                 [  8, 31.5811403508772],
                 [  9, 31.5811403508772],
                 [ 10, 51.3021276595745],
                 [ 11, 94.1561771561772],
                 [ 12, 166.721014492754],
                 [ 13, 393.848314606742],
                 [ 14, 1032.96500920810],
                 [ 15, 3257.43629343630],
                 [ 16, 11925.9092872570],
                 [ 17, 47687.4677419355],
                 [ 18, 206892.538106236],
                 [ 19, 1133686.96614583],
                 [ 20, 6345990.64968153],
                 [ 21, 41404322.8260870],
                 [ 22, 1000000100.00000],
                 [ 23, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S750 = [ [ -4, 37.8140900195695],
                 [ -3, 37.8140900195695],
                 [ -2, 37.8140900195695],
                 [ -1, 37.8140900195695],
                 [  0, 37.8140900195695],
                 [  1, 37.8140900195695],
                 [  2, 37.8140900195695],
                 [  3, 37.8140900195695],
                 [  4, 37.8140900195695],
                 [  5, 37.8140900195695],
                 [  6, 37.8140900195695],
                 [  7, 37.8140900195695],
                 [  8, 37.8140900195695],
                 [  9, 37.8140900195695],
                 [ 10, 37.8140900195695],
                 [ 11, 59.8302277432712],
                 [ 12, 100.437810945274],
                 [ 13, 157.248917748918],
                 [ 14, 297.618444846293],
                 [ 15, 682.269230769231],
                 [ 16, 1529.49277266754],
                 [ 17, 4175.59264305177],
                 [ 18, 13303.6765578635],
                 [ 19, 41083.0710823910],
                 [ 20, 167959.348729792],
                 [ 21, 587190.778688525],
                 [ 22, 2748757.02257337],
                 [ 23, 9311948.66417911],
                 [ 24, 60885390.8500000],
                 [ 25, 88863175.6923077],
                 [ 26, 1000000100.00000],
                 [ 27, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM1NIBP6S900 = [ [ -4, 23.3349436392915],
                 [ -3, 23.3349436392915],
                 [ -2, 23.3349436392915],
                 [ -1, 23.3349436392915],
                 [  0, 23.3349436392915],
                 [  1, 23.3349436392915],
                 [  2, 23.3349436392915],
                 [  3, 23.3349436392915],
                 [  4, 23.3349436392915],
                 [  5, 23.3349436392915],
                 [  6, 23.3349436392915],
                 [  7, 23.3349436392915],
                 [  8, 23.3349436392915],
                 [  9, 23.3349436392915],
                 [ 10, 23.3349436392915],
                 [ 11, 23.3349436392915],
                 [ 12, 23.3349436392915],
                 [ 13, 33.0575657894737],
                 [ 14, 50.3905429071804],
                 [ 15, 74.9685314685316],
                 [ 16, 108.331896551724],
                 [ 17, 184.046698872786],
                 [ 18, 320.130578512397],
                 [ 19, 624.127819548873],
                 [ 20, 1126.71364985163],
                 [ 21, 2155.81012658228],
                 [ 22, 4284.27676767677],
                 [ 23, 7781.62227074236],
                 [ 24, 13933.2005076142],
                 [ 25, 24417.9207459207],
                 [ 26, 40677.8020833333],
                 [ 27, 65023.2598039216],
                 [ 28, 109792.576271187],
                 [ 29, 181645.611494253],
                 [ 30, 334477.279518072],
                 [ 31, 615609.308008213],
                 [ 32, 1263334.82832618],
                 [ 33, 2761590.02941177],
                 [ 34, 6545494.24229075],
                 [ 35, 19646257.8815789],
                 [ 36, 96119842.8666667],
                 [ 37, 1000000100.00000],
                 [ 38, 1000000100.00000],
                 [ 39, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
#### /ChannelModel 1

#### ChannelModel 2
rawCM2NIBP6S100 = [ [ -4, 81.4124168514412],
                 [ -3, 81.4124168514412],
                 [ -2, 81.4124168514412],
                 [ -1, 81.4124168514412],
                 [  0, 387.857894736842],
                 [  1, 3492.10840108401],
                 [  2, 46941.8644501279],
                 [  3, 1110725.17763158],
                 [  4, 1000000100.00000],
                 [  5, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S150 = [ [ -4, 34.9127358490566],
                 [ -3, 34.9127358490566],
                 [ -2, 34.9127358490566],
                 [ -1, 34.9127358490566],
                 [  0, 34.9127358490566],
                 [  1, 98.9040307101728],
                 [  2, 488.728464419476],
                 [  3, 3855.48351648352],
                 [  4, 45324.7456647399],
                 [  5, 700780.802702702],
                 [  6, 19171641.0869565],
                 [  7, 1000000100.00000],
                 [  8, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S200 = [ [ -4, 22.8785046728972],
                 [ -3, 22.8785046728972],
                 [ -2, 22.8785046728972],
                 [ -1, 22.8785046728972],
                 [  0, 22.8785046728972],
                 [  1, 22.8785046728972],
                 [  2, 108.277522935780],
                 [  3, 369.506666666666],
                 [  4, 2230.24251497006],
                 [  5, 16432.1791907514],
                 [  6, 179016.416279070],
                 [  7, 1942995.07288630],
                 [  8, 35076372.8947368],
                 [  9, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S300 = [ [ -4, 40.6570155902004],
                 [ -3, 40.6570155902004],
                 [ -2, 40.6570155902004],
                 [ -1, 40.6570155902004],
                 [  0, 40.6570155902004],
                 [  1, 40.6570155902004],
                 [  2, 40.6570155902004],
                 [  3, 40.6570155902004],
                 [  4, 85.1727493917274],
                 [  5, 233.912144702842],
                 [  6, 800.542936288089],
                 [  7, 3780.77729257642],
                 [  8, 20901.0509383378],
                 [  9, 142247.365497076],
                 [ 10, 1327418.36051502],
                 [ 11, 13221158.5945946],
                 [ 12, 1000000100.00000],
                 [ 13, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S375 = [ [ -4, 53.0833333333334],
                 [ -3, 53.0833333333334],
                 [ -2, 53.0833333333334],
                 [ -1, 53.0833333333334],
                 [  0, 53.0833333333334],
                 [  1, 53.0833333333334],
                 [  2, 53.0833333333334],
                 [  3, 53.0833333333334],
                 [  4, 53.0833333333334],
                 [  5, 53.0833333333334],
                 [  6, 128.028455284553],
                 [  7, 302.578740157480],
                 [  8, 873.371369294606],
                 [  9, 2835.55744680851],
                 [ 10, 12230.8841463415],
                 [ 11, 53674.7943722944],
                 [ 12, 233136.887804878],
                 [ 13, 999219.901515151],
                 [ 14, 4838963.45490196],
                 [ 15, 20604696.6724138],
                 [ 16, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S600 = [ [ -4, 40.1785714285715],
                 [ -3, 40.1785714285715],
                 [ -2, 40.1785714285715],
                 [ -1, 40.1785714285715],
                 [  0, 40.1785714285715],
                 [  1, 40.1785714285715],
                 [  2, 40.1785714285715],
                 [  3, 40.1785714285715],
                 [  4, 40.1785714285715],
                 [  5, 40.1785714285715],
                 [  6, 40.1785714285715],
                 [  7, 40.1785714285715],
                 [  8, 40.1785714285715],
                 [  9, 40.1785714285715],
                 [ 10, 63.9043280182233],
                 [ 11, 126.622807017544],
                 [ 12, 304.248322147651],
                 [ 13, 736.279310344828],
                 [ 14, 2286.05192307693],
                 [ 15, 7944.84761904762],
                 [ 16, 29599.9012096774],
                 [ 17, 105615.328918322],
                 [ 18, 414865.726436782],
                 [ 19, 1633914.22879177],
                 [ 20, 6987292.11888112],
                 [ 21, 35389914.6785714],
                 [ 22, 1000000100.00000],
                 [ 23, 1000000100.00000],
                 [ 24, 1000000100.00000],
                 [ 25, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S750 = [ [ -4, 48.1966873706004],
                 [ -3, 48.1966873706004],
                 [ -2, 48.1966873706004],
                 [ -1, 48.1966873706004],
                 [  0, 48.1966873706004],
                 [  1, 48.1966873706004],
                 [  2, 48.1966873706004],
                 [  3, 48.1966873706004],
                 [  4, 48.1966873706004],
                 [  5, 48.1966873706004],
                 [  6, 48.1966873706004],
                 [  7, 48.1966873706004],
                 [  8, 48.1966873706004],
                 [  9, 48.1966873706004],
                 [ 10, 48.1966873706004],
                 [ 11, 84.2816399286988],
                 [ 12, 134.340000000000],
                 [ 13, 272.655813953488],
                 [ 14, 547.608294930877],
                 [ 15, 1245.75979112272],
                 [ 16, 2693.37240537241],
                 [ 17, 6361.93472222222],
                 [ 18, 16024.1343283582],
                 [ 19, 41393.1057692308],
                 [ 20, 109160.276190476],
                 [ 21, 346685.288025890],
                 [ 22, 1271710.57269504],
                 [ 23, 5327267.46153846],
                 [ 24, 5327267.46153846],
                 [ 25, 5327267.46153846],
                 [ 26, 1000000100.00000],
                 [ 27, 1000000100.00000],
                 [ 28, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM2NIBP6S900 = [ [ -4, 26.5134706814580],
                 [ -3, 26.5134706814580],
                 [ -2, 26.5134706814580],
                 [ -1, 26.5134706814580],
                 [  0, 26.5134706814580],
                 [  1, 26.5134706814580],
                 [  2, 26.5134706814580],
                 [  3, 26.5134706814580],
                 [  4, 26.5134706814580],
                 [  5, 26.5134706814580],
                 [  6, 26.5134706814580],
                 [  7, 26.5134706814580],
                 [  8, 26.5134706814580],
                 [  9, 26.5134706814580],
                 [ 10, 26.5134706814580],
                 [ 11, 26.5134706814580],
                 [ 12, 26.5134706814580],
                 [ 13, 40.8660130718954],
                 [ 14, 62.3696498054475],
                 [ 15, 99.5559006211181],
                 [ 16, 152.957377049180],
                 [ 17, 274.446982055465],
                 [ 18, 446.803809523810],
                 [ 19, 756.701046337817],
                 [ 20, 1464.75577557756],
                 [ 21, 2563.94590163934],
                 [ 22, 4405.39157566303],
                 [ 23, 7939.34801762114],
                 [ 24, 16002.2018867924],
                 [ 25, 28662.9024856597],
                 [ 26, 56701.2453608248],
                 [ 27, 131284.841336117],
                 [ 28, 287674.121457490],
                 [ 29, 628761.446088794],
                 [ 30, 1313266.15584415],
                 [ 31, 1313266.15584415],
                 [ 32, 1313266.15584415],
                 [ 33, 1313266.15584415],
                 [ 34, 1313266.15584415],
                 [ 35, 1313266.15584415],
                 [ 36, 1313266.15584415],
                 [ 37, 1313266.15584415],
                 [ 38, 1313266.15584415],
                 [ 39, 1313266.15584415],
                 [ 40, 1313266.15584415]
               ]
#### /ChannelModel 2


#### ChannelModel 3
rawCM3NIBP6S100 = [ [ -4, 188.673809523810],
                 [ -3, 188.673809523810],
                 [ -2, 188.673809523810],
                 [ -1, 188.673809523810],
                 [  0, 188.673809523810],
                 [  1, 1065.47198275862],
                 [  2, 10072.7076023392],
                 [  3, 142412.217721519],
                 [  4, 2755709.44262295],
                 [  5, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM3NIBP6S150 = [ [ -4, 28.4611528822055],
                 [ -3, 28.4611528822055],
                 [ -2, 28.4611528822055],
                 [ -1, 28.4611528822055],
                 [  0, 28.4611528822055],
                 [  1, 69.7374631268437],
                 [  2, 214.147727272727],
                 [  3, 996.381355932204],
                 [  4, 7741.27047146402],
                 [  5, 84133.2384823848],
                 [  6, 1428522.76217765],
                 [  7, 26075689.4666667],
                 [  8, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM3NIBP6S200 = [ [ -4, 18.2564102564103],
                 [ -3, 18.2564102564103],
                 [ -2, 18.2564102564103],
                 [ -1, 18.2564102564103],
                 [  0, 18.2564102564103],
                 [  1, 33.7637540453075],
                 [  2, 86.0272952853597],
                 [  3, 282.018433179724],
                 [  4, 1438.63224181360],
                 [  5, 9455.15333333333],
                 [  6, 96901.1373056995],
                 [  7, 1034733.72521246],
                 [  8, 13032904.8125000],
                 [  9, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM3NIBP6S300 = [ [ -4, 29.9979079497908],
                 [ -3, 29.9979079497908],
                 [ -2, 29.9979079497908],
                 [ -1, 29.9979079497908],
                 [  0, 29.9979079497908],
                 [  1, 29.9979079497908],
                 [  2, 29.9979079497908],
                 [  3, 29.9979079497908],
                 [  4, 59.3676814988290],
                 [  5, 163.954545454545],
                 [  6, 602.560570071259],
                 [  7, 2638.45454545454],
                 [  8, 12980.0286458333],
                 [  9, 94422.3858267716],
                 [ 10, 782861.013921114],
                 [ 11, 6786009.48275862],
                 [ 12, 45726686.5555556],
                 [ 13, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM3NIBP6S375 = [ [ -4, 39.5775401069519],
                 [ -3, 39.5775401069519],
                 [ -2, 39.5775401069519],
                 [ -1, 39.5775401069519],
                 [  0, 39.5775401069519],
                 [  1, 39.5775401069519],
                 [  2, 39.5775401069519],
                 [  3, 39.5775401069519],
                 [  4, 39.5775401069519],
                 [  5, 39.5775401069519],
                 [  6, 76.4690265486726],
                 [  7, 220.926829268293],
                 [  8, 687.289617486339],
                 [  9, 2707.78800856531],
                 [ 10, 10810.6746987952],
                 [ 11, 60380.5729442971],
                 [ 12, 325305.970588235],
                 [ 13, 2136980.40369393],
                 [ 14, 19988261.0000000],
                 [ 15, 87966881.8461538],
                 [ 16, 1000000100.00000],
                 [ 40, 1000000100.00000]
               ]
rawCM3NIBP6S600 = [ [ -4, 36.3593750000000],
                 [ -3, 36.3593750000000],
                 [ -2, 36.3593750000000],
                 [ -1, 36.3593750000000],
                 [  0, 36.3593750000000],
                 [  1, 36.3593750000000],
                 [  2, 36.3593750000000],
                 [  3, 36.3593750000000],
                 [  4, 36.3593750000000],
                 [  5, 36.3593750000000],
                 [  6, 36.3593750000000],
                 [  7, 36.3593750000000],
                 [  8, 36.3593750000000],
                 [  9, 36.3593750000000],
                 [ 10, 60.8125000000000],
                 [ 11, 107.114583333333],
                 [ 12, 194.156673114120],
                 [ 13, 460.944933920705],
                 [ 14, 1373.21731748727],
                 [ 15, 3715.10267857143],
                 [ 16, 11446.4624413146],
                 [ 17, 33741.2553699284],
                 [ 18, 113709.469273743],
                 [ 19, 303304.652661064],
                 [ 20, 890490.622739019],
                 [ 21, 2320781.43123543],
                 [ 22, 6848245.33566434],
                 [ 23, 19005907.8627451],
                 [ 24, 25507934.1315790],
                 [ 25, 64620124.2666667],
                 [ 40, 64620124.2666667]
               ]
rawCM3NIBP6S750 = [ [ -4, 41.9382239382239],
                 [ -3, 41.9382239382239],
                 [ -2, 41.9382239382239],
                 [ -1, 41.9382239382239],
                 [  0, 41.9382239382239],
                 [  1, 41.9382239382239],
                 [  2, 41.9382239382239],
                 [  3, 41.9382239382239],
                 [  4, 41.9382239382239],
                 [  5, 41.9382239382239],
                 [  6, 41.9382239382239],
                 [  7, 41.9382239382239],
                 [  8, 41.9382239382239],
                 [  9, 41.9382239382239],
                 [ 10, 41.9382239382239],
                 [ 11, 61.8273092369478],
                 [ 12, 86.0368932038835],
                 [ 13, 130.370297029703],
                 [ 14, 239.903563941300],
                 [ 15, 474.673566878981],
                 [ 16, 946.744615384615],
                 [ 17, 1812.23714759536],
                 [ 18, 3665.64473684211],
                 [ 19, 6855.99571428571],
                 [ 20, 13051.3679245283],
                 [ 21, 25063.0672389127],
                 [ 22, 46654.3282548476],
                 [ 23, 101574.229166667],
                 [ 24, 208789.697885196],
                 [ 25, 495439.534959350],
                 [ 26, 1215692.35623003],
                 [ 27, 3146636.84693878],
                 [ 28, 8009627.81168831],
                 [ 40, 8009627.81168831]
               ]
rawCM3NIBP6S900 = [ [ -4, 40.2927580893683],
                 [ -3, 40.2927580893683],
                 [ -2, 40.2927580893683],
                 [ -1, 40.2927580893683],
                 [  0, 40.2927580893683],
                 [  1, 40.2927580893683],
                 [  2, 40.2927580893683],
                 [  3, 40.2927580893683],
                 [  4, 40.2927580893683],
                 [  5, 40.2927580893683],
                 [  6, 40.2927580893683],
                 [  7, 40.2927580893683],
                 [  8, 40.2927580893683],
                 [  9, 40.2927580893683],
                 [ 10, 40.2927580893683],
                 [ 11, 40.2927580893683],
                 [ 12, 40.2927580893683],
                 [ 13, 40.2927580893683],
                 [ 14, 52.5072933549433],
                 [ 15, 77.5433526011562],
                 [ 16, 108.766114180479],
                 [ 17, 162.650623885918],
                 [ 18, 243.612654320988],
                 [ 19, 369.678846153846],
                 [ 20, 581.420338983050],
                 [ 21, 786.877516778524],
                 [ 22, 1114.80000000000],
                 [ 23, 1548.59550561798],
                 [ 24, 1941.16210526316],
                 [ 25, 2489.95604395604],
                 [ 26, 2996.99591002045],
                 [ 27, 3525.96481481481],
                 [ 28, 3932.35251798561],
                 [ 29, 4406.05490196078],
                 [ 30, 4684.44715447155],
                 [ 31, 5112.77868852459],
                 [ 32, 5517.50833333333],
                 [ 33, 5797.45905172414],
                 [ 34, 5986.87893462469],
                 [ 35, 6070.30000000000],
                 [ 36, 6204.36184210526],
                 [ 37, 6312.89655172414],
                 [ 38, 6279.64222222222],
                 [ 39, 6335.25155925156],
                 [ 40, 6724.38803088803]
               ]
#### /ChannelModel 3

class ErrorModelling(openwns.FUN.FunctionalUnit):
    """This class mappes the snr to pmean (mean error free gap length) and calculate PER

       self.PrintMappings:  if True, the Mapping Tables will be prinnted
                            at the initialization,
    """
    __plugin__ = 'wimemac.convergence.ErrorModelling'
    name = 'ErrorModelling'
    logger = None
    phyUserCommandName = None
    phyUserName = None
    managerCommandName = None
    managerName = None

    channelModel = None
    localIDs = None

    snr2pmean_100 = None
    snr2pmean_150 = None
    snr2pmean_200 = None
    snr2pmean_300 = None
    snr2pmean_375 = None
    snr2pmean_600 = None
    snr2pmean_750 = None
    snr2pmean_900 = None
    PrintMappings = False
    
    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self,
             name,
             commandName,
             phyUserCommandName,
             phyUserName,
             managerCommandName,
             managerName,
             channelModel,
             parentLogger = None, **kw):
        super(ErrorModelling, self).__init__(functionalUnitName=name, commandName=commandName)
        self.phyUserCommandName = phyUserCommandName
        self.phyUserName = phyUserName
        self.managerCommandName = managerCommandName
        self.managerName = managerName
        self.channelModel = channelModel
        self.localIDs = {}
        self.logger = wimemac.Logger.Logger("ErrorModelling", parent = parentLogger)
        
        if self.channelModel == 1:
                self.snr2pmean_100 = SNR2PmeanMapping(rawCM1NIBP6S100)
                self.snr2pmean_150 = SNR2PmeanMapping(rawCM1NIBP6S150)
                self.snr2pmean_200 = SNR2PmeanMapping(rawCM1NIBP6S200)
                self.snr2pmean_300 = SNR2PmeanMapping(rawCM1NIBP6S300)
                self.snr2pmean_375 = SNR2PmeanMapping(rawCM1NIBP6S375)
                self.snr2pmean_600 = SNR2PmeanMapping(rawCM1NIBP6S600)
                self.snr2pmean_750 = SNR2PmeanMapping(rawCM1NIBP6S750)
                self.snr2pmean_900 = SNR2PmeanMapping(rawCM1NIBP6S900)
        
        elif self.channelModel == 2:
                self.snr2pmean_100 = SNR2PmeanMapping(rawCM2NIBP6S100)
                self.snr2pmean_150 = SNR2PmeanMapping(rawCM2NIBP6S150)
                self.snr2pmean_200 = SNR2PmeanMapping(rawCM2NIBP6S200)
                self.snr2pmean_300 = SNR2PmeanMapping(rawCM2NIBP6S300)
                self.snr2pmean_375 = SNR2PmeanMapping(rawCM2NIBP6S375)
                self.snr2pmean_600 = SNR2PmeanMapping(rawCM2NIBP6S600)
                self.snr2pmean_750 = SNR2PmeanMapping(rawCM2NIBP6S750)
                self.snr2pmean_900 = SNR2PmeanMapping(rawCM2NIBP6S900)
        
        elif self.channelModel == 3:
                self.snr2pmean_100 = SNR2PmeanMapping(rawCM3NIBP6S100)
                self.snr2pmean_150 = SNR2PmeanMapping(rawCM3NIBP6S150)
                self.snr2pmean_200 = SNR2PmeanMapping(rawCM3NIBP6S200)
                self.snr2pmean_300 = SNR2PmeanMapping(rawCM3NIBP6S300)
                self.snr2pmean_375 = SNR2PmeanMapping(rawCM3NIBP6S375)
                self.snr2pmean_600 = SNR2PmeanMapping(rawCM3NIBP6S600)
                self.snr2pmean_750 = SNR2PmeanMapping(rawCM3NIBP6S750)
                self.snr2pmean_900 = SNR2PmeanMapping(rawCM3NIBP6S900)
        
        else:
                print "ChannelModel does not exist or is not implemented: " , self.channelModel
                assert self.channelModel in (1,2,3)
        
        attrsetter(self, kw)

