#coding:utf-8
import subprocess


def startSimulation(sceneName,sceneType,unity = False,parallel:bool = False):
    pid = 0
    if(parallel == False):
        if(unity == False):
            #运行menge仿真
            process = subprocess.Popen(
                "D:\File\Project\git\Menge-0.9.2\Exe\menge.exe -p D:\File\Project\git\Menge-0.9.2\examples\\" + sceneName + "\\" + sceneName + sceneType + ".xml",
                shell=True, stdout=False)
            pid = process.pid
        else:
            pass
            #打开Unity程序
    #平行模式
    else:
        if(unity == False):
            # 运行menge仿真
            process = subprocess.Popen(
                "D:\File\Project\git\Menge-0.9.2\Exe\menge.exe -p D:\File\Project\git\Menge-0.9.2\examples\\"+ sceneName+"\\"+ sceneName +"Parallel.xml",
                shell=True, stdout=False)
            pid = process.pid
        else:
            pass
    # 打开Unity程序
    print(sceneName + " simulation start, pid: "+ str(pid))
    return pid