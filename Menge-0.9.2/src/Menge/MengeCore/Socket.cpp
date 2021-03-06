
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
using namespace Menge::BFSM;


namespace Menge {

	SOCKET Socket::socketClientInit(char* ip, int host) {
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 1), &wsadata);//第一个参数，socket版本；第二个参数，socket通过它返回请求socket版本信息

		//c++作为客户端，生成套接字/绑定/监听
		SOCKET  clientPython;
		//初始化套接字配置
		struct sockaddr_in adr_pythons;
		adr_pythons.sin_family = AF_INET;
		adr_pythons.sin_addr.s_addr = inet_addr(ip);
		adr_pythons.sin_port = htons(host);

		std::cout << "socket client start" << std::endl;
		clientPython = socket(AF_INET, SOCK_STREAM, 0);
		connect(clientPython, (struct sockaddr*)&adr_pythons, sizeof(adr_pythons));

		return clientPython;
	}

	SOCKET Socket::socketServerInit(char* ip, int host) {
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 1), &wsadata);//第一个参数，socket版本；第二个参数，socket通过它返回请求socket版本信息

		//c++作为客户端，生成套接字/绑定/监听
		SOCKET  socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//初始化套接字配置
		struct sockaddr_in adr_socket;
		adr_socket.sin_family = AF_INET;
		adr_socket.sin_addr.s_addr = inet_addr(ip);
		adr_socket.sin_port = htons(host);


		if (bind(socketServer, (SOCKADDR*)&adr_socket, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			std::cout << "socket server bind error! ip: " << ip <<" port: "<< host << std::endl;
			exit(0);
		}
		
		std::cout << "socket server start" << std::endl;
		listen(socketServer, 10);

		return socketServer;
	
	}

	int* Socket::socketGetCouponBusiness(char* message) {
		//1.socket初始化、发送数据
		//char* ip = "10.210.77.109";
		//int host = 12349;
		char* ip = "10.28.195.233";
		int host = 12347;
		SOCKET clientPython = socketClientInit(ip,host);

		std::cout << "send msg to python: " << message << std::endl;
		send(clientPython, message, strlen(message) * sizeof(char), 0);//发送信息给python端server

		//2.socket接收数据
		char buffer[100];
		int len;
		cout << "socket listen start and wait for msg" << endl;
		if ((len = recv(clientPython, buffer, 100 * sizeof(char), 0)) < 0)
			perror("recv");
		buffer[len] = '\0';


		//3.处理接收到的buffer，转换为res[7]，存储三个动作
		static int res[7] = { 0 };
		string s2;
		stringstream sstream(buffer);
		stringstream tmp;
		int i = 0;
		while (getline(sstream, s2, ' ')) {
			tmp << s2;
			tmp >> res[i];
			tmp.clear();
			//cout << "res " << i << ":" << res[i] << endl; 
			i++;
		}

		closesocket(clientPython);
		cout << "socket over" << endl;
	
		return res;
	}
		
	int* Socket::socketGetCouponBusinessReality(char* message) {
		//1.socket初始化、发送数据
		//char* ip = "10.128.234.214";
		char* ip = "10.28.195.233";
		int host = 12347;
		SOCKET clientPython = socketClientInit(ip,host);


		std::cout << "send msg to python: " << message << std::endl;
		send(clientPython, message, strlen(message) * sizeof(char), 0);//发送信息给python端server

		//2.socket接收数据
		char buffer[100];
		int len;
		cout << "socket listen start and wait for msg" << endl;
		if ((len = recv(clientPython, buffer, 100 * sizeof(char), 0)) < 0)
			perror("recv");
		buffer[len] = '\0';


		//3.处理接收到的buffer，转换为res[3]，存储三个动作
		static int res[3] = { 0 };
		string s2;
		stringstream sstream(buffer);
		stringstream tmp;
		int i = 0;
		while (getline(sstream, s2, ' ')) {
			tmp << s2;
			tmp >> res[i];
			tmp.clear();
			//cout << "res " << i << ":" << res[i] << endl; 
			i++;
		}

		closesocket(clientPython);
		cout << "socket over" << endl;

		return res;
	}

}




