
#include "MengeCore/Scene/BaseScene.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/Events/EventSystem.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#include "MengeVis/Viewer/GLViewer.h"
#include <string>
#include <map>


using namespace std;
using namespace Menge::BFSM;
using namespace nlohmann;
using namespace Menge::Olympic;

namespace Menge {
	/////////////////////////////////////////////////////////////////////
	//					Implementation of BaseScene
	/////////////////////////////////////////////////////////////////////
	void BaseScene::sockerServerListen(SOCKET socketServer) {
		SOCKADDR_IN clientsocket;
		int len = sizeof(SOCKADDR);
		SOCKET serConn;
		while (true) {
			//等待客户端的连接
			serConn = accept(socketServer, (SOCKADDR*)& clientsocket, &len);

			//接收客户端传来的数据大小
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			int dataSize = atoi(receiveBuf);
			cout << "datasize: " << receiveBuf << endl;
			string receiveSignal = "ok";
			send(serConn, receiveSignal.c_str(), strlen(receiveSignal.c_str()), 0);

			//循环接收客户端数据
			int recevied_size = 0;
			string recevied_data = "";
			int lenBuf;
			//当接收的数据大小 小于 客户端发来的数据
			while (recevied_size < dataSize) {
				memset(receiveBuf, '\0', sizeof(receiveBuf));
				lenBuf = recv(serConn, receiveBuf, 1023, 0);
				cout << "dataPartSize: " << lenBuf << endl;
				recevied_size += lenBuf;  //每次收到的服务端的数据有可能小于1024，所以必须用len判断
				receiveBuf[lenBuf] = '\0';
				recevied_data += receiveBuf;
				cout << "recevie_buf: " << receiveBuf << endl;
			}

			//json解析
			json j = json::parse(recevied_data);
			string command = j["command"];
			cout << "socketServer receive command: " << j["command"] << endl;
			string matrix = j["data"];
			cout << "socketServer receive data: " << j["data"] << endl;

			//返回的json
			j.clear();

			//暂停，处理matrix数据，false是正常运行
			//注意：Unity中的MengeVis DLL无法正常导入，这条指令会导致崩溃，意味着Unity中无法对仿真进行暂停
			//MengeVis::SimViewer->_pause = true;
			//如果客户端传来了Evacuate，则进入状态转移的函数
			if (!strcmp(command.c_str(), "Evacuate")) {
				cout << "Evacuate mode start" << endl;
				//下面是根据不同项目的定制化代码部分
				if (Menge::PROJECTNAME == THEMEPARK) ThemePark::evacuateModeStart(serConn, j);
				else if (Menge::PROJECTNAME == OLYMPIC) Olympic::evacuateModeStart(serConn, j);
				//MengeVis::SimViewer->_pause = false;//false是正常运行
			}
			//如果客户端传来了修改概率矩阵的数据，则修改概率矩阵
			else if (!strcmp(command.c_str(), "MatrixModify")) {
				cout << "Modifying probability matrix " << endl;
				//修改概率矩阵
				modifyMatrix((char*)matrix.c_str(), serConn, j);
			}
			//如果客户端传来了FlowScene的请求，则发送人流数据和矩阵
			else if (!strcmp(command.c_str(), "FlowScene")) {
				cout << "FlowScene" << endl;
				Olympic::sendMatrixFlowScene(serConn, j);
			}
			else if (!strcmp(command.c_str(), "BusinessScene")) {
				cout << "BusinessScene" << endl;
				Olympic::sendMatrixBusinessScene(serConn, j);
			}
			//恢复运行
			//MengeVis::SimViewer->_pause = false;

		}
	}

