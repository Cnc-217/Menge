
#include "MengeCore/Socket.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/Events/EventSystem.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"

using namespace std;

namespace Menge {

	SOCKET Socket::socketClientInit(char* ip, int host) {
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);//第一个参数，socket版本；第二个参数，socket通过它返回请求socket版本信息

		//c++作为客户端，生成套接字/绑定/监听
		SOCKET  client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); ;
		//初始化套接字配置
		struct sockaddr_in adr_socket;
		adr_socket.sin_family = AF_INET;
		adr_socket.sin_addr.s_addr = inet_addr(ip);
		adr_socket.sin_port = htons(host);
		client = socket(AF_INET, SOCK_STREAM, 0);

		int fail = connect(client, (struct sockaddr*)&adr_socket, sizeof(adr_socket));
		if (fail) {
			cout << "与服务端连接失败！程序将退出..." << endl;
			exit(1);
		}
		std::cout << "socket client start" << std::endl;

		return client;
	}

	SOCKET Socket::socketServerInit(char* ip, int host) {
		WSADATA wsadata;
		int i =WSAStartup(MAKEWORD(2, 2), &wsadata);//第一个参数，socket版本；第二个参数，socket通过它返回请求socket版本信息

		if (i != 0)
		{
			printf("socket error 1!");
			return 0;
		}
		//c++作为客户端，生成套接字/绑定/监听
		SOCKET  socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketServer == INVALID_SOCKET)
		{
			printf("socket error 2!");
			return 0;
		}
		//初始化套接字配置
		struct sockaddr_in adr_socket;
		adr_socket.sin_family = AF_INET;
		adr_socket.sin_addr.s_addr = inet_addr(ip);
		adr_socket.sin_port = htons(host);
		if (::bind(socketServer, (SOCKADDR*)&adr_socket, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			std::cout << "socket server bind error! ip: " << ip <<" port: "<< host << std::endl;
			exit(1);
		}
		
		std::cout << "socket server start" << std::endl;
		listen(socketServer, 10);

		return socketServer;
	
	}

	void Socket::socketSend(const char* str, SOCKET socket) {
		string len = to_string(strlen(str));
		//发送两次，第一次是数据长度，第二次是数据
		send(socket, len.c_str(), strlen(len.c_str()), 0);
		char receiveBuf[1024] = {};
		recv(socket, receiveBuf, 1024, 0);
		send(socket, str, strlen(str), 0);
	}

	string Socket::socketListen(SOCKET socket) {
		//接收客户端传来的数据大小
		char receiveBuf[1024] = {};
		recv(socket, receiveBuf, 1024, 0);
		int dataSize = atoi(receiveBuf);
		string receiveSignal = "ok";
		send(socket, receiveSignal.c_str(), strlen(receiveSignal.c_str()), 0);

		//循环接收客户端数据
		int recevied_size = 0;
		string recevied_data = "";
		int lenBuf;
		//当接收的数据大小 小于 客户端发来的数据
		while (recevied_size < dataSize) {
			memset(receiveBuf, '\0', sizeof(receiveBuf));
			lenBuf = recv(socket, receiveBuf, 1023, 0);
			recevied_size += lenBuf;  //每次收到的服务端的数据有可能小于1024，所以必须用len判断
			receiveBuf[lenBuf] = '\0';
			recevied_data += receiveBuf;
		}
		return recevied_data;
	}


}




