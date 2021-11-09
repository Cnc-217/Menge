#coding:utf-8
from simService.scenes.olympic import Olympic
import os

class Simulation:
    def __init__(self, scene=None, client=None, processPid = None):
        self.__scene = scene
        self.__client = client
        self.__processPid = processPid
        self.isRunning = False

    def setScene(self,sceneName):
        if(sceneName=="olympic"):
            olympic = Olympic()
            self.__scene = olympic
            self.isRunning = True

    def getSceneName(self):
        return self.__scene.getSceneName()

    def getScene(self):
        return self.__scene

    def setClient(self,client):
        self.__client = client

    def getClient(self):
        return self.__client

    def setPid(self,processPid):
        self.__processPid = processPid

    def getPid(self):
        return self.__processPid

    def stopSimulation(self):
        os.system("taskkill /pid " + str(self.__processPid) + " -t -f")
        self.__scene = None
        self.__client = None
        self.__processPid = None
        self.isRunning = False