
# from services.timeService import *
from resources.xmlTool import *

from flask import Blueprint, request
controllerXML = Blueprint('controllerXML', __name__)

@controllerXML.route('/xmlTest', methods=['POST'])
def xmlTest(sceneName):
    tree = ET.parse("..\..\examples\Olympic\OlympicS.xml")#相对路径
    root = tree.getroot()

    sceneType = request.values.get("sceneType")  # model matrix
    runType = request.values.get("runType")  # menge unity
    agentName = request.values.get("agentName")# tourist leader xxxx
    generatorType = request.values.get("generatorType")#rect  explicit

    if generatorType == "rect":
        xyCount = request.values.get("xyCount").strip('[').strip(']').split(',')
        print(xyCount)
        info = ""
        agentGroupNode = [node for node in root.iter("AgentGroup")
                          if node.find("ProfileSelector").attrib["name"] == agentName][0]

        #print(agentGroupNode.getElementsByTagName("Generator")[0].getAttribute("count_x"))
        generatorNode = agentGroupNode.find("Generator")
        generatorNode.set("count_x" , xyCount[0])
        generatorNode.set("count_y" , xyCount[1])
        #generatorNode.set("YES" , "YES")
        generatorNode.set("type" , "rect_grid")

    elif generatorType == "explicit":
        agentPositions = request.values.get("agentPositions").split(',')
        agentPositions = [xy.strip("[").strip("]") for xy in agentPositions]
        print(agentPositions)

        positions = []
        for i in range(0, len(agentPositions), 2):
            positions.append(agentPositions[i:i+2])
        print(positions)

        agentGroupNode = [node for node in root.iter("AgentGroup")
                          if node.find("ProfileSelector").attrib["name"] == agentName][0]
        generatorNode = agentGroupNode.find("Generator")
        generatorNode.set("type" , generatorType)

        for i in range(len(positions)):
            subElement(generatorNode,"Agent",positions[i][0],positions[i][1])
    else:
        print("parameter error ! generatorType should be explicit or rect!")

    saveXML(root, "..\..\examples\Olympic\OlympicS.xml", indent="", newl="")
    #tree = ET.ElementTree(root)
    #tree.write("E:\git\men\Menge\Menge-0.9.2\examples\Olympic\OlympicS.xml", encoding="utf-8", xml_declaration=True)

    if (runType == "menge"):
        sim = sims.setNewSimulation(sceneName, sceneType, False)
    elif (runType == "unity"):
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



@controllerXML.route('/xmlTest2', methods=['POST'])
def xmlTest2(sceneName):
    sceneType = request.values.get("sceneType")  # model matrix
    runType = request.values.get("runType")  # menge unity

    stateParaList = request.values.get("stateParaList").split(",")
    transitionParaList = request.values.get("transitionParaList").split(",")
    transitionParaList = [keyvalve.strip("[").strip("]") for keyvalve in transitionParaList]
    addGoalSelector = request.values.get("addGoalSelector").split(",")
    addGoalSelector = [keyValve.strip("[").strip("]") for keyValve in addGoalSelector]
    addGoalSelectorList = []
    for i in range(0, len(addGoalSelector), 2):
        addGoalSelectorList.append(addGoalSelector[i:i + 2])#转成list of list

    addVelComponent = request.values.get("addVelComponent").split(",")
    addVelComponent = [keyValve.strip("[").strip("]") for keyValve in addVelComponent]
    addVelComponentList = []
    for i in range(0, len(addVelComponent), 2):
        addVelComponentList.append(addVelComponent[i:i + 2])

    stateName = stateParaList[0]#state的名字
    final = stateParaList[1]#final
    GoalSelectorType = stateParaList[2]
    VelComponentType = stateParaList[3]

    if sceneType == "model":
        treeDir = "..\..\examples\Olympic\Olympic"+"Model"+"B.xml"
    elif sceneType == "matrix":
        treeDir = "..\..\examples\Olympic\Olympic"+"Matrix"+"B.xml"
    else:
        info = "error, you have to set right sceneType: model/matrix"
        jsonData = {"info": info}
        return jsonData
    treeB = ET.parse(treeDir)  # 相对路径
    rootB = treeB.getroot()

    stateNodes = [node for node in rootB.iter("State") if node.attrib["name"] == stateName]
    if len(stateNodes) == 0:#没有此节点  新增模式
        ele = ET.SubElement(rootB, "State")
        ele.set("name", stateName)
        ele.set("final", final)

        GoalSelectorNode = ET.SubElement(ele,"GoalSelector")
        GoalSelectorNode.set("type",GoalSelectorType)
        for i in range(len(addGoalSelectorList)):
            GoalSelectorNode.set(addGoalSelectorList[i][0],addGoalSelectorList[i][1])

        VelComponentNode = \
            ET.SubElement(ele,"VelComponent")
        VelComponentNode.set("type",VelComponentType)
        for j in range(len(addVelComponentList)):
            VelComponentNode.set(addVelComponentList[j][0],addVelComponentList[j][1])

        ele.tail = '\n\t\t'
        VelComponentNode.tail = '\n\t\t'
        GoalSelectorNode.tail = '\n\t\t'

    else:#有此节点  修改模式
        stateNode = stateNodes[0]
        stateNode.set("name", stateName)
        stateNode.set("final", final)
        GoalSelectorNode = stateNode.find("GoalSelector")
        GoalSelectorNode.set("type",GoalSelectorType)

        for i in range(len(addGoalSelectorList)):
            GoalSelectorNode.set(addGoalSelectorList[i][0],addGoalSelectorList[i][1])

        VelComponentNode = stateNode.find("VelComponent")
        VelComponentNode.set("type",VelComponentType)
        for j in range(len(addVelComponentList)):
            VelComponentNode.set(addVelComponentList[j][0],addVelComponentList[j][1])
    transitionNodes = [node for node in rootB.iter("Transition")
                       if node.attrib["from"] == transitionParaList[0] and node.attrib["to"] == transitionParaList[1]]
    if len(transitionNodes) == 0:
        ele = ET.SubElement(rootB, "Transition")
        ele.set("from", transitionParaList[0])
        ele.set("to", transitionParaList[1])

        ConditionNode = ET.SubElement(ele, "Condition")
        ConditionNode.set("type", transitionParaList[2])
        ConditionNode.set("distance","5")
    else:
        transitionNode = transitionNodes[0]
        transitionNode.set("from", transitionParaList[0])
        transitionNode.set("to", transitionParaList[1])
        ConditionNode = transitionNode.find("Condition")
        ConditionNode.set("type", transitionParaList[2])

    saveXML(rootB, treeDir, indent="", newl="")

    if (runType == "menge"):
        sim = sims.setNewSimulation(sceneName, sceneType, False)
    elif (runType == "unity"):
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