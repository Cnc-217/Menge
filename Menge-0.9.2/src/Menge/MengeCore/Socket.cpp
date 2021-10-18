
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
		WSAStartup(MAKEWORD(2, 2), &wsadata);//��һ��������socket�汾���ڶ���������socketͨ������������socket�汾��Ϣ

		//c++��Ϊ�ͻ��ˣ������׽���/��/����
		SOCKET  clientPython;
		//��ʼ���׽�������
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
		//adr_socket.sin_port = htons(8888);
		//adr_socket.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(socketServer, (SOCKADDR*)&adr_socket, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			std::cout << "socket server bind error! ip: " << ip <<" port: "<< host << std::endl;
			exit(1);
		}
		
		std::cout << "socket server start" << std::endl;
		listen(socketServer, 10);

		return socketServer;
	
	}

	int* Socket::socketGetCouponBusiness(char* message) {
		//1.socket��ʼ������������
		//char* ip = "10.210.77.109";
		//int host = 12349;
		char* ip = "10.28.195.233";
		int host = 12347;
		SOCKET clientPython = socketClientInit(ip,host);

		std::cout << "send msg to python: " << message << std::endl;
		send(clientPython, message, strlen(message) * sizeof(char), 0);//������Ϣ��python��server

		//2.socket��������
		char buffer[100];
		int len;
		cout << "socket listen start and wait for msg" << endl;
		if ((len = recv(clientPython, buffer, 100 * sizeof(char), 0)) < 0)
			perror("recv");
		buffer[len] = '\0';


		//3.������յ���buffer��ת��Ϊres[7]���洢��������
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
		//1.socket��ʼ������������
		//char* ip = "10.128.234.214";
		char* ip = "10.28.195.233";
		int host = 12347;
		SOCKET clientPython = socketClientInit(ip,host);


		std::cout << "send msg to python: " << message << std::endl;
		send(clientPython, message, strlen(message) * sizeof(char), 0);//������Ϣ��python��server

		//2.socket��������
		char buffer[100];
		int len;
		cout << "socket listen start and wait for msg" << endl;
		if ((len = recv(clientPython, buffer, 100 * sizeof(char), 0)) < 0)
			perror("recv");
		buffer[len] = '\0';


		//3.������յ���buffer��ת��Ϊres[3]���洢��������
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


	int Socket::getIpinfo()
	{
		char host_name[255];
		//��ȡ������������ 
		if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) {
			printf("Error %d when getting local host name.\n", WSAGetLastError());
			return 1;
		}
		printf("Host name is: %s\n", host_name);

		//�����������ݿ��еõ���Ӧ�ġ������� 
		struct hostent* phe = gethostbyname(host_name);
		if (phe == 0) {
			printf("Yow! Bad host lookup.");
			return 1;
		}

		//ѭ���ó����ػ�������IP��ַ 
		for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
			printf("Address %d : %s\n", i, inet_ntoa(addr));
		}

		return 0;
	}

	int Socket::getIp()
	{

		WSAData wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
			return 255;
		}

		int retval = getIpinfo();

		WSACleanup();

		return retval;
	}

	int Socket::getIp2()
	{
		//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
		PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
		//�õ��ṹ���С,����GetAdaptersInfo����
		unsigned long stSize = sizeof(IP_ADAPTER_INFO);
		//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
		int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		//��¼��������
		int netCardNum = 0;
		//��¼ÿ�������ϵ�IP��ַ����
		int IPnumPerNetCard = 0;
		if (ERROR_BUFFER_OVERFLOW == nRel)
		{
			//����������ص���ERROR_BUFFER_OVERFLOW
			//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
			//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
			//�ͷ�ԭ�����ڴ�ռ�
			delete pIpAdapterInfo;
			//���������ڴ�ռ������洢����������Ϣ
			pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
			//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
			nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		}
		if (ERROR_SUCCESS == nRel)
		{
			//���������Ϣ
			 //�����ж�����,���ͨ��ѭ��ȥ�ж�
			while (pIpAdapterInfo)
			{
				cout << "����������" << ++netCardNum << endl;
				cout << "�������ƣ�" << pIpAdapterInfo->AdapterName << endl;
				cout << "����������" << pIpAdapterInfo->Description << endl;
				switch (pIpAdapterInfo->Type)
				{
				case MIB_IF_TYPE_OTHER:
					cout << "�������ͣ�" << "OTHER" << endl;
					break;
				case MIB_IF_TYPE_ETHERNET:
					cout << "�������ͣ�" << "ETHERNET" << endl;
					break;
				case MIB_IF_TYPE_TOKENRING:
					cout << "�������ͣ�" << "TOKENRING" << endl;
					break;
				case MIB_IF_TYPE_FDDI:
					cout << "�������ͣ�" << "FDDI" << endl;
					break;
				case MIB_IF_TYPE_PPP:
					printf("PP\n");
					cout << "�������ͣ�" << "PPP" << endl;
					break;
				case MIB_IF_TYPE_LOOPBACK:
					cout << "�������ͣ�" << "LOOPBACK" << endl;
					break;
				case MIB_IF_TYPE_SLIP:
					cout << "�������ͣ�" << "SLIP" << endl;
					break;
				default:

					break;
				}
				cout << "����MAC��ַ��";
				for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
					if (i < pIpAdapterInfo->AddressLength - 1)
					{
						printf("%02X-", pIpAdapterInfo->Address[i]);
					}
					else
					{
						printf("%02X\n", pIpAdapterInfo->Address[i]);
					}
				cout << "����IP��ַ���£�" << endl;
				//���������ж�IP,���ͨ��ѭ��ȥ�ж�
				IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
				do
				{
					cout << "�������ϵ�IP������" << ++IPnumPerNetCard << endl;
					cout << "IP ��ַ��" << pIpAddrString->IpAddress.String << endl;
					cout << "������ַ��" << pIpAddrString->IpMask.String << endl;
					cout << "���ص�ַ��" << pIpAdapterInfo->GatewayList.IpAddress.String << endl;
					pIpAddrString = pIpAddrString->Next;
				} while (pIpAddrString);
				pIpAdapterInfo = pIpAdapterInfo->Next;
				cout << "--------------------------------------------------------------------" << endl;
			}

		}
		//�ͷ��ڴ�ռ�
		if (pIpAdapterInfo)
		{
			delete pIpAdapterInfo;
		}

		return 0;
	}

}




