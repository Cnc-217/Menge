
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
			//�ȴ��ͻ��˵�����
			serConn = accept(socketServer, (SOCKADDR*)& clientsocket, &len);

			//���տͻ��˴��������ݴ�С
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			int dataSize = atoi(receiveBuf);
			cout << "datasize: " << receiveBuf << endl;
			string receiveSignal = "ok";
			send(serConn, receiveSignal.c_str(), strlen(receiveSignal.c_str()), 0);

			//ѭ�����տͻ�������
			int recevied_size = 0;
			string recevied_data = "";
			int lenBuf;
			//�����յ����ݴ�С С�� �ͻ��˷���������
			while (recevied_size < dataSize) {
				memset(receiveBuf, '\0', sizeof(receiveBuf));
				lenBuf = recv(serConn, receiveBuf, 1023, 0);
				cout << "dataPartSize: " << lenBuf << endl;
				recevied_size += lenBuf;  //ÿ���յ��ķ���˵������п���С��1024�����Ա�����len�ж�
				receiveBuf[lenBuf] = '\0';
				recevied_data += receiveBuf;
				cout << "recevie_buf: " << receiveBuf << endl;
			}

			//json����
			json j = json::parse(recevied_data);
			string command = j["command"];
			cout << "socketServer receive command: " << j["command"] << endl;
			string matrix = j["data"];
			cout << "socketServer receive data: " << j["data"] << endl;

			//���ص�json
			j.clear();

			//��ͣ������matrix���ݣ�false����������
			//ע�⣺Unity�е�MengeVis DLL�޷��������룬����ָ��ᵼ�±�������ζ��Unity���޷��Է��������ͣ
			//MengeVis::SimViewer->_pause = true;
			//����ͻ��˴�����Evacuate�������״̬ת�Ƶĺ���
			if (!strcmp(command.c_str(), "Evacuate")) {
				cout << "Evacuate mode start" << endl;
				//�����Ǹ��ݲ�ͬ��Ŀ�Ķ��ƻ����벿��
				if (Menge::PROJECTNAME == THEMEPARK) ThemePark::evacuateModeStart(serConn, j);
				else if (Menge::PROJECTNAME == OLYMPIC) Olympic::evacuateModeStart(serConn, j);
				//MengeVis::SimViewer->_pause = false;//false����������
			}
			//����ͻ��˴������޸ĸ��ʾ�������ݣ����޸ĸ��ʾ���
			else if (!strcmp(command.c_str(), "MatrixModify")) {
				cout << "Modifying probability matrix " << endl;
				//�޸ĸ��ʾ���
				modifyMatrix((char*)matrix.c_str(), serConn, j);
			}
			//����ͻ��˴�����FlowScene�����������������ݺ;���
			else if (!strcmp(command.c_str(), "FlowScene")) {
				cout << "FlowScene" << endl;
				Olympic::sendMatrixFlowScene(serConn, j);
			}
			else if (!strcmp(command.c_str(), "BusinessScene")) {
				cout << "BusinessScene" << endl;
				Olympic::sendMatrixBusinessScene(serConn, j);
			}
			//�ָ�����
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

		//�ظ��ͻ���
		j["Info"] = "Menge has receive your commend: Modify matrix";
		string sendBuf = j.dump();
		string len = to_string(strlen(sendBuf.c_str()));
		//�������Σ���һ�������ݳ��ȣ��ڶ���������
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

			//�ظ��ͻ���
			j["Info"] = "Menge has receive your commend: ThemePark evacuation start";
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//�������Σ���һ�������ݳ��ȣ��ڶ���������
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
		}

	}

	namespace Olympic {
		void evacuateModeStart(SOCKET serConn, json j) {
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
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);
				State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.��дevent,event��Ҫ����״̬��
			evacuationState = true;

			//�ظ��ͻ���
			j["Info"] = "Menge has receive your commend: Olympic evacuation start";
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//�������Σ���һ�������ݳ��ȣ��ڶ���������
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);

		}

		bool shopInit(string dir) {
			int data[10][4] = { 0 };
			ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
			infile.open(dir);//���ļ�
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			for (int i = 0; i < 10; i++)//������ѭ��
				for (int j = 0; j < 4; j++)//������ѭ��
					infile >> data[i][j];//��ȡһ��ֵ���ո��Ʊ�������и�������д�뵽�����У����в���ѭ������
			infile.close();//��ȡ���֮��ر��ļ�
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
					shopInfo.insert(make_pair(index, shoptemp));//����
					index++;
				}
			}
			for (int i = 0; i < 10; i++)
				goalSetInfo.insert(std::map< size_t, int >::value_type(i, data[i][1]));
			//for (int i = 0; i < 10; i++)
				//cout<<goalSetInfo[i]<<endl;
			return true;
			//for (int i = 0; i < 36; i++)//������
			//	cout << shopInfo[index].blockMax << "+" << shopInfo[i].serviceMax << "+" << shopInfo[i].type <<"+"<< endl;
			}


		void sendMatrixFlowScene(SOCKET serConn, json j) {
			//���� 1��36��Ŀ�������� 2�����ʾ���
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();

			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json����
			j["Info"] = "Menge has receive your commend: FlowScene";
			j["FlowData"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//�������Σ���һ�������ݳ��ȣ��ڶ���������
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);

		}

		void sendMatrixBusinessScene(SOCKET serConn, json j) {
			//���� 1��36��Ŀ�������� 2�����ʾ���
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json����
			j["Info"] = "Menge has receive your commend: BusinessScene";
			j["BusinessScene"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();
			string len = to_string(strlen(sendBuf.c_str()));
			//�������Σ���һ�������ݳ��ȣ��ڶ���������
			send(serConn, len.c_str(), strlen(len.c_str()), 0);
			char receiveBuf[1024] = {};
			recv(serConn, receiveBuf, 1024, 0);
			send(serConn, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
		}

	}

}

