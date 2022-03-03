import time

import simService
from simService import sims,scheduler
from simService.services.mengeDataService import parameterSynToMenge

# def matrixUpdate():
#     if (len(sims.getSimulationList())==0 or simService.TRAIN == False):
#         pass
#     else :
#         for sim in sims.getSimulationList():
#             if sim.getScene().getSceneType()=="Matrix" and len(sim.action)!=0:
#                 sim.getScene().updateParameter("flow", sim.action)
#                 print("sim.action: "+str(sim.action))
#                 parameterSynToMenge(sim)
#                 sim.action = []
#                 time.sleep(0.2)

def matrixUpdate(sim,actionType):
    if sim.getScene().getSceneType()=="Matrix" and len(sim.getScene().action)!=0:
        sim.getScene().updateParameter(actionType, sim.getScene().action)
        parameterSynToMenge(sim)
        sim.getScene().action = []
        time.sleep(0.2)

def matrixReset(sim):
    print("matrix reset")
    if sim.getScene().getSceneType()=="Matrix":
        sim.action = []
        sim.getScene().matrixReset()
        parameterSynToMenge(sim)
        time.sleep(0.2)

def timer(n,sim):
    if (scheduler.get_job("1234") == None):
        print("reset will at :"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time()+n)))
        scheduler.add_job(func=matrixReset, args=[sim],id="1234", trigger="date", run_date=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time()+n)))

