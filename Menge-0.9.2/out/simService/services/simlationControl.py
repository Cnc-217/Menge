#coding:utf-8
import subprocess


def startSimulation(sceneName):
    #运行menge仿真
    process = subprocess.Popen(
        "D:\File\Project\git\Menge-0.9.2\Exe\menge.exe -p D:\File\Project\git\Menge-0.9.2\examples\\"+sceneName+"\\"+sceneName+".xml",
        shell=True, stdout=False)
    pid = process.pid
    print(sceneName + " simulation start, pid: "+ str(pid))
    return pid