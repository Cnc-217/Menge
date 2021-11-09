#coding:utf-8
import socket
import json
import threading
from modelService.model import model


def service(client):
    while True:
        try:
            rawData = client.recv(1024).decode("utf8")
        except ConnectionResetError:
            print("The connection is dead")
            break
        # 接收数据，解析json
        print("\nhave recive data")
        jsonData = json.loads(rawData)
        dataList = jsonData["data"]
        print(dataList)
        # 输入模型，输出probList，返回参数
        problist = model(dataList)
        jsonData = {"data": problist}
        jsonStr = json.dumps(jsonData)
        client.sendall(jsonStr.encode("utf8"))
        print("send ok")


if __name__ == '__main__':
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('127.0.0.1', 5001)) #本机mobile
    server.listen(20)
    while True:
        # 接收client
        print("\nwaiting for simulation")
        client, caddr = server.accept()
        print("new simulation is running")
        thread = threading.Thread(target=service(client))
        thread.start()



