#coding:utf-8
import socket


class SocketServer:
    #初始化
    def __init__(self, serverIp, serverPort):
        self.server:socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((serverIp, serverPort))
        self.server.listen(5)
        print("socket server start")


    #监听MengeClient
    def listen(self):
        print("\nwait for pythonClient")
        client, caddr = self.server.accept()
        data = client.recv(1024).decode("utf8")
        return data, client, caddr

    #关闭socketClient
    def close(self):
        self.server.close()

    def getSocket(self):
        return self.server


#socket服务器，用于和Menge通信
socketServer = SocketServer("10.28.195.233", 12660)