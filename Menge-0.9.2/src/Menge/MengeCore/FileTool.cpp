#include "FileTool.h"
#include "MengeCore/Core.h"


using namespace std;

namespace Menge {

	namespace FileTool {

		/*
		std::cout << " path: " << str.substr(0,found) << '\n';
		std::cout << " file: " << str.substr(found+1) << '\n';
		*/

		void copyBehaveFile() {
			string behave = BehaveFilePath;
			std::size_t found = behave.find_last_of("/");
			std::string parallelFilePath = behave.substr(0, found) + "/OlympicParallelB.xml";
			CopyFile(behave.c_str(), parallelFilePath.c_str(), FALSE);
			//FALSE:���Ŀ��λ���Ѿ�����ͬ���ļ����͸��ǣ�return 1
			//TRUE:���Ŀ��λ���Ѿ�����ͬ���ļ����򲹿�����return 0
			//����·���������ڣ�return 0
		}

		void copySceneFile() {
			string scene = SceneFilePath;
			std::size_t found = scene.find_last_of("/");
			std::string parallelFilePath = scene.substr(0, found) + "/OlympicParallelS.xml";
			CopyFile(scene.c_str(), parallelFilePath.c_str(), FALSE);
		}

		string getDirectoryPath(string str) {
			std::size_t found = str.find_last_of("/");
			string filePath = str.substr(0, found);
			return filePath;
		}

		bool testConfigOpen(string dir, string teststr)
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
			TiXmlElement* nodeElem = hRoot.FirstChild(teststr).Element(); //��ǰָ����Goal�ڵ�
			int  s;
			nodeElem->QueryIntAttribute("onoff", &s);
			return s;
		}

		vector<bool> testAllConfigOpen(string dir)
		{
			vector<bool> result;
			ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
			infile.open(dir);//���ļ�
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				exit(0);
			}
			TiXmlDocument doc(dir);    // ����XML�ļ�
			if (!doc.LoadFile())
				exit(0);  // ����޷���ȡ�ļ����򷵻�
			TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
			TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
			pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
			TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�

			// ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
			ConfigVectorPushBack(hRoot, "RoadConfig", result);
			ConfigVectorPushBack(hRoot, "internetConfig", result);
			ConfigVectorPushBack(hRoot, "ProbConfig", result);
			return result;
		}

		void ConfigVectorPushBack(TiXmlHandle hRoot, string testStr, vector<bool>& res)
		{
			int  s;
			TiXmlElement* nodeElem = hRoot.FirstChild(testStr).Element(); //��ǰָ����Goal�ڵ�
			nodeElem->QueryIntAttribute("onoff", &s);
			if (s)
				res.push_back(true);
			else
				res.push_back(false);
		}

		bool setRoadRegionFromXML(string dir, std::vector <Menge::Olympic::roadRegionType>& roadRegionInfo) {
			double Rad_to_deg = 45.0 / atan(1.0);//�Ƕ�ת����
			float data[5] = { 0 };
			int capacityData = 0;
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
				roadRegionTemp.obbRoadbRegion.set(Menge::Math::Vector2(data[0], data[1]), data[2], data[3], data[4] / Rad_to_deg);
				roadRegionTemp.peopleNumInThisRoad = 0;//��ʼ��Ϊ0
				roadRegionTemp.capacity = capacityData;
				roadRegionInfo.push_back(roadRegionTemp);//����
			}
			for (int i = 0; i < roadRegionInfo.size(); i++)
				cout << roadRegionInfo[i].obbRoadbRegion.getPivot() << roadRegionInfo[i].obbRoadbRegion.getSize() << roadRegionInfo[i].peopleNumInThisRoad << endl;
			return true;
		}

		void testParallel(string dir)
		{
			string source = "Parallel";
			if (dir.find(source) != string::npos)
				Menge::Olympic::parallelState = true;
		}

		bool getIpFromXml(string dir, int& choose, string& ip, int& port)
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
			nodeElem->QueryIntAttribute("method", &choose);  //��x�ŵ�method�У�����ֵ����
			nodeElem->QueryValueAttribute("ip", &ip);  //��x�ŵ�ip�У�����ֵ����
			nodeElem->QueryIntAttribute("port", &port);  //��x�ŵ�port�У�����ֵ����
			return true;
		}

		void projectNameExtract(string folderPath) {
			if (folderPath.find("Olympic") != folderPath.npos) {
				PROJECTNAME = OLYMPIC;
			}

		}

		void sceneParallelXML(string senceXmlFliePath) {
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

		bool shopInit(string dir, map<int, Menge::Olympic::Shoptype>& shopInfo) {
			int data[4] = { 0 };
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
			TiXmlElement* nodeElem = hRoot.FirstChild("ShopSet").FirstChild("Shop").Element(); //��ǰָ����Goal�ڵ�
			int count = 0;  // ��¼�ƶ������ĸ�node�ڵ㣬���ҰѸ�node�ڵ����Ϣ¼�뵽˳���Ӧ��data��
			for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
			{ // ������ȡnode�ڵ����Ϣ
				nodeElem->QueryIntAttribute("type", &data[0]);  //��x�ŵ�data[count][0]�У�����ֵ����
				nodeElem->QueryIntAttribute("number", &data[1]);  //��y�ŵ�data[count][1]�У�����ֵ����
				nodeElem->QueryIntAttribute("serviceMax", &data[2]);  //��width�ŵ�data[count][2]�У�����ֵ����
				nodeElem->QueryIntAttribute("blockMax", &data[3]);  //��height�ŵ�data[count][3]�У�����ֵ����
				Menge::Olympic::Shoptype shoptemp;
				for (int i = 0; i < data[1]; i++)
				{
					shoptemp.type = data[0];
					shoptemp.serviceMax = data[2];
					shoptemp.blockMax = data[3];
					shopInfo.insert(make_pair(count, shoptemp));//����
					count++;
				}
			}
			infile.close();//��ȡ���֮��ر��ļ�
			return true;
		}

	}

}