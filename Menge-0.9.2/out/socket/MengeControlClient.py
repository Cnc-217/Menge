#coding:utf-8
import socket
import json

def main(command):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    serverIp = "localhost" #服务器
    serverPort = 12660
    try:
        client.connect((serverIp, serverPort))
    except socket.error:
        print('fail to setup socket connection')
        return

    print('connection ok')
    messageSend = command
    #data = json.dumps(messageSend)
    client.sendall(messageSend.encode("utf8"))

    data = client.recv(1024).decode("utf8")
    print("recive message:")
    print(data)


if __name__ == '__main__':
    command = "Evacuate"
    main(command)