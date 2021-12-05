#coding:utf-8
from scenes.simulation import Simulation
from services.socketService import *
from resources.tool import *

#将Menge的参数与服务器同步
#json={"info"="updateParameter","data"=matrix}
#接收json={"info"="xxxxx"}
def parameterSynToMenge(sim:Simulation):
   scene = sim.getScene()
   sceneName = scene.getSceneName()
   sceneType = scene.getSceneType()
   jsonData = {}
   if(sceneName=="Olympic"):
       if(sceneType=="Matrix"):
           matrix, roadblock = scene.getParameter()
           jsonData = {"info": "parameters", "matrix": matrixToStr(matrix),"roadblock": listToStr(roadblock)}
       elif (sceneType == "Model"):
           influence, roadblock = scene.getParameter()
           jsonData = {"info": "parameters", "influence": listToStr(influence),"roadblock": listToStr(roadblock)}
   #向Menge发送参数信息
   socketSend(jsonData,sim.getClient())

#得到Menge的仿真数据
#json={"info"="getData"}
#接收json={"info"="xxxxx","data"=vector}
def getDataFromMenge(sim):
    client = sim.getClient()
    jsonData = {"info": "getData", "data": ""}
    socketSend(jsonData, client)
    rawData = socketRecive(client)
    return rawData

#命令Menge开始疏散
#json={"info"="evacuate"}
def evacuateStart(sim):
    client = sim.getClient()
    jsonData = {"info": "evacuate", "data": ""}
    socketSend(jsonData, client)

#命令Menge存储时间切片的数据到XML
#json={"info"="evacuate"}
def timeSliceSave(sim):
    client = sim.getClient()
    jsonData = {"info": "timeSlice", "data": ""}
    socketSend(jsonData, client)

#命令Menge暂停
#json={"info"="pause"}
def simulationPause(sim):
    if(sim.isUnitySim()==True):
        client = sim.getUnityClient()
    else:
        client = sim.getClient()
    jsonData = {"info": "pause", "data": ""}
    socketSend(jsonData, client)

#命令Menge解除暂停
#json={"info"="restart"}
def simulationRestart(sim):
    if (sim.isUnitySim() == True):
        client = sim.getUnityClient()
    else:
        client = sim.getClient()
    jsonData = {"info": "restart", "data": ""}
    socketSend(jsonData, client)