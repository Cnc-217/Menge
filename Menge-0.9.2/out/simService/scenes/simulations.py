#coding:utf-8
from simService.scenes.simulation import Simulation

class Simulations:
    def __init__(self):
        self.__simulations = []#存储simulation的list,存储的是sim对象


    def setNewSimulation(self,sceneName, sceneType,unity = False):
        sim = Simulation(sceneName,sceneType,unity)
        self.__simulations.append(sim)
        return sim

    def getParallelSimulation(self,sim:Simulation):
        simNew = sim.getParallelSim()
        self.__simulations.append(simNew)
        return simNew

    def stopSimulation(self,pid):
        for sim in self.__simulations:
            if sim.getPid()==pid:
                sim.stopSimulation()
                self.__simulations.remove(sim)
                break

    def getSimulation(self,pid):
        for sim in self.__simulations:
            if sim.getPid()==pid:
                return sim

    def getSimulationList(self):
        return self.__simulations

