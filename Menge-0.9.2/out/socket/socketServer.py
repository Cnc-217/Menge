import socket

def socketAccept(s):
    print ("\nwait for c++")
    c, caddr = s.accept()
    data = c.recv(120).decode()
    print ("get msg from c++: ")
    print (data)
    data_result = []
    data_list = data.split( )
    # print (data_list)
    for i in data_list:
        if (i != ' ' and i != ''):
            data_result.append(int(i))
    return data_result,c, caddr

def socketSend(c,action):
    res = ""
    for i in action:
        if isinstance(i, list):
            for j in i:
                res = res + str(j)
                res = res + ' '
        else:
            res = res + str(i)
            res = res + ' '

    print ("send msg to c++: " )
    print(res)
    c.send(res.encode())
    c.close()

def main( project):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    portSend = 12347
    s.bind(('10.28.195.233', portSend))
    s.listen(5)

    if (project == "BusinessLearning"):
        while True:
            data_result, c, caddr = socketAccept(s)
            print ("after transfer:")
            print (data_result)

            # action = [[0, 0, 1], [0, 4, 5], 3]
            action = [[1, 0, 0], [8, 0, 0], 0]
            socketSend(c, action)


    elif (project == "BusinessReality"):
        while True:
            data_result, c, addr = socketAccept(s)
            print ("after transfer:")
            print (data_result)
            result = [data_result,0,1] #observation, reward, done
            print (result)

            action = [0, 1, 0]
            socketSend(c,action)


if __name__ == '__main__':

    project = "BusinessLearning"
    #project = "BusinessReality"
    main(project)
