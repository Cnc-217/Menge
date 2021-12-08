
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

#include "MengeVis/Viewer/GLViewer.h"
#include "MengeCore/Socket.h"
#include "MengeCore/FileTool.h"
#include <map>

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
				if (Menge::PROJECTNAME == OLYMPIC) 
					Olympic::evacuateModeStart();
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

			if (Menge::PROJECTNAME == OLYMPIC) {
				if (!strcmp(info.c_str(), "getData")) {
					string sendBuf = Olympic::getSimData();
					Menge::Socket::socketSend(sendBuf.c_str(), socketClient);
				}
				else if (!strcmp(info.c_str(), "getBlockPosition")) {
					string sendBuf = Olympic::getBlockPosition();
					Menge::Socket::socketSend(sendBuf.c_str(), socketClient);
				}
				else if (!strcmp(info.c_str(), "evacuate")) Olympic::evacuateModeStart();
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
					BaseScene::sceneParallelXML(parallelSceneFilePath);
				}
				else if (!strcmp(info.c_str(), "pause")) {
					MengeVis::SimViewer->_pause = true;
				}
				else if (!strcmp(info.c_str(), "restart")) {
					MengeVis::SimViewer->_pause = false;
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

	void BaseScene::projectNameExtract(string folderPath) {
		if (folderPath.find("Olympic") != folderPath.npos) {
			PROJECTNAME = OLYMPIC;
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
	
	void BaseScene::sceneParallelXML(string senceXmlFliePath) {
		TiXmlDocument xml(senceXmlFliePath);
		bool loadOkay = xml.LoadFile();

		if (!loadOkay) {	// load xml file
			cout << "load xml error: " << senceXmlFliePath << endl;
			exit(1);
		}

		TiXmlElement* experimentNode = xml.RootElement();
		if (!experimentNode) {
			cout << "load experimentNode error: " << endl;
			exit(1);
		}

		//Tags I'm not ready to parse - only parse agent sets and obstacles AFTER experiment
		//parameters
		TiXmlElement* child;
		for (child = experimentNode->FirstChildElement(); child; child = child->NextSiblingElement()) {
			//�ҵ���AgentGroup��������
			if (child->ValueStr() == "AgentGroup") {
				TiXmlElement* nodeChild;
				for (nodeChild = child->FirstChildElement(); nodeChild; nodeChild = nodeChild->NextSiblingElement()) {
					//�ҵ���ProfileSelector��������
					if (nodeChild->ValueStr() == "ProfileSelector") 
					{
						TiXmlAttribute* attr;
						attr = nodeChild->FirstAttribute();
						if (strncmp(attr->Name(), "name", 4) != 0 || strncmp(attr->Value(), "tourist", 7) != 0) 
							break;
					}
					//�ҵ���Generator��������
					if (nodeChild->ValueStr() == "Generator") {
						nodeChild->Clear();
						nodeChild->SetAttribute("type", "explicit");
						int numAgent = SIMULATOR->getNumAgents();
						for (int i = 0; i < numAgent; i++) {
							Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
							if (agent->_class == 0)//�ο�classΪ0   leader��classΪ1
							{
								TiXmlElement* node = new TiXmlElement("Agent");
								node->SetAttribute("p_x", to_string(agent->_pos._x));
								node->SetAttribute("p_y", to_string(agent->_pos._y));
								node->SetAttribute("goingTo", to_string(Menge::Olympic::agentGoingShop[i]));
								nodeChild->LinkEndChild(node);
							}
						}
					}

				}
			}
		}
		xml.SaveFile();
	}

	bool BaseScene::setRoadRegionFromXML(string dir){
		double Rad_to_deg = 45.0 / atan(1.0);//�Ƕ�ת����
		float data[5] = { 0 };
		int capacityData =  0 ;
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
		// ��¼�ƶ������ĸ�node�ڵ㣬���ҰѸ�node�ڵ����Ϣ¼�뵽˳���Ӧ��data��
		for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
		{ // ������ȡnode�ڵ����Ϣ
			Menge::Olympic::roadRegionType roadRegionTemp;//����һ����ʱ����
			nodeElem->QueryFloatAttribute("x", &data[0]);  //��x�ŵ�data[0]�У�����ֵ����
			nodeElem->QueryFloatAttribute("y", &data[1]);  //��y�ŵ�data[1]�У�����ֵ����
			nodeElem->QueryFloatAttribute("width", &data[2]);  //��width�ŵ�data[2]�У�����ֵ����
			nodeElem->QueryFloatAttribute("height", &data[3]);  //��height�ŵ�data[3]�У�����ֵ����
			nodeElem->QueryFloatAttribute("angle", &data[4]);  //��angle�ŵ�data[4]�У�����ֵ����
			nodeElem->QueryIntAttribute("capacity", &capacityData);  //��angle�ŵ�data[4]�У�����ֵ����
			roadRegionTemp.obbRoadbRegion.set(Vector2(data[0], data[1]), data[2], data[3], data[4] / Rad_to_deg);
			roadRegionTemp.peopleNumInThisRoad = 0;//��ʼ��Ϊ0
			roadRegionTemp.capacity = capacityData;
			roadRegionInfo.push_back(roadRegionTemp);//����
		}
		//for (int i = 0; i < roadRegionInfo.size(); i++)
			//cout << roadRegionInfo[i].obbRoadbRegion.getPivot() << roadRegionInfo[i].obbRoadbRegion.getSize() << roadRegionInfo[i].peopleNumInThisRoad << endl;
		return true;
	}

	void BaseScene::updateRoadNum()//��ÿһ������ ����������  �ض�  ͬʱ�������������������ĸ�����
	{
		agentInWhichRegion.assign(agentInWhichRegion.size(), -1);//��ȫ��ʼ����-1
		Menge::Math::OBBShape  region;
		int agentNum = Menge::SIMULATOR->getNumAgents();
		int num;
		int regionNum = roadRegionInfo.size();
		for (int roadId = 0; roadId < regionNum; roadId++)
		{
			region = roadRegionInfo[roadId].obbRoadbRegion;//ȡ��ǰ��Ҫ���µ�����
			num = 0;
			for (int agentID = 0; agentID < agentNum && agentInWhichRegion[agentID] == -1 ; agentID++)//�����в����κ�һ���������
			{
				Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(agentID);
				if (region.containsPoint(agent->_pos))//�����region����
				{
					num++;
					agentInWhichRegion[agentID] = roadId;//��agent���ڵ�����ĵ�  ������һ�ξ����ٱ���һ��
				}
			}
			roadRegionInfo[roadId].peopleNumInThisRoad = num;
		}
	}

	void BaseScene::updateAgentInRegion()//��ÿһ���� �������е�����  ��ͳ�������ĸ�����
	{
		agentInWhichRegion.assign(agentInWhichRegion.size(), -1);//��ȫ��ʼ����-1
		Agents::BaseAgent* agent;
		for (size_t agentId = 0; agentId < Menge::SIMULATOR->getNumAgents(); agentId++)
		{
			agent = Menge::SIMULATOR->getAgent(agentId);
			for (size_t regionId = 0; regionId < roadRegionInfo.size(); regionId++)
				if (roadRegionInfo[regionId].obbRoadbRegion.containsPoint(agent->_pos))
				{
					agentInWhichRegion[agentId] = regionId;
					break;
				}
		}
	}

<<<<<<< HEAD
	void BaseScene::testParallel(string dir)
	{
		string source = "Parallel";
		if (dir.find(source) != string::npos)
			parallelState = true;
=======
	bool BaseScene::testSwitchOn(string dir)
	{
		ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
		infile.open(dir);//���ļ�
		if (!infile.is_open())
		{
			cout << "open file error!" << endl;
			return false;
		}
		
		TiXmlDocument doc(dir);    // ����XML�ļ�
		if (!doc.LoadFile())
			return false;  // ����޷���ȡ�ļ����򷵻�
		TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
		TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
		pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
		TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�

		// ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
		TiXmlElement* nodeElem = hRoot.FirstChild("kon").Element(); //��ǰָ����Goal�ڵ�
		int  s;
		nodeElem->QueryIntAttribute("onoff", &s);
		return s ;
>>>>>>> fc1829b9771d9e306d62138437bcae1d101b51d6
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
			int data[4] = { 0 };
			ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
			infile.open(dir);//���ļ�

			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			//for (int i = 0; i < 10; i++)//������ѭ��
			//	for (int j = 0; j < 4; j++)//������ѭ��
			//		infile >> data[i][j];//��ȡһ��ֵ���ո��Ʊ�������и�������д�뵽�����У����в���ѭ������
			TiXmlDocument doc(dir);    // ����XML�ļ�
			if (!doc.LoadFile())
				return false;  // ����޷���ȡ�ļ����򷵻�
			TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
			TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
			pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
			TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�

			// ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
			TiXmlElement* nodeElem = hRoot.FirstChild("ShopSet").FirstChild("Shop").Element(); //��ǰָ����Goal�ڵ�
			int count = 0;  // ��¼�ƶ������ĸ�node�ڵ㣬���ҰѸ�node�ڵ����Ϣ¼�뵽˳���Ӧ��data��
			for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
			{ // ������ȡnode�ڵ����Ϣ
				nodeElem->QueryIntAttribute("type", &data[0]);  //��x�ŵ�data[count][0]�У�����ֵ����
				nodeElem->QueryIntAttribute("number", &data[1]);  //��y�ŵ�data[count][1]�У�����ֵ����
				nodeElem->QueryIntAttribute("serviceMax", &data[2]);  //��width�ŵ�data[count][2]�У�����ֵ����
				nodeElem->QueryIntAttribute("blockMax", &data[3]);  //��height�ŵ�data[count][3]�У�����ֵ����
				Shoptype shoptemp;
				for (int i = 0; i < data[1]; i++)
				{
					shoptemp.type = data[0];
					shoptemp.serviceMax = data[2];
					shoptemp.blockMax = data[3];
					shoptemp.goalSet = count;
					shoptemp.sameSetGoalNum = data[1];
					shopInfo.insert(make_pair(count, shoptemp));//����
					count++;
				}
			}
			infile.close();//��ȡ���֮��ر��ļ�
			int index = 0;
			/*for (int i = 0; i < 10; i++)
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
			}	*/
			

			return true;
			}

		bool getIpFromXml(string dir)
		{
			ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
			infile.open(dir);//���ļ�
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			TiXmlDocument doc(dir);    // ����XML�ļ�
			if (!doc.LoadFile())
				return false;  // ����޷���ȡ�ļ����򷵻�
			TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
			TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
			pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
			TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�
			TiXmlElement* nodeElem = hRoot.FirstChild("internet").Element(); //��ǰָ����internet�ڵ�
			nodeElem->QueryIntAttribute("methor", &methor);  //��x�ŵ�data[count][0]�У�����ֵ����
			nodeElem->QueryValueAttribute("ip", &ip);  //��x�ŵ�data[count][0]�У�����ֵ����
			nodeElem->QueryIntAttribute("port", &port);  //��x�ŵ�data[count][0]�У�����ֵ����
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
			std::vector <int>roadRegionNum(19, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) 
				agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			//BaseScene::updateRoadNum();
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
	
		string getBlockPosition() {
			vector<float> blockPosition;
			for (int i = 0; i < verticesCanGo.size(); i++) {
				if (verticesCanGo[i] == false) {
					blockPosition.push_back(Menge::GRAPH->getVertex(i)->getPosition().x());
					blockPosition.push_back(Menge::GRAPH->getVertex(i)->getPosition().y());
				}
			}
			json reply;
			reply["info"] = "blockSet";
			reply["data"] = blockPosition;
			string sendBuf = reply.dump();
			return sendBuf;
		}
}

}

