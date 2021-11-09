# coding:utf-8
from simService.services.simlationControl import *
from simService.services.mengeDataService import *
from simService import sim,socketServer
from flask import Blueprint, request
from simService.resources.actions import *

controller = Blueprint('controller', __name__)


@controller.route('/')
def hello():
    return 'Welcome to MengeSimService!'


# 开始指定类型的程序仿真，与menge建立socket连接，启动后对menge参数进行初始化
# http://10.28.195.233:5000/olympic/startService
@controller.route('/<sceneName>/startService',methods=['POST'])
def simStart(sceneName):
    if (sim.isRunning ==True):
        return sceneName+" simulation is alreadly in running"
    #初始化仿真对象，获取运行pid，等待menge端socket连接本服务，与自己的仿真参数进行同步
    sim.setScene(sceneName)
    pid = startSimulation(sceneName)
    sim.setPid(pid)
    #等待Menge的socketClient连接本服务,如侦测到client，将clientSocket放入sim对象中
    client, caddr = socketServer.getSocket().accept()
    sim.setClient(client)
    #将仿真参数发送到menge
    parameterSynToMenge(sim)
    return sim.getSceneName() + " simulation start complete"


# 终止指定类型的程序仿真
# http://10.28.195.233:5000/olympic/stopService
@controller.route('/<sceneName>/stopService',methods=['POST'])
def simStop(sceneName):
    if(sim.isRunning==False):
        return "no " + sceneName + " simulation is running"
    sim.stopSimulation()
    return sceneName+" simulation stop complete"

# 开始指定场景的Unity仿真
# http://10.28.195.233:5000/olympic/startServiceUnity
@controller.route('/<sceneName>/startServiceUnity')
def simStartUnity(sceneName):
    sim.setScene(sceneName)
    return sim.getSceneName() + " simulation start complete"


# 接收action，修改sim对象中scene对象的参数，socket方式将参数发送给Menge进行数据同步
#http://10.28.195.233:5000/olympic/action?actionType=flow&actionList=[1,2,35]
@controller.route('/<sceneName>/action', methods=['POST'])
def actionApply(sceneName):
    if (sim.isRunning == False):
        return "no " + sceneName + " simulation is running"
    actionType = request.values.get("actionType")
    if(actionType==None):
        return "actionType is None"
    if(actionType=="flow"):
        actionList = request.values.get("actionList")
        actionList = actionList.strip('[').strip(']').split(',')
        actionList = [int(num) for num in actionList]
        sim.getScene().updateParameter(flowAction,list(actionList))
        parameterSynToMenge(sim)
        return sim.getSceneName() + " update parameter complete"
    else:
        return "actionType wrong"


# 执行疏散
# http://10.28.195.233:5000/olympic/evacuate
@controller.route('/<sceneName>/evacuate', methods=['POST'])
def evacuateApply(sceneName):
    if (sim.isRunning == False):
        return "no " + sceneName + " simulation is running"
    evacuateStart(sim)
    return sim.getSceneName() + " evacuate start complete"