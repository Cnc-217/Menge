from scbData import ScbProcess
from XMLGenerate import XML
import sys
import os
import socket


def main():
    #M0,M1,M2,Sh0,Sh1,Sh2,S = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), \
    #                         int(sys.argv[5]), int(sys.argv[6]), int(sys.argv[7])
    M0,M1,M2,Sh0,Sh1,Sh2,S = 1,0,1,5,0,5,1
    XML(M0,M1,M2,Sh0,Sh1,Sh2,S)
    print ("XMLgenerate over")
    os.system('D:\File\Project\Menge-master\Exe\menge.exe -p D:\File\Project\Menge-master\examples\Town_v2\Town_v2.xml -o D:\File\Project\Menge-master\examples\Town_v2\out.scb -d 200')
    print ("simulation over")
    result = ScbProcess()
    print result
    #return result
    print ("Unity start")
    os.system('D:\File\Project\Menge-master\examples\Town_v2\/town_all\/"Vehicle Project.exe"')


if __name__ == '__main__':

    tcp_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    tcp_server_socket.bind(("localhost", 8081))

    tcp_server_socket.listen(128)
    print("wait client!")

    client_socket, client_Addr = tcp_server_socket.accept()
    goon = True
    print("1")
    while goon:
        recv_data = client_socket.recv(1024)
        if not recv_data:
            break
        else:
            main()
            goon = False
            print("C:request is over")
    print("2")
    tcp_server_socket.close()
