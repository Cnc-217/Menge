#coding:utf-8
from services.simlationControl import startSimulation
from simService.scenes.scene import *
from simService.socket.socketServer import socketServer
from simService.services.mengeDataService import *
import os

class Simulation:
    def __init__(self, sceneName, sceneType,unity:bool = False, parallel:bool = False):
        self.__scene = None
        self.__client = None
        self.__unity = unity #true false
        self.__parallel = parallel #true false
        self.__unityClient = None
        self.__processPid = None #运行Unity程序的时候pid是unity程序的pid，单独运行menge是menge的pid
        if (sceneName == "olympic"):
            if(sceneType == "matrix"):
                self.__scene = OlympicMatrix()
            elif(sceneType == "model"):
                self.__scene = OlympicModel()

    def simRun(self):
        if (self.isUnitySim() == True):
            self.__processPid = startSimulation(self.__scene.getSceneName(), self.__scene.getSceneType(), True,self.isParallel())
            self.__client, caddr = socketServer.getSocket().accept()
            self.__unityClient, caddr = socketServer.getSocket().accept()
        else:
            self.__processPid = startSimulation(self.__scene.getSceneName(), self.__scene.getSceneType(), False,self.isParallel())
            # 等待Menge的socketClient连接本服务
            self.__client, caddr = socketServer.getSocket().accept()

    def getSceneName(self):
        return self.__scene.getSceneName()

    def getScene(self):
        return self.__scene

    def getClient(self):
        return self.__client

    def getUnityClient(self):
        return self.__unityClient

    def getPid(self):
        return self.__processPid

    def stopSimulation(self):
        if(self.isUnitySim()==True):
            simulationStop(self.__unityClient)
        else:
            os.system("taskkill /pid " + str(self.__processPid) + " -t -f")
        self.__scene = None
        self.__client = None
        self.__unityClient = None
        self.__processPid = None
        self.__sceneType = None

    def isUnitySim(self):
        return self.__unity

    def isParallel(self):
        return self.__parallel

    def getParallelSim(self):
        simNew = Simulation(self.__scene.getSceneName(),self.__scene.getSceneType(),self.isUnitySim(),True)
        simNew.__scene = self.__scene
        return simNew