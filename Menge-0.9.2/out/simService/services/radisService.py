#coding:utf-8
from simService import redisServer, sims
from simService.services.mengeDataService import getDataFromMenge
from simService.resources.error import errorConnectionResetError
import json

def dataSynToRadis():
    for sim in sims.getSimulationList():
        try:
            rawData = getDataFromMenge(sim)
            # list类型
            jsonRecive = json.loads(rawData)
            profitList = [500, 500, 500,  # 滑雪
                          200, 250, 150, 200, 100, 100, 50,  # 饭店
                          0,  # 广场
                          100, 50,  # 纪念品
                          400, 300,  # 雪具
                          100, 80, 80, 120, 50,  # 复仇者
                          200, 300, 250, 100, 300,  # 服装店
                          200, 400, 200,  # 冰雪
                          0, 0, 0, 0, 0, 0, 0, 0]
            jsonRecive["profit"] = [a * b for a, b in zip(jsonRecive["data"], profitList)]
            jsonStr = json.dumps(jsonRecive)
            redisServer.set(sim.getPid(), jsonStr)
            print("collect sim pid:" + str(sim.getPid()) + " data to Redis")
        except ConnectionResetError:
            errorConnectionResetError(sim)
            print("menge crush, sim and redis job deleted")



def dataCollectToRedis():
    if (len(sims.getSimulationList())==0):
        pass
    else :
        dataSynToRadis()


