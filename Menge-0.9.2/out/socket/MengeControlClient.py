#coding:utf-8
import socket
import json


def main(jsonData):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    serverIp = "localhost" #服务器
    serverPort = 12660
    try:
        client.connect((serverIp, serverPort))
    except socket.error:
        print('fail to setup socket connection')
        return

    print('connection ok')
    messageSend = json.dumps(jsonData)
    client.sendall(messageSend.encode("utf8"))

    data = client.recv(1024).decode("utf8")
    print("recive message:")
    print(data)
    client.close()

if __name__ == '__main__':

    matrix = [[0, 0, 0.05, 0, 0.16,0 ,0.79, 0, 0, 0],
[0, 0, 0.19, 0.37, 0, 0.12, 0.24, 0.07, 0, 0],
[0,0.26,0,0.40,0,0.10,0.12,0.11,0,0],
[0,0.13,0.20,0,0.08,0.30,0.13,0,0,0.16],
[0,0.17,0,0.62,0,0.11,0.11,0,0,0],
[0,0.10,0.08,0.26,0,0,0,0.09,0.12,0.35],
[0,0.21,0.15,0.54,0,0.10,0,0,0,0],
[0,0.25,0.19,0.16,0,0.17,0,0,0.10,0.13],
[0,0,0,0,0,0.51,0,0.20,0,0.29],
[0,0,0,0,0,0,0,0,0,1]
]

    matrixStr = ""
    rowNum = len(matrix)
    columnNUm = len(matrix[0])
    sumNum = 0
    for i in matrix:
        for j in i:
            matrixStr = matrixStr + str(j) + " "
            sumNum = sumNum + 1

    # print("rowNum: "+str(rowNum))
    # print("columnNUm: "+str(columnNUm))
    # print("sumNum: "+str(sumNum))
    if(sumNum!=rowNum^2 | rowNum!=columnNUm):
        print("error!!!")

    print(matrixStr)

    jsonData = { "command" : "Evacuate", "data":"" }
    #jsonData = { "command" : "Matrix", "data":matrixStr }
    main(jsonData)