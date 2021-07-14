#coding:utf-8
import socket
import json


def main(project,duration):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serverIp = "10.28.195.233"
    serverPort = 12348
    try:
        client.connect((serverIp, serverPort))
    except socket.error:
        print('fail to setup socket connection')

    messageSend = [project,str(duration)]
    data = json.dumps(messageSend)
    client.sendall(bytes(data.encode("utf8")))
    if(duration==0):
        stopSignal = ""
        while(stopSignal!="stop"):
            stopSignal = input("input \"stop\" when you want to stop menge: ")
        client.sendall(stopSignal.encode("utf8"))
    data = client.recv(1024).decode("utf8")
    print("recive message:")
    print(data)


if __name__ == '__main__':
    duration = 0
    project = "BusinessLearning"
    #project = "BusinessReality"
    main(project,duration)