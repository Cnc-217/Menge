#coding:utf-8
from simService import redisServer, sims
from simService.services.mengeDataService import getDataFromMenge
import json

def dataSynToRadis():
    for sim in sims.getSimulationList():
        rawData = getDataFromMenge(sim)
        #list类型
        jsonRecive = json.loads(rawData)
        jsonStr = json.dumps(jsonRecive)
        redisServer.set(sim.getPid(),jsonStr)
        print ("collect sim pid:"+str(sim.getPid())+" data to Redis")

def dataCollectToRedis():
    if (len(sims.getSimulationList())==0):
        pass
    else :
        dataSynToRadis()