	void BaseScene::loadMatrixFromTxt(const char* fileName) {
		cout << "loading Matrix from txt" << endl;
		int rowNum = Matrix::getFileRows(fileName);
		if (rowNum == 0) {
			cout << "error, rowNum=0 " << endl;
			exit(1);
		}
		int columnsNum = Matrix::getFileColumns(fileName);
		if (columnsNum == 0) {
			cout << "error, columnsNum=0 " << endl;
			exit(1);
		}
		if (rowNum != columnsNum) {
			cout << "error, rowNum!=columnsNum " << endl;
			exit(1);
		}
		Matrix::getFileData(fileName, rowNum, columnsNum);
		Menge::BaseScene::ProbMatrix->Show();
	}

	void BaseScene::modifyMatrix(char* matrixStr, SOCKET serConn, json j) {
		char* temp = strtok(matrixStr, " ");
		vector<float> vec;
		int sumNum = 0;
		int goalNum = ACTIVE_FSM->getGoalSet(0)->size();
		while (temp != NULL)
		{
			vec.push_back(atof(temp));
			temp = strtok(NULL, " ");
			sumNum = sumNum + 1;
		}
		if (sumNum != goalNum * goalNum) {
			cout << sumNum << " " << goalNum << endl;
			cout << "modify matrix fail, matrix not match goalNum" << endl;
			return;
		}
		else {
			for (int i = 0; i < goalNum; i++) {
				for (int j = 0; j < goalNum; j++) {
					ProbMatrix->SetPoint(i, j, vec[i * goalNum + j]);
				}
			}
			cout << "modify matrix complete" << endl;
			ProbMatrix->Show();
		}

		//回复客户端
		j["Info"] = "Menge has receive your commend: Modify matrix";
		string sendBuf = j.dump();
		string len = to_string(strlen(sendBuf.c_str()));
		//发送两次，第一次是数据长度，第二次是数据
		send(serConn, len.c_str(), strlen(len.c_str()), 0);
		char receiveBuf[1024] = {};
		recv(serConn, receiveBuf, 1024, 0);
		send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
	}

	void BaseScene::sendMatrix(SOCKET serConn, json j) {

		char sendBuf[100] = " ";
		//send(serConn, sendBuf, strlen(sendBuf) + 1, 0);

	}

	void BaseScene::projectNameExtract(string folderPath) {
		std::string tmp = folderPath;
		size_t ps = tmp.find_last_of("\\");
		size_t pe = tmp.length();
		std::string name = tmp.substr(ps + 1, pe - ps + 1);

		if (strcmp(name.c_str(), "Business") == 0) {
			PROJECTNAME = BUSINESS;
		}
		else if (strcmp(name.c_str(), "BusinessLearning") == 0) {
			PROJECTNAME = BUSINESSLEARNING;
		}
		else if (strcmp(name.c_str(), "BusinessReality") == 0) {
			PROJECTNAME = BUSINESSREALITY;
		}
		else if (name.find("Evacuation") != name.npos) {
			PROJECTNAME = EVACUATION;
		}
		else if (name.find("ThemePark") != name.npos) {
			PROJECTNAME = THEMEPARK;
		}
		else if (name.find("Olympic") != name.npos) {
			PROJECTNAME = OLYMPIC;
		}
	}

	namespace ThemePark {

		void evacuateModeStart(SOCKET serConn, json j) {
			//0.预先定义好引导者的agentgoalset，但恐慌者的goalset无法提前定义好，解决方案是把所有人都变成goal，id匹配，然后为goalset代码添加agentgoal,目前只能手动添加
			//1.分配agent身份 8:2 普通：恐慌 ，存入数组
			//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
			//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
			//4.agent转移到新state
			//5.编写event neighborhoodtrigger中编写themepark的agent跟随逻辑，如果附近有特定的agent，就重新进入evacuation状态，选择目标

			//1.分配agent身份，存入数组
			int numAgent = SIMULATOR->getNumAgents();
			float normalPercentage = 0.8;
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				//游客
				if (agent->_class == 0) {
					//随机数生成
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float random = ((rand() % 100) * 0.01);
					//normalAgent的比例
					if (random <= normalPercentage) {
						agent->_class = 3;//normal
						normalAgentSet.push_back(agent);
					}
					else {
						agent->_class = 2;//panic
						panicAgentSet.push_back(agent);
					}
				}
				//leader
				else if (agent->_class == 1) {
					leaderAgentSet.push_back(agent);
				}
			}


