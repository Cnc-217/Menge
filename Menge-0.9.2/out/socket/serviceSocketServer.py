#coding:utf-8
import os
import socket
import time
import subprocess
import json
import threading

def socketAccept(s):
    print ("\nwait for pythonClient")
    c, caddr = s.accept()
    data = c.recv(1024).decode("utf8")
    return data,c,caddr

def simulation(client,sleepTime,projectname):
    #运行menge仿真
    print("run "+projectname+" simulation")                          
    process = subprocess.Popen(
        "D:\File\Project\git\Menge-0.9.2\Exe\menge.exe -p D:\File\Project\git\Menge-0.9.2\examples\\"+projectname+"\\"+projectname+".xml -x D:\File\Project\git\Menge-0.9.2\examples\\"+projectname+"\\matrix.txt",
        shell=True, stdout=False)
    pid = process.pid
    #menge定时运行
    if(sleepTime>0):
        print("simulation will be stoped after "+str(sleepTime)+" seconds")
        time.sleep(sleepTime)
        os.system("taskkill /pid " + str(pid) + " -t -f")
        reply = projectname +" simulation over"
        print(reply)
    #menge无限时间运行，接收来自client的stop信号
    else:
        while True:
            print("waiting for the stop signal")
            try:
                stopSignal = client.recv(1024).decode("utf8")
            except ConnectionResetError:
                print("The connection is dead")
            os.system("taskkill /pid " + str(pid) + " -t -f")
            replay = projectname+" simulation over"
            break

    #运行完毕后回复client
    try:
        client.send(replay.encode("utf8"))
    except ConnectionResetError:
        print("The connection is dead")
    client.close()

def service( ):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = 12348
    server.bind(('10.28.195.233', port)) #本机mobile
    #server.bind(('10.128.254.80', port))  #本机portal
    server.listen(5)
    while True:
        json_string, client, addr = socketAccept(server)
        list = json.loads(json_string)
        projectName = list[0]
        sleepTime = int(list[1])
        print(list)
        thread = threading.Thread(target=simulation(client, sleepTime,projectName))
        thread.start()


if __name__ == '__main__':
    service()