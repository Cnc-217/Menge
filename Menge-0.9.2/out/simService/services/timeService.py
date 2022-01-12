import time

import simService
from simService import sims
from simService.services.mengeDataService import parameterSynToMenge

def matrixUpdate():
    if (len(sims.getSimulationList())==0 or simService.TRAIN == False):
        pass
    else :
        for sim in sims.getSimulationList():
            if sim.getScene().getSceneType()=="Matrix" and len(sim.action)!=0:
                sim.getScene().updateParameter("flow", sim.action)
                print("sim.action: "+str(sim.action))
                parameterSynToMenge(sim)
                sim.action = []
                time.sleep(0.2)