			//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
			//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
			//4.agent转移到新state
			for (int i = 0; i < numAgent; i++) {
				cout << i << endl;
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);
				State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}

			//5.编写event,event需要启动状态码
			evacuationState = true;

			//回复客户端
			j["Info"] = "Menge has receive your commend: ThemePark evacuation start";
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//发送两次，第一次是数据长度，第二次是数据
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
		}

	}

	namespace Olympic {
		void evacuateModeStart(SOCKET serConn, json j) {
			//0.预先定义好引导者的agentgoalset，但恐慌者的goalset无法提前定义好，解决方案是把所有人都变成goal，id匹配，然后为goalset代码添加agentgoal,目前只能手动添加
			//1.分配agent身份 8:2 普通：恐慌 ，存入数组
			//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
			//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
			//4.agent转移到新state
			//5.编写event neighborhoodtrigger中编写themepark的agent跟随逻辑，如果附近有特定的agent，就重新进入evacuation状态，选择目标

			//1.分配agent身份，存入数组
			int numAgent = SIMULATOR->getNumAgents();
			float normalPercentage = 0.8;
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				//游客
				if (agent->_class == 0) {
					//随机数生成
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float random = ((rand() % 100) * 0.01);
					//normalAgent的比例
					if (random <= normalPercentage) {
						agent->_class = 3;//normal
						normalAgentSet.push_back(agent);
					}
					else {
						agent->_class = 2;//panic
						panicAgentSet.push_back(agent);
					}
				}
				//leader
				else if (agent->_class == 1) {
					leaderAgentSet.push_back(agent);
				}
			}


			//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
			//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
			//4.agent转移到新state
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);
				State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.编写event,event需要启动状态码
			evacuationState = true;

			//回复客户端
			j["Info"] = "Menge has receive your commend: Olympic evacuation start";
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//发送两次，第一次是数据长度，第二次是数据
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);

		}

		bool shopInit(string dir) {
			int data[10][4] = { 0 };
			ifstream infile;//定义读取文件流，相对于程序来说是in
			infile.open(dir);//打开文件
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			for (int i = 0; i < 10; i++)//定义行循环
				for (int j = 0; j < 4; j++)//定义列循环
					infile >> data[i][j];//读取一个值（空格、制表符、换行隔开）就写入到矩阵中，行列不断循环进行
			infile.close();//读取完成之后关闭文件
			int index = 0;
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < data[i][1]; j++)
				{
					Shoptype shoptemp;
					shoptemp.type = data[i][0];
					shoptemp.serviceMax = data[i][2];
					shoptemp.blockMax = data[i][3];
					shoptemp.goalSet = i;
					shopInfo.insert(make_pair(index, shoptemp));//插入
					index++;
				}
			}
			for (int i = 0; i < 10; i++)
				goalSetInfo.insert(std::map< size_t, int >::value_type(i, data[i][1]));
			//for (int i = 0; i < 10; i++)
				//cout<<goalSetInfo[i]<<endl;
			return true;
			//for (int i = 0; i < 36; i++)//测试用
			//	cout << shopInfo[index].blockMax << "+" << shopInfo[i].serviceMax << "+" << shopInfo[i].type <<"+"<< endl;
			}


		void sendMatrixFlowScene(SOCKET serConn, json j) {
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();

			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json生成
			j["Info"] = "Menge has receive your commend: FlowScene";
			j["FlowData"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//发送两次，第一次是数据长度，第二次是数据
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);

		}

		void sendMatrixBusinessScene(SOCKET serConn, json j) {
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json生成
			j["Info"] = "Menge has receive your commend: BusinessScene";
			j["BusinessScene"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//发送两次，第一次是数据长度，第二次是数据
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
		}

	}

}

