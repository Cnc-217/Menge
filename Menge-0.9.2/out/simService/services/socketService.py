#coding:utf-8
import json

# 给Client发送数据
def socketSend(jsonData, client):
    # 发送数据，发送第一次：数据大小 第二次：数据
    messageSend = json.dumps(jsonData)
    client.sendall(str(len(messageSend.encode("utf8"))).encode("utf8"))  # 发送服务端发送给客户端数据的长度
    client.recv(1024).decode("utf8")  # 接收确认信息
    client.sendall(messageSend.encode("utf8"))

#接收到的是原始信息，函数外需要json化处理
def socketRecive(client):
    # 接收数据：先接收即将发送过来的数据大小,然后循环接收大数据
    data_size = client.recv(1024).decode("utf8")
    receiveSignal = "ok"
    client.sendall(receiveSignal.encode("utf8"))
    recevied_size = 0  # 接收客户端发来数据的计算器
    recevied_data = ''  # 客户端每次发来内容的计数器
    while recevied_size < int(data_size):  # 当接收的数据大小 小于 服务端发来的数据
        dataRecived = client.recv(1024).decode("utf8")
        recevied_size += len(dataRecived)  # 每次收到的服务端的数据有可能小于1024，所以必须用len判断
        recevied_data += dataRecived

    return recevied_data
