
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
		WSAStartup(MAKEWORD(2, 2), &wsadata);//第一个参数，socket版本；第二个参数，socket通过它返回请求socket版本信息

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


	int Socket::getIpinfo()
	{
		char host_name[255];
		//获取本地主机名称 
		if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) {
			printf("Error %d when getting local host name.\n", WSAGetLastError());
			return 1;
		}
		printf("Host name is: %s\n", host_name);

		//从主机名数据库中得到对应的“主机” 
		struct hostent* phe = gethostbyname(host_name);
		if (phe == 0) {
			printf("Yow! Bad host lookup.");
			return 1;
		}

		//循环得出本地机器所有IP地址 
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
		//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
		PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
		//得到结构体大小,用于GetAdaptersInfo参数
		unsigned long stSize = sizeof(IP_ADAPTER_INFO);
		//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
		int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		//记录网卡数量
		int netCardNum = 0;
		//记录每张网卡上的IP地址数量
		int IPnumPerNetCard = 0;
		if (ERROR_BUFFER_OVERFLOW == nRel)
		{
			//如果函数返回的是ERROR_BUFFER_OVERFLOW
			//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
			//这也是说明为什么stSize既是一个输入量也是一个输出量
			//释放原来的内存空间
			delete pIpAdapterInfo;
			//重新申请内存空间用来存储所有网卡信息
			pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
			//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
			nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		}
		if (ERROR_SUCCESS == nRel)
		{
			//输出网卡信息
			 //可能有多网卡,因此通过循环去判断
			while (pIpAdapterInfo)
			{
				cout << "网卡数量：" << ++netCardNum << endl;
				cout << "网卡名称：" << pIpAdapterInfo->AdapterName << endl;
				cout << "网卡描述：" << pIpAdapterInfo->Description << endl;
				switch (pIpAdapterInfo->Type)
				{
				case MIB_IF_TYPE_OTHER:
					cout << "网卡类型：" << "OTHER" << endl;
					break;
				case MIB_IF_TYPE_ETHERNET:
					cout << "网卡类型：" << "ETHERNET" << endl;
					break;
				case MIB_IF_TYPE_TOKENRING:
					cout << "网卡类型：" << "TOKENRING" << endl;
					break;
				case MIB_IF_TYPE_FDDI:
					cout << "网卡类型：" << "FDDI" << endl;
					break;
				case MIB_IF_TYPE_PPP:
					printf("PP\n");
					cout << "网卡类型：" << "PPP" << endl;
					break;
				case MIB_IF_TYPE_LOOPBACK:
					cout << "网卡类型：" << "LOOPBACK" << endl;
					break;
				case MIB_IF_TYPE_SLIP:
					cout << "网卡类型：" << "SLIP" << endl;
					break;
				default:

					break;
				}
				cout << "网卡MAC地址：";
				for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
					if (i < pIpAdapterInfo->AddressLength - 1)
					{
						printf("%02X-", pIpAdapterInfo->Address[i]);
					}
					else
					{
						printf("%02X\n", pIpAdapterInfo->Address[i]);
					}
				cout << "网卡IP地址如下：" << endl;
				//可能网卡有多IP,因此通过循环去判断
				IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
				do
				{
					cout << "该网卡上的IP数量：" << ++IPnumPerNetCard << endl;
					cout << "IP 地址：" << pIpAddrString->IpAddress.String << endl;
					cout << "子网地址：" << pIpAddrString->IpMask.String << endl;
					cout << "网关地址：" << pIpAdapterInfo->GatewayList.IpAddress.String << endl;
					pIpAddrString = pIpAddrString->Next;
				} while (pIpAddrString);
				pIpAdapterInfo = pIpAdapterInfo->Next;
				cout << "--------------------------------------------------------------------" << endl;
			}

		}
		//释放内存空间
		if (pIpAdapterInfo)
		{
			delete pIpAdapterInfo;
		}

		return 0;
	}

}




