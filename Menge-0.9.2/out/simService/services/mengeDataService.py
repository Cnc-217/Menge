#coding:utf-8
from services.socketService import *

#将Menge的参数与服务器同步
#json={"info"="updateParameter","data"=matrix}
#接收json={"info"="xxxxx"}
def parameterSynToMenge(sim):
   scene = sim.getScene()
   sceneName = sim.getScene().getSceneName()
   parameter = scene.getParameter()
   dataSend = ''
   if(sceneName=="olympic"):
       dataSend = matrixToStr(parameter)
   jsonData = {"info": "parameters", "data": dataSend}
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

def evacuateStart(sim):
    client = sim.getClient()
    jsonData = {"info": "evacuate", "data": ""}
    socketSend(jsonData, client)


def matrixToStr(matrix):
    # 将矩阵转换为为字符串
    matrixStr = ""
    rowNum = len(matrix)
    columnNUm = len(matrix[0])
    sumNum = 0
    for i in matrix:
        for j in i:
            matrixStr = matrixStr + str(j) + " "
            sumNum = sumNum + 1
    if (sumNum != rowNum ** 2 or rowNum != columnNUm):
        print("error!!!")
        exit()
    return matrixStr