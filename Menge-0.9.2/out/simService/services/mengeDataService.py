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
def evacuateStart(sim,algorithmID):
    client = sim.getClient()
    jsonData = {"info": "evacuate", "data": algorithmID}
    socketSend(jsonData, client)

#命令Menge存储时间切片的数据到XML
#json={"info"="evacuate"}
def timeSliceSave(sim):
    client = sim.getClient()
    jsonData = {"info": "timeSlice", "data": ""}
    socketSend(jsonData, client)

#暂停
#json={"info"="pause"}
def simulationPause(sim):
    jsonData = {"info": "pause", "data": ""}
    #发送给C++的socket接收程序是需要交互两次的，发送给c#的因为数据小，不考虑粘包问题
    if(sim.isUnitySim()==True):
        client = sim.getUnityClient()
        messageSend = json.dumps(jsonData)
        client.sendall(messageSend.encode("utf8"))
    else:
        client = sim.getClient()
        socketSend(jsonData, client)

#解除暂停
#json={"info"="restart"}
def simulationRestart(sim):
    jsonData = {"info": "restart", "data": ""}
    if (sim.isUnitySim() == True):
        client = sim.getUnityClient()
        messageSend = json.dumps(jsonData)
        client.sendall(messageSend.encode("utf8"))
    else:
        client = sim.getClient()
        socketSend(jsonData, client)

#Unity停止仿真
#json={"info"="restart"}
def simulationStop(unityClient):
    jsonData = {"info": "stop", "data": ""}
    messageSend = json.dumps(jsonData)
    unityClient.sendall(messageSend.encode("utf8"))

#命令unity新路障贴图信息更
#json={"info"="setBlock","data"=blockList}
def setUnityBlock(sim):
    #从menge获取block坐标
    client = sim.getClient()
    jsonData = {"info": "getBlockPosition", "data": ""}
    socketSend(jsonData, client)
    rawData = socketRecive(client)
    jsonData = json.loads(rawData)
    #print(jsonData)
    #发送给unity
    messageSend = json.dumps(jsonData)
    unityClient = sim.getUnityClient()
    unityClient.sendall(messageSend.encode("utf8"))

