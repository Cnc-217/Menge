#include<MengeCore/BFSM/init.h>
#include<stdio.h>
#include"Menge/MengeCore/Math/Geometry2D.h"


using namespace std;
using namespace Menge;
using namespace Menge::BFSM;

void Menge::Scene::init_RoadandShop(string roadRegionDir)
{
	pinitRoadAndShop->init(roadRegionDir);
}

void Menge::Scene::init_Socket()
{
	pinitSocket->init();
}

void Menge::Scene::init_ProbMatrix(int goalSetSize)
{
	pinitProbMatrix->init(goalSetSize);
}

void Menge::Scene::init_all(string roadRegionDir, int goalSetSize)
{
	pinitRoadAndShop->init(roadRegionDir);
	pinitSocket->init();
	pinitProbMatrix->init(goalSetSize);
}

void Menge::initRoadAndShop::init(string roadRegionDir)
{
	bool shopInitOk = Menge::FileTool::shopInit(roadRegionDir, shopinfo2);//此处的shopinfo2是此类的内部变量
	bool roadRegionOk = Menge::FileTool::setRoadRegionFromXML(roadRegionDir,roadRegionInfo2);
	if (!shopInitOk || !roadRegionOk)
		cout << " shop : " << shopInitOk << " road : " << roadRegionOk << endl;
	else  cout << "shop and road init OK!" << endl;
}

void Menge::initProbMatrix::init(int goalSetSize)
{
	string dir = Menge::DirectoryPath + "/matrix.txt";
	Menge::BaseScene::loadMatrixFromTxt((char*)dir.data());
	if (BaseScene::ProbMatrix->col_size() != goalSetSize) {
		cout << "matrix not matching goal number" << endl;
		exit(1);
	}
}

void Menge::initSocketDefault::init()
{
	cout << "No Specific Simulation" << endl;
	//3.初始化socket服务端，用于疏散状态转移控制
	SOCKET socketServer = Menge::Socket::socketServerInit("127.0.0.1", 12660);
	thread threadSocket(Menge::BaseScene::sockerServerListen, socketServer);
	threadSocket.detach();
}

void Menge::initSocketMethod::init()
{
	string roadRegionDir = Menge::DirectoryPath + "/roadRegion.xml";
	Menge::FileTool::getIpFromXml(roadRegionDir, method, ip, port);
	switch (method)
	{
	case 0:
	{
		SOCKET socketServer = Menge::Socket::socketServerInit((char*)ip.data(), port);
		thread threadSocket(Menge::BaseScene::sockerServerListen, socketServer);
		threadSocket.detach();
	}; break;
	case 1:
	{	//初始化socket客户端，发出数据同步请求，接收仿真参数，更新仿真参数，最后进入监听状态
		SOCKET socketClient = Menge::Socket::socketClientInit((char*)ip.data(), port);
		Menge::Olympic::parameterInit(socketClient);
		thread threadSocket(Menge::BaseScene::sockerClientListen, socketClient);
		threadSocket.detach();
	}; break;
	}
	cout << "It's OLYMPIC Simulation" << endl;
}

void Menge::initProbMatrixDefault::init(int goalSetSize)
{
	int goalNum = goalSetSize;
	cout << "apply the defult matrix,all ONE" << endl;
	if (BaseScene::ProbMatrix == 0x0) {
		
		MatrixDim2* tmpm = new MatrixDim2(goalNum, goalNum, 1);
		BaseScene::ProbMatrix = tmpm;
	}
	BaseScene::ProbMatrix->InitSumWeight();
}

void Menge::initRoadAndShopDefault::init(string roadRegionDir)
{
	cout << "未初始化道路和商店！" << endl;
}
