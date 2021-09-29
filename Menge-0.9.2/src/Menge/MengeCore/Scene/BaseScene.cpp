
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


using namespace std;
using namespace Menge::BFSM;
using namespace nlohmann;

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
			serConn = accept(socketServer, (SOCKADDR*)&clientsocket, &len);

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
			MengeVis::SimViewer->_pause = true;
			//如果客户端传来了Evacuate，则进入状态转移的函数
			if (!strcmp(command.c_str(), "Evacuate")) {
				cout << "Evacuate mode start" << endl;
				//下面是根据不同项目的定制化代码部分
				if (Menge::PROJECTNAME == THEMEPARK) ThemePark::evacuateModeStart(serConn, j);
				else if (Menge::PROJECTNAME == OLYMPIC) Olympic::evacuateModeStart(serConn, j);
				MengeVis::SimViewer->_pause = false;//false是正常运行
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
			MengeVis::SimViewer->_pause = false;

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
<<<<<<< HEAD
=======
		void Shopinit() {
			for (int i = 0; i < 36; i++)
			{
				if (Menge::Olympic::shoptype.find(i) != Menge::Olympic::shoptype.end()) {
					Menge::Olympic::shoptype[i].type = i % 2;
					Menge::Olympic::shoptype[i].maximum = 3;

				}
				else {
					Menge::Olympic::Shoptype shoptemp;
					shoptemp.type = i % 2;
					shoptemp.maximum = 3;
					Menge::Olympic::shoptype.insert(std::map< int, Menge::Olympic::Shoptype>::value_type(i, shoptemp));
				}
			}
		}
	}
	
}
>>>>>>> c4c79ab4b93521008725e96b31d4fc3a6baf00e3

		void sendMatrixFlowScene(SOCKET serConn, json j) {
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 1);
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
			std::vector<int> agentNumOfShop(36, 1);
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


