
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

#include "MengeCore/Json/json.hpp"


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
		
        while (1) {
            //等待客户端的连接
            serConn = accept(socketServer, (SOCKADDR*)&clientsocket, &len);
            char receiveBuf[1024];
            //接收客户端传来的信息
            int lenBuf;
            lenBuf = recv(serConn, receiveBuf, 1024, 0);
            receiveBuf[lenBuf] = '\0';

			//json解析
			json j = json::parse(receiveBuf);
			string command = j["command"];
			cout <<"socketServer receive command: "<< j["command"] << endl;
			string matrix = j["data"];
			cout <<"socketServer receive data: "<< j["data"] << endl;
			
			//处理matrix数据
			
            //如果客户端传来了Evacuate，则进入状态转移的函数
            if (!strcmp(command.c_str(), "Evacuate")) {
                cout << "Evacuate mode start" << endl;
                MengeVis::SimViewer->_pause = true;//暂停
                //暂停，下面是根据不同项目的定制化代码部分
                if(Menge::PROJECTNAME==THEMEPARK) ThemePark::evacuateModeStart();
                else if(Menge::PROJECTNAME==OLYMPIC) Olympic::evacuateModeStart();
                MengeVis::SimViewer->_pause = false;//false是正常运行
            }
			//如果客户端传来了修改概率矩阵的数据，则修改概率矩阵
			else if (!strcmp(command.c_str(), "Matrix")) {
				cout << "Modifying probability matrix " << endl;
				MengeVis::SimViewer->_pause = true;//false是正常运行
				//暂停，下面是修改概率矩阵
				modifyMatrix((char*)matrix.c_str());
				MengeVis::SimViewer->_pause = false;
			}

			//回复客户端
            char sendBuf[100] = "Menge has receive your commend";
            send(serConn, sendBuf, strlen(sendBuf) + 1, 0);

        }
	}

    void BaseScene::loadMatrixFromTxt(const char* fileName) {
		cout << "loading Matrix from txt" << endl;
        int rowNum = Matrix::getFileRows(fileName);
		if (rowNum == 0) {
			cout << "error, rowNum=0 "  << endl;
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

	void BaseScene::modifyMatrix(char* matrixStr) {
		char* temp = strtok(matrixStr," ");
		vector<float> vec;
		int sumNum = 0;
		int goalNum = ACTIVE_FSM->getGoalSet(0)->size();
		while (temp != NULL)
		{
			vec.push_back(atof(temp));
			temp = strtok(NULL, " ");
			sumNum = sumNum + 1;
		}
		if (sumNum != goalNum* goalNum) {
			cout << sumNum << " "<< goalNum << endl;
			cout << "modify matrix fail, matrix not match goalNum"<<endl;
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
	}


	namespace ThemePark {

		void evacuateModeStart() {
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


		}

	}

	namespace Olympic {
		void evacuateModeStart() {
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


		}
	}
	
}




