
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
#include "MengeCore/Socket.h"
#include <map>

#define row 19//txt�ĵ��е�����
using namespace std;
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
				if (Menge::PROJECTNAME == OLYMPIC) Olympic::evacuateModeStart();
				json j;
				j["Info"] = "Menge has receive your commend: evacuation start";
				string sendBuf = j.dump();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);
				//MengeVis::SimViewer->_pause = false;//false����������
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
			string matrix = j["data"];
			cout << "socketClient receive data: " << j["data"] << endl;
			//���ص�json
			j.clear();

			if (Menge::PROJECTNAME == OLYMPIC) {
				if (!strcmp(info.c_str(), "getData")) {
					string sendBuf = Olympic::getSimData();
					Menge::Socket::socketSend(sendBuf.c_str(), socketClient);
				}
				else if (!strcmp(info.c_str(), "parameters")) modifyMatrix((char*)matrix.c_str());
				else if (!strcmp(info.c_str(), "evacuate")) {
					Olympic::evacuateModeStart();
				}
				
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

	void BaseScene::sendMatrix(SOCKET serConn, json j) {

		char sendBuf[100] = " ";
		//send(serConn, sendBuf, strlen(sendBuf) + 1, 0);

	}

	void BaseScene::projectNameExtract(string folderPath) {
		std::string tmp = folderPath;
		size_t ps = tmp.find_last_of("\\");
		size_t pe = tmp.length();
		std::string name = tmp.substr(ps + 1, pe - ps + 1);
		cout << name << endl;
		if (name.find("Olympic") != name.npos) {
			PROJECTNAME = OLYMPIC;
		}
	}

	bool BaseScene::setRoadRegionFromXML(string dir)
	{
		double Rad_to_deg = 45.0 / atan(1.0);//�Ƕ�ת����
		float data[row][5] = { 0 };
		int count = 0;
		TiXmlDocument doc(dir);    // ����XML�ļ�
		if (!doc.LoadFile())
			return false;  // ����޷���ȡ�ļ����򷵻�
		TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
		TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
		pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
		TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�

		// ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
		TiXmlElement* nodeElem = hRoot.FirstChild("GoalSet").FirstChild("Goal").Element(); //��ǰָ����Goal�ڵ�
		count = 0;  // ��¼�ƶ������ĸ�node�ڵ㣬���ҰѸ�node�ڵ����Ϣ¼�뵽˳���Ӧ��data��
		for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
		{ // ������ȡnode�ڵ����Ϣ
			nodeElem->QueryFloatAttribute("x", &data[count][0]);  //��x�ŵ�data[count][0]�У�����ֵ����
			nodeElem->QueryFloatAttribute("y", &data[count][1]);  //��y�ŵ�data[count][1]�У�����ֵ����
			nodeElem->QueryFloatAttribute("width", &data[count][2]);  //��width�ŵ�data[count][2]�У�����ֵ����
			nodeElem->QueryFloatAttribute("height", &data[count][3]);  //��height�ŵ�data[count][3]�У�����ֵ����
			nodeElem->QueryFloatAttribute("angle", &data[count][4]);  //��angle�ŵ�data[count][4]�У�����ֵ����
			count++;
		}
		Menge::Olympic::roadRegionType roadRegionTemp;//����һ����ʱ����
		for (int i = 0; i < row; i++)
		{//�ֱ���vector2��Xmin��Ymin������width����height
			roadRegionTemp.obbRoadbRegion.set(Vector2(data[i][0], data[i][1]), data[i][2], data[i][3], data[i][4] / Rad_to_deg);
			roadRegionTemp.peopleNumInThisRoad = 0;//��ʼ��Ϊ0
			roadRegionInfo.push_back(roadRegionTemp);//����
		}
		for (int i = 0; i < row; i++)
			cout << roadRegionInfo[i].obbRoadbRegion.getPivot() << roadRegionInfo[i].obbRoadbRegion.getSize() << data[i][4] << roadRegionInfo[i].peopleNumInThisRoad << endl;
		return true;
	}

	void BaseScene::updateRoadNum()
	{
		Menge::Math::OBBShape  region;
		for (int idx = 0; idx < roadRegionInfo.size(); idx++)
		{
			region = roadRegionInfo[idx].obbRoadbRegion;
			int num = checkRegion(region);
			roadRegionInfo[idx].peopleNumInThisRoad = num;
		}
	}

	int  BaseScene::checkRegion(Menge::Math::OBBShape region)
	{
		int regionNum = 0;
		for (int idx = 0; idx < Menge::SIMULATOR->getNumAgents(); idx++)//�����е�agent����
		{
			Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(idx);
			if (region.containsPoint(agent->_pos))//�����region����
				regionNum++;
		}
		return regionNum;
	};

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
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);

				BFSM::State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);

				BFSM::State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.��дevent,event��Ҫ����״̬��
			evacuationState = true;
			cout << "evacuationState" << endl;


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
					shoptemp.sameSetGoalNum = data[i][1];
					shopInfo.insert(make_pair(index, shoptemp));//����

					index++;
				}
			}
			for (int i = 0; i < 10; i++)
				goalSetInfo.insert(std::map< size_t, int >::value_type(i, data[i][1]));
			return true;
			
			}

		bool roadRegionInit(string dir) {//��ȡstop�ļ�  ��·�����������ļ�
			double Rad_to_deg = 45.0 / atan(1.0);//�Ƕ�ת����
			float data[row][5] = { 0 };
			ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
			infile.open(dir);//���ļ�
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			for (int i = 0; i < row; i++)//������ѭ��
				for (int j = 0; j < 5; j++)//������ѭ��
					infile >> data[i][j];//��ȡһ��ֵ���ո��Ʊ�������и�������д�뵽�����У����в���ѭ������
			infile.close();//��ȡ���֮��ر��ļ�
			Menge::Olympic::roadRegionType roadRegionTemp;//����һ����ʱ����
			for (int i = 0; i < row; i++)
			{//�ֱ���vector2��Xmin��Ymin������width����height
				roadRegionTemp.obbRoadbRegion.set(Vector2(data[i][0], data[i][1]), data[i][2], data[i][3], data[i][4]/ Rad_to_deg);
				roadRegionTemp.peopleNumInThisRoad = 0;//��ʼ��Ϊ0
				roadRegionInfo.push_back(roadRegionTemp);//����
			}
			for(int i = 0; i<row ;i++)
				cout << roadRegionInfo[i].obbRoadbRegion.getPivot() <<roadRegionInfo[i].obbRoadbRegion.getSize()<< roadRegionInfo[i].peopleNumInThisRoad<< endl;
			return true;

		}

		string matrixFlowScene() {
			//���� 1��36��Ŀ�������� 2�����ʾ���
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();

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
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
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
			string matrix = j["data"];
			BaseScene::modifyMatrix((char*)matrix.c_str());
		}

		

		string getSimData() {
			cout << "yes" << endl;
			//���� 1��36��Ŀ��������
			std::vector<int> agentNumOfShop(36, 0);
			std::vector <int>roadRegionNum(19, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) 
				agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			BaseScene::updateRoadNum();
			for (int i = 0; i < roadRegionInfo.size(); i++)
				roadRegionNum[i] = roadRegionInfo[i].peopleNumInThisRoad;
			//json����
			json j;
			j["info"] = "Menge has receive your commend: getData";
			j["data"] = agentNumOfShop;
			j["regionPopulation"] = roadRegionNum;
			string sendBuf = j.dump();
			return sendBuf;
		}
	}

}

