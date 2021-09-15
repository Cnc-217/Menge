
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
            //�ȴ��ͻ��˵�����
            serConn = accept(socketServer, (SOCKADDR*)&clientsocket, &len);
            char receiveBuf[1024];
            //���տͻ��˴�������Ϣ
            int lenBuf;
            lenBuf = recv(serConn, receiveBuf, 1024, 0);
            receiveBuf[lenBuf] = '\0';

			//json����
			json j = json::parse(receiveBuf);
			string command = j["command"];
			cout <<"socketServer receive command: "<< j["command"] << endl;
			string matrix = j["data"];
			cout <<"socketServer receive data: "<< j["data"] << endl;
			
			//����matrix����
			
            //����ͻ��˴�����Evacuate�������״̬ת�Ƶĺ���
            if (!strcmp(command.c_str(), "Evacuate")) {
                cout << "Evacuate mode start" << endl;
                MengeVis::SimViewer->_pause = true;//��ͣ
                //��ͣ�������Ǹ��ݲ�ͬ��Ŀ�Ķ��ƻ����벿��
                if(Menge::PROJECTNAME==THEMEPARK) ThemePark::evacuateModeStart();
                else if(Menge::PROJECTNAME==OLYMPIC) Olympic::evacuateModeStart();
                MengeVis::SimViewer->_pause = false;//false����������
            }
			//����ͻ��˴������޸ĸ��ʾ�������ݣ����޸ĸ��ʾ���
			else if (!strcmp(command.c_str(), "Matrix")) {
				cout << "Modifying probability matrix " << endl;
				MengeVis::SimViewer->_pause = true;//false����������
				//��ͣ���������޸ĸ��ʾ���
				modifyMatrix((char*)matrix.c_str());
				MengeVis::SimViewer->_pause = false;
			}

			//�ظ��ͻ���
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
			//0.Ԥ�ȶ���������ߵ�agentgoalset�����ֻ��ߵ�goalset�޷���ǰ����ã���������ǰ������˶����goal��idƥ�䣬Ȼ��Ϊgoalset�������agentgoal,Ŀǰֻ���ֶ����
			//1.����agent��� 8:2 ��ͨ���ֻ� ����������
			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			//5.��дevent neighborhoodtrigger�б�дthemepark��agent�����߼�������������ض���agent�������½���evacuation״̬��ѡ��Ŀ��

			//1.����agent��ݣ���������
			int numAgent = SIMULATOR->getNumAgents();
			float normalPercentage = 0.8;
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				//�ο�
				if (agent->_class == 0) {
					//���������
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
					float random = ((rand() % 100) * 0.01);
					//normalAgent�ı���
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


			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			for (int i = 0; i < numAgent; i++) {
				cout << i << endl;
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);
				State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}

			//5.��дevent,event��Ҫ����״̬��
			evacuationState = true;


		}

	}

	namespace Olympic {
		void evacuateModeStart() {
			//0.Ԥ�ȶ���������ߵ�agentgoalset�����ֻ��ߵ�goalset�޷���ǰ����ã���������ǰ������˶����goal��idƥ�䣬Ȼ��Ϊgoalset�������agentgoal,Ŀǰֻ���ֶ����
			//1.����agent��� 8:2 ��ͨ���ֻ� ����������
			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			//5.��дevent neighborhoodtrigger�б�дthemepark��agent�����߼�������������ض���agent�������½���evacuation״̬��ѡ��Ŀ��

			//1.����agent��ݣ���������
			int numAgent = SIMULATOR->getNumAgents();
			float normalPercentage = 0.8;
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				//�ο�
				if (agent->_class == 0) {
					//���������
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
					float random = ((rand() % 100) * 0.01);
					//normalAgent�ı���
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


			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			for (int i = 0; i < numAgent; i++) {
				cout << i << endl;
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);
				State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.��дevent,event��Ҫ����״̬��
			evacuationState = true;


		}
	}
	
}




