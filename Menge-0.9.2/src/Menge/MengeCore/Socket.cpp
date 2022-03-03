
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
		WSAStartup(MAKEWORD(2, 2), &wsadata);//��һ��������socket�汾���ڶ���������socketͨ������������socket�汾��Ϣ

		//c++��Ϊ�ͻ��ˣ������׽���/��/����
		SOCKET  client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); ;
		//��ʼ���׽�������
		struct sockaddr_in adr_socket;
		adr_socket.sin_family = AF_INET;
		adr_socket.sin_addr.s_addr = inet_addr(ip);
		adr_socket.sin_port = htons(host);
		client = socket(AF_INET, SOCK_STREAM, 0);

		int fail = connect(client, (struct sockaddr*)&adr_socket, sizeof(adr_socket));
		if (fail) {
			cout << "����������ʧ�ܣ������˳�..." << endl;
			exit(1);
		}
		std::cout << "socket client start" << std::endl;

		return client;
	}

	SOCKET Socket::socketServerInit(char* ip, int host) {
		WSADATA wsadata;
		int i =WSAStartup(MAKEWORD(2, 2), &wsadata);//��һ��������socket�汾���ڶ���������socketͨ������������socket�汾��Ϣ

		if (i != 0)
		{
			printf("socket error 1!");
			return 0;
		}
		//c++��Ϊ�ͻ��ˣ������׽���/��/����
		SOCKET  socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketServer == INVALID_SOCKET)
		{
			printf("socket error 2!");
			return 0;
		}
		//��ʼ���׽�������
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
		//�������Σ���һ�������ݳ��ȣ��ڶ���������
		send(socket, len.c_str(), strlen(len.c_str()), 0);
		char receiveBuf[1024] = {};
		recv(socket, receiveBuf, 1024, 0);
		send(socket, str, strlen(str), 0);
	}

	string Socket::socketListen(SOCKET socket) {
		//���տͻ��˴��������ݴ�С
		char receiveBuf[1024] = {};
		recv(socket, receiveBuf, 1024, 0);
		int dataSize = atoi(receiveBuf);
		string receiveSignal = "ok";
		send(socket, receiveSignal.c_str(), strlen(receiveSignal.c_str()), 0);

		//ѭ�����տͻ�������
		int recevied_size = 0;
		string recevied_data = "";
		int lenBuf;
		//�����յ����ݴ�С С�� �ͻ��˷���������
		while (recevied_size < dataSize) {
			memset(receiveBuf, '\0', sizeof(receiveBuf));
			lenBuf = recv(socket, receiveBuf, 1023, 0);
			recevied_size += lenBuf;  //ÿ���յ��ķ���˵������п���С��1024�����Ա�����len�ж�
			receiveBuf[lenBuf] = '\0';
			recevied_data += receiveBuf;
		}
		return recevied_data;
	}


}




