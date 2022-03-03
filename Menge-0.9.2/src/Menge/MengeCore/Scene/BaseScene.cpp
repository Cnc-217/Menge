
#include "MengeCore/Scene/BaseScene.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/Agents/Events/EventSystem.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/resources/Graph.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorEvacuation.h"

#include "MengeVis/Viewer/GLViewer.h"
#include "MengeCore/Socket.h"
#include "MengeCore/FileTool.h"
#include "MengeCore/BFSM/init.h"
#include <map>

using namespace std;
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
			string recevied_data = Menge::Socket::socketListen(serConn);
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
				int algorithmID = j["data"];
				cout << "here "<<algorithmID << endl;
				Menge::BFSM::EvacuationGoalSelector* evacuationGoalSelector = (Menge::BFSM::EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Evacuation")->getGoalSelector();
				evacuationGoalSelector->_algorithmID = algorithmID;
				Olympic::evacuateModeStart();
				json j;
				j["Info"] = "Menge has receive your commend: evacuation start";
				string sendBuf = j.dump();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);
			}
			//����ͻ��˴������޸ĸ��ʾ�������ݣ����޸ĸ��ʾ���
			else if (!strcmp(command.c_str(), "MatrixModify")) {
				cout << "Modifying probability matrix " << endl;
				//�޸ĸ��ʾ���

				modifyMatrix((char*)matrix.c_str());
				j["Info"] = "Menge has receive your commend: Modify matrix";
				string sendBuf = j.dump();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);

			}
			//����ͻ��˴�����FlowScene�����������������ݺ;���
			else if (!strcmp(command.c_str(), "FlowScene")) {
				cout << "FlowScene" << endl;

				string sendBuf = Olympic::matrixFlowScene();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);
			}
			else if (!strcmp(command.c_str(), "BusinessScene")) {
				cout << "BusinessScene" << endl;

				string sendBuf = Olympic::matrixBusinessScene();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);
			}
			//�ָ�����
			//MengeVis::SimViewer->_pause = false;

		}
	}

	void BaseScene::sockerClientListen(SOCKET socketClient) {
		while (true) {
			//�ȴ�����˵���Ϣ
			string recevied_data = Menge::Socket::socketListen(socketClient);

			//json����
			json j = json::parse(recevied_data);
			string info = j["info"];
			cout << "socketClient receive info: " << j["info"] << endl;

			if (!strcmp(info.c_str(), "getData")) {
				string sendBuf = Olympic::getSimData();
				Menge::Socket::socketSend(sendBuf.c_str(), socketClient);
			}
			else if (!strcmp(info.c_str(), "getBlockPosition")) {
				string sendBuf = Olympic::getBlockPosition();
				Menge::Socket::socketSend(sendBuf.c_str(), socketClient);
			}
			else if (!strcmp(info.c_str(), "evacuate")) {
				int algorithmID = j["data"];
				Menge::BFSM::EvacuationGoalSelector* evacuationGoalSelector = (Menge::BFSM::EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Evacuation")->getGoalSelector();
				evacuationGoalSelector->_algorithmID = algorithmID;
				Olympic::evacuateModeStart();
			}
			else if (!strcmp(info.c_str(), "parameters")) {
				if (Olympic::goalSeclectorType == "Matrix") {
					string matrix = j["matrix"];
					BaseScene::modifyMatrix((char*)matrix.c_str());
				}
				else if (Olympic::goalSeclectorType == "Model") {
					string tmp = j["influence"];
					char* strc = new char[strlen(tmp.c_str()) + 1];
					strcpy(strc, tmp.c_str());
					vector<float> vec = BaseScene::strToVectorFloat(strc);
					Menge::Olympic::Influence.assign(vec.begin(), vec.end());
					delete[] strc;
				}
				string tmp1 = j["roadblock"];
				cout << tmp1 << endl;
				char* strc1 = new char[strlen(tmp1.c_str()) + 1];
				strcpy(strc1, tmp1.c_str());
				vector<bool> vec1 = BaseScene::strToVectorBool(strc1);
				Menge::Olympic::verticesCanGo.assign(vec1.begin(), vec1.end());
				delete[] strc1;
			}
			else if (!strcmp(info.c_str(), "timeSlice")) {
				Menge::FileTool::copySceneFile();
				//Menge::FileTool::copyBehaveFile();
				std::size_t found = SceneFilePath.find_last_of("/");
				std::string parallelSceneFilePath = SceneFilePath.substr(0, found) + "/OlympicParallelS.xml";
				Menge::FileTool::sceneParallelXML(parallelSceneFilePath);
			}
			else if (!strcmp(info.c_str(), "pause")) {
				MengeVis::SimViewer->_pause = true;
			}
			else if (!strcmp(info.c_str(), "restart")) {
				MengeVis::SimViewer->_pause = false;
			}
			
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

	void BaseScene::modifyMatrix(char* matrixStr) {
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

	}
	
	vector<bool> BaseScene::strToVectorBool(char* listStr) {
		char* temp = strtok(listStr, " ");
		vector<bool> vec;
		while (temp != NULL) {
			//strcmp(temp, "False") ���ʱ����0
			if (strcmp(temp, "False")==0)vec.push_back(false);
			else if (strcmp(temp, "True")==0)vec.push_back(true);
			temp = strtok(NULL, " ");
		}
		return vec;
	}

	vector<float> BaseScene::strToVectorFloat(char* listStr) {
		char* temp = strtok(listStr, " ");
		vector<float> vec;
		while (temp != NULL) {
			vec.push_back(atof(temp));
			temp = strtok(NULL, " ");
		}
		return vec;
	}

	void BaseScene::updateRoadNum()//��ÿһ������ ����������  �ض�  ͬʱ�������������������ĸ�����
	{
		vector<Olympic::roadRegionType>* roadRegionInfo = ACTIVE_SCENE->roadRegionInfo;//�ֲ�����
		Olympic::agentInWhichRegion.assign(Olympic::agentInWhichRegion.size(), -1);//��ȫ��ʼ����-1
		Menge::Math::OBBShape  region;
		int agentNum = Menge::SIMULATOR->getNumAgents();
		int num;
		int regionNum = roadRegionInfo->size();
		for (int roadId = 0; roadId < regionNum; roadId++)
		{
			region = roadRegionInfo->at(roadId).obbRoadbRegion;//ȡ��ǰ��Ҫ���µ�����
			num = 0;
			for (int agentID = 0; agentID < agentNum; agentID++)//�����в����κ�һ���������
			{
				if (Olympic::agentInWhichRegion[agentID] == -1)
				{
					Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(agentID);
					if (region.containsPoint(agent->_pos))//�����region����
					{
						num++;
						Olympic::agentInWhichRegion[agentID] = roadId;//��agent���ڵ�����ĵ�  ������һ�ξ����ٱ���һ��
					}
				}
			}
			roadRegionInfo->at(roadId).peopleNumInThisRoad = num;
		}
	}

	void BaseScene::updateAgentInRegion()//��ÿһ���� �������е�����  ��ͳ�������ĸ�����
	{
		vector<Olympic::roadRegionType>* roadRegionInfo = ACTIVE_SCENE->roadRegionInfo;//�ֲ�����
		Olympic::agentInWhichRegion.assign(Olympic::agentInWhichRegion.size(), -1);//��ȫ��ʼ����-1
		Agents::BaseAgent* agent;
		for (size_t agentId = 0; agentId < Menge::SIMULATOR->getNumAgents(); agentId++)
		{
			agent = Menge::SIMULATOR->getAgent(agentId);
			for (size_t regionId = 0; regionId < roadRegionInfo->size(); regionId++)
				if (roadRegionInfo->at(regionId).obbRoadbRegion.containsPoint(agent->_pos))
				{
					Olympic::agentInWhichRegion[agentId] = regionId;
					break;
				}
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

			
			int AGENT_NUM = Menge::SIMULATOR->getNumAgents();
			for (int i = 0; i < AGENT_NUM; i++) AGENT_SCORES.push_back(0);
			for (int i = 0; i < AGENT_NUM; i++) AGENT_GOALS.push_back(-1);
			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);

				BFSM::State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);

				BFSM::State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.��дevent,event��Ҫ����״̬��
			startSimTime = Menge::SIM_TIME;
			evacuationState = true;
			cout << "evacuationState" << endl;


		}

		string matrixFlowScene() {
			//���� 1��36��Ŀ�������� 2�����ʾ���
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) 
				agentNumOfShop[i] = ACTIVE_SCENE->shopInfo->at(i).serviceQ.size() + ACTIVE_SCENE->shopInfo->at(i).blockQ.size();

			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json����
			json j;
			j["Info"] = "Menge has receive your commend: FlowScene";
			j["FlowData"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();

			return sendBuf;

		}

		string matrixBusinessScene() {
			//���� 1��36��Ŀ�������� 2�����ʾ���
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) 
				agentNumOfShop[i] = ACTIVE_SCENE->shopInfo->at(i).serviceQ.size() + ACTIVE_SCENE->shopInfo->at(i).blockQ.size();
			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json����
			json j;
			j["Info"] = "Menge has receive your commend: BusinessScene";
			j["BusinessScene"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();

			return sendBuf;
		}

		void parameterInit(SOCKET socketClient) {
			json j;
			//���յ���ԭʼ��Ϣ
			string recevied_data = Menge::Socket::socketListen(socketClient);
			//json����
			j = json::parse(recevied_data);
			cout << recevied_data << endl;
			if (j.find("matrix") != j.end()) {
				string matrix = j["matrix"];
				BaseScene::modifyMatrix((char*)matrix.c_str());
			}
			else if (j.find("influence") != j.end()) {
				string tmp = j["influence"];
				char* strc = new char[strlen(tmp.c_str()) + 1];
				strcpy(strc,tmp.c_str());
				vector<float> vec = BaseScene::strToVectorFloat(strc);
				Menge::Olympic::Influence.assign(vec.begin(), vec.end());
				delete[] strc;
			}
			string tmp = j["roadblock"];
			char* strc = new char[strlen(tmp.c_str()) + 1];
			strcpy(strc, tmp.c_str());
			vector<bool> vec = BaseScene::strToVectorBool(strc);
			Menge::Olympic::verticesCanGo.assign(vec.begin(), vec.end());
			delete[] strc;
		}
		
		string getSimData() {
			//���� 1��36��Ŀ��������
			std::vector<int> agentNumOfShop(36, 0);
			vector<roadRegionType>* roadRegionInfo = (ACTIVE_SCENE->roadRegionInfo);//�ֲ�����
			std::vector<int> roadRegionNum(roadRegionInfo->size(), 0);
			int numAgent = Menge::SIMULATOR->getNumAgents();
			std::vector<std::vector<int>> agentTrajectory(numAgent, std::vector<int>(30,0));
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) 
				agentNumOfShop[i] = ACTIVE_SCENE->shopInfo->at(i).serviceQ.size() + ACTIVE_SCENE->shopInfo->at(i).blockQ.size();
			for (int i = 0; i < roadRegionInfo->size(); i++)
				roadRegionNum[i] = roadRegionInfo->at(i).peopleNumInThisRoad;
			for (int i = 0; i < numAgent; i++)
				agentTrajectory[i].assign(Menge::SIMULATOR->getAgent(i)->_shopGone.begin(), Menge::SIMULATOR->getAgent(i)->_shopGone.end());
			//json����
			json j;
			j["info"] = "Menge has receive your commend: getData";
			j["data"] = agentNumOfShop;
			j["regionPopulation"] = roadRegionNum;
			j["agentTrajectory"] = agentTrajectory;
			j["time"] = Menge::SIMULATOR->getGlobalTime();
			if (evacuationState == true) j["evacuateTime"] = Menge::SIMULATOR->getGlobalTime() - Olympic::startSimTime;
			else j["evacuateTime"] = 0;
			string sendBuf = j.dump();
			return sendBuf;
		}
	
		string getBlockPosition() {
			vector<float> blockPosition;
			for (int i = 0; i < verticesCanGo.size(); i++) {
				if (verticesCanGo[i] == false) {
					blockPosition.push_back(Menge::GRAPH->getVertex(i)->getPosition().x());
					blockPosition.push_back(Menge::GRAPH->getVertex(i)->getPosition().y());
				}
			}
			json reply;
			reply["info"] = "setBlock";
			reply["data"] = blockPosition;
			string sendBuf = reply.dump();
			return sendBuf;
		}

		void evacuateExperiment() {
			//0.Ԥ�ȶ���������ߵ�agentgoalset�����ֻ��ߵ�goalset�޷���ǰ����ã���������ǰ������˶����goal��idƥ�䣬Ȼ��Ϊgoalset�������agentgoal,Ŀǰֻ���ֶ����
			//1.����agent��� 8:2 ��ͨ���ֻ� ����������
			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			//5.��дevent neighborhoodtrigger�б�дthemepark��agent�����߼�������������ض���agent�������½���evacuation״̬��ѡ��Ŀ��

			Menge::BFSM::EvacuationGoalSelector* selector = (Menge::BFSM::EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Evacuation")->getGoalSelector();
			selector->setAgentGoalSet();

			//1.����agent��ݣ���������
			int numAgent = SIMULATOR->getNumAgents();
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
				//normal
				if (agent->_class == 3) {
					normalAgentSet.push_back(agent);
				}//panic
				else if (agent->_class == 2) {
					panicAgentSet.push_back(agent);
				}
				//leader
				else if (agent->_class == 1) {
					leaderAgentSet.push_back(agent);
				}
			}

			startSimTime = Menge::SIM_TIME;
			AGENT_NUM = Menge::SIMULATOR->getNumAgents();			
			for (int i = 0; i < AGENT_NUM; i++) AGENT_SCORES.push_back(0);
			for (int i = 0; i < AGENT_NUM; i++) AGENT_GOALS.push_back(-1);
			startSimTime = Menge::SIM_TIME;

			//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
			//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
			//4.agentת�Ƶ���state
			for (int i = 0; i < numAgent; i++) {
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);

				BFSM::State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);

				BFSM::State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.��дevent,event��Ҫ����״̬��
			
			evacuationState = true;
		}
}

}

