libname = 'wimemac--main--0.1'

srcFiles = [
    'src/WiMeMAC--main--0.1Module.cpp',
    'src/SimulationModel.cpp',
    ]

hppFiles = [
    'src/WiMeMAC--main--0.1Module.hpp',
    'src/SimulationModel.hpp',
    ]

pyconfigs = [
    'wimemac--main--0.1/__init__.py',
    'wimemac--main--0.1/simulationmodel.py',
]

dependencies = []
# Put in any external lib here as you would pass it to a -l compiler flag, e.g.
# dependencies = ['boost_date_time']
Return('libname srcFiles hppFiles pyconfigs dependencies')
