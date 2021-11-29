
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

#include "MengeVis/Viewer/GLViewer.h"
#include "MengeCore/Socket.h"
#include "MengeCore/FileTool.h"
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
			//等待客户端的连接
			serConn = accept(socketServer, (SOCKADDR*)& clientsocket, &len);


			string recevied_data = Menge::Socket::socketListen(serConn);


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
				if (Menge::PROJECTNAME == OLYMPIC) Olympic::evacuateModeStart();
				json j;
				j["Info"] = "Menge has receive your commend: evacuation start";
				string sendBuf = j.dump();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);
				//MengeVis::SimViewer->_pause = false;//false是正常运行
			}
			//如果客户端传来了修改概率矩阵的数据，则修改概率矩阵
			else if (!strcmp(command.c_str(), "MatrixModify")) {
				cout << "Modifying probability matrix " << endl;
				//修改概率矩阵

				modifyMatrix((char*)matrix.c_str());
				j["Info"] = "Menge has receive your commend: Modify matrix";
				string sendBuf = j.dump();
				Menge::Socket::socketSend(sendBuf.c_str(), serConn);

			}
			//如果客户端传来了FlowScene的请求，则发送人流数据和矩阵
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
			//恢复运行
			//MengeVis::SimViewer->_pause = false;

		}
	}

	void BaseScene::sockerClientListen(SOCKET socketClient) {
		while (true) {
			//等待服务端的信息
			string recevied_data = Menge::Socket::socketListen(socketClient);

			//json解析
			json j = json::parse(recevied_data);
			string info = j["info"];
			cout << "socketClient receive info: " << j["info"] << endl;

			if (Menge::PROJECTNAME == OLYMPIC) {
				if (!strcmp(info.c_str(), "getData")) {
					string sendBuf = Olympic::getSimData();
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
			//strcmp(temp, "False") 相等时返回0
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
			cout << "load xml error: "<< senceXmlFliePath <<endl;
			exit(1);
		}

		TiXmlElement* experimentNode = xml.RootElement();
		if (!experimentNode) {
			cout << "load experimentNode error: "  << endl;
			exit(1);
		}

		//Tags I'm not ready to parse - only parse agent sets and obstacles AFTER experiment
		//parameters
		TiXmlElement* child;
		for (child = experimentNode->FirstChildElement(); child; child = child->NextSiblingElement()) {
			//找到了AgentGroup的属性组
			if (child->ValueStr() == "AgentGroup") {
				TiXmlElement* nodeChild;
				for (nodeChild = child->FirstChildElement();nodeChild;nodeChild = nodeChild->NextSiblingElement()) {
					//找到了ProfileSelector的属性组
					if (nodeChild->ValueStr() == "ProfileSelector") {
						TiXmlAttribute* attr;
						attr = nodeChild->FirstAttribute();
						if (strncmp(attr->Name(), "name", 4) != 0 || strncmp(attr->Value(), "tourist", 7) != 0) break;
					}
					//找到了Generator的属性组
					if (nodeChild->ValueStr() == "Generator") {
						nodeChild->Clear();
						nodeChild->SetAttribute("type", "explicit");
						int numAgent = SIMULATOR->getNumAgents();
						for (int i = 0; i < numAgent; i++) {
							Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
							TiXmlElement* node = new TiXmlElement("Agent");
							node->SetAttribute("p_x", to_string(agent->_pos._x));
							node->SetAttribute("p_y", to_string(agent->_pos._y));
							nodeChild->LinkEndChild(node);
						}
					}

				}
			}
		}
		xml.SaveFile();

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
				Agents::BaseAgent* agent = SIMULATOR->getAgent(i);

				BFSM::State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				currentState->leave(agent);

				BFSM::State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
				nextState->enter(agent);
				Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
			}


			//5.编写event,event需要启动状态码
			evacuationState = true;
			cout << "evacuationState" << endl;


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
					shoptemp.sameSetGoalNum = data[i][1];
					shopInfo.insert(make_pair(index, shoptemp));//插入

					index++;
				}
			}
			for (int i = 0; i < 10; i++)
				goalSetInfo.insert(std::map< size_t, int >::value_type(i, data[i][1]));
			return true;
			
			}


		string matrixFlowScene() {
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();

			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json生成
			json j;
			j["Info"] = "Menge has receive your commend: FlowScene";
			j["FlowData"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();

			return sendBuf;

		}


		string matrixBusinessScene() {
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			vector<vector<float>> matrixVector = Menge::BaseScene::ProbMatrix->toVector();
			//json生成
			json j;
			j["Info"] = "Menge has receive your commend: BusinessScene";
			j["BusinessScene"] = agentNumOfShop;
			j["Matrix"] = matrixVector;
			string sendBuf = j.dump();

			return sendBuf;
		}

		void parameterInit(SOCKET socketClient) {
			json j;
			//接收到的原始信息
			string recevied_data = Menge::Socket::socketListen(socketClient);
			//json解析
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
			//发送 1：36个目标点的人数 2：概率矩阵
			std::vector<int> agentNumOfShop(36, 0);
			for (int i = 0; i < Menge::ACTIVE_FSM->getGoalSet(0)->size(); i++) agentNumOfShop[i] = shopInfo[i].serviceQ.size() + shopInfo[i].blockQ.size();
			//json生成
			json j;
			j["info"] = "Menge has receive your commend: getData";
			j["data"] = agentNumOfShop;
			string sendBuf = j.dump();
			return sendBuf;
		}
	}

}

