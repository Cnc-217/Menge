#coding:utf-8
from simService import sim,redisServer
from simService.services.mengeDataService import getDataFromMenge
import json

def dataSynToRadis():
    if(sim.getSceneName()=="olympic"):
        rawData = getDataFromMenge(sim)
        #list类型
        jsonRecive = json.loads(rawData)
        jsonStr = json.dumps(jsonRecive)
        redisServer.set("data",jsonStr)
        print("collect data to Redis")

def dataCollectToRedis():
    if(sim.isRunning==True):
        dataSynToRadis()
    else:
        print("simulation is not running")