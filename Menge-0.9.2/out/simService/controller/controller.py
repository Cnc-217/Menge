# coding:utf-8
from simService.services.mengeDataService import *
from simService import sims,redisServer
from flask import Blueprint, request
import time

controller = Blueprint('controller', __name__)


@controller.route('/')
def hello():
    return 'Welcome to MengeSimService!'

# 开始指定类型的程序仿真，与menge建立socket连接，启动后对menge参数进行初始化
# http://10.28.195.233:5000/olympic/startService
@controller.route('/<sceneName>/startService',methods=['POST'])
def simStart(sceneName):
    #初始化仿真对象，获取运行pid，等待menge端socket连接本服务，与自己的仿真参数进行同步
    sceneType = request.values.get("sceneType")#model matrix
    runType = request.values.get("runType")#menge unity
    if(runType == "menge"):
        sim = sims.setNewSimulation(sceneName, sceneType, False)
    elif(runType == "unity"):
        sim = sims.setNewSimulation(sceneName, sceneType, True)
    else:
        info = "error, you have to set right runType: menge/unity"
        jsonData = {"info": info}
        return jsonData
    # 将仿真参数发送到menge
    sim.simRun()
    parameterSynToMenge(sim)
    info = sim.getSceneName() + " simulation start complete"
    jsonData = {"info": info, "pid": str(sim.getPid())}
    return jsonData

# 开始一个平行的仿真，可能是unity平行，可能是Menge程序平行
# http://10.28.195.233:5000/startParallel
@controller.route('/startParallel',methods=['POST'])
def simParallelStart():
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            timeSliceSave(sim)
            simulationPause(sim)
            #将已存在的仿真里的变量复制到新仿真对象中
            simNew = sims.getParallelSimulation(sim)
            simNew.simRun()
            parameterSynToMenge(simNew)
            info = simNew.getSceneName() + " parallel simulation start complete"
            jsonData = {"info": info, "pid": str(simNew.getPid())}
            return jsonData
    if (info == ""):
        info = "no " + simPid + " simulation is running"
        jsonData = {"info": info}
        return jsonData

# 终止指定类型的程序仿真
# http://10.28.195.233:5000/stopService
@controller.route('/stopService',methods=['POST'])
def simStop():
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid())==simPid:
            redisServer.delete(sim.getPid())
            sims.stopSimulation(sim.getPid())
            info = simPid + " simulation stop complete"
    if(info == ""):
        info =  "no "+simPid+ " simulation is running"
    jsonData = {"info": info}
    return jsonData

# 暂停指定pid的仿真，可能是unity暂停，可能是Menge程序暂停
# http://10.28.195.233:5000/pauseService
@controller.route('/pauseService', methods=['POST'])
def simPause():
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            simulationPause(sim)
            info = sim.getSceneName() + " parallel simulation pause"
    if (info == ""):
        info = "no " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData

# 解除暂停，可能是unity解除，可能是Menge解除
# http://10.28.195.233:5000/restartService
@controller.route('/restartService', methods=['POST'])
def simRestart():
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            simulationRestart(sim)
            info = sim.getSceneName() + " restart simulation"
    if (info == ""):
        info = "no " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData


# 接收action，修改sim对象中scene对象的参数，socket方式将参数发送给Menge进行数据同步
#http://10.28.195.233:5000/olympic/action?actionType=flow&actionList=[1,2,35]
@controller.route('/<sceneName>/action', methods=['POST'])
def actionApply(sceneName):
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            actionType = request.values.get("actionType")
            if(actionType==None):
                return "actionType is None"
            if(actionType=="flow"):
                actionList = request.values.get("actionList")
                actionList = actionList.strip('[').strip(']').split(',')
                actionList = [int(num) for num in actionList]
                sim.getScene().updateParameter(actionType,list(actionList))
                parameterSynToMenge(sim)
                info =  sim.getSceneName() + " update parameter complete"
            else:
                info =  "actionType wrong"
    if (info == ""):
        info = "no " + sceneName + ": " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData

# 执行疏散
# http://10.28.195.233:5000/olympic/evacuate
@controller.route('/<sceneName>/evacuate', methods=['POST'])
def evacuateApply(sceneName):
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            evacuateStart(sim)
            info =  sim.getSceneName() + " evacuate start complete"
    if (info == ""):
        info = "no " + sceneName + ": " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData

# 查询路障信息
# http://10.28.195.233:5000/olympic/blockCheck
@controller.route('/<sceneName>/blockCheck', methods=['POST'])
def checkRoadBlock(sceneName):
    simPid = request.values.get("pid")
    block = []
    find = False
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            block = sim.getScene().checkRoadBlock()
            find = True
            break
    if (find == False):
        info = "no " + sceneName + ": " + simPid + " simulation is running"
    else:
        if(len(block)==0):
            info =  simPid + " has no block setting now"
        else:
            returnstr = 'block index are: '
            for i in block:
                returnstr = returnstr + str(i) +' '
            info = returnstr
    jsonData = {"info": info}
    jsonStr = json.dumps(jsonData)
    return jsonStr

# 设置路障
# http://10.28.195.233:5000/olympic/blockSet
@controller.route('/<sceneName>/blockSet', methods=['POST'])
def setRoadBlock(sceneName):
    simPid = request.values.get("pid")
    blockList = request.values.get("blockList")
    blockList = blockList.strip('[').strip(']').split(',')
    blockList = [int(num) for num in blockList]
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            sim.getScene().updateRoadBlock(blockList)
            parameterSynToMenge(sim)
            time.sleep(0.5)
            setUnityBlock(sim)
            info =  sim.getSceneName() + " update parameter complete"
    if (info == ""):
        info = "no " + sceneName + ": " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData

# 存储即时数据到XML
# http://10.28.195.233:5000/olympic/timeSlice
@controller.route('/<sceneName>/timeSlice', methods=['POST'])
def saveTimeSlice(sceneName):
    simPid = request.values.get("pid")
    info = ""
    for sim in sims.getSimulationList():
        if str(sim.getPid()) == simPid:
            sim = sims.getSimulation(sim.getPid())
            timeSliceSave(sim)
            info =  sim.getSceneName() + " save timeSlice to xml complete"
    if (info == ""):
        info = "no " + sceneName + ": " + simPid + " simulation is running"
    jsonData = {"info": info}
    return jsonData