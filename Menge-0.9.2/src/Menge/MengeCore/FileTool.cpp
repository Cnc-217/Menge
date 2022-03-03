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
			//FALSE:如果目标位置已经存在同名文件，就覆盖，return 1
			//TRUE:如果目标位置已经存在同名文件，则补拷贝，return 0
			//后者路径若不错在，return 0
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
			ifstream infile;//定义读取文件流，相对于程序来说是in
			infile.open(dir);//打开文件
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}

			TiXmlDocument doc(dir);    // 读入XML文件
			if (!doc.LoadFile())
				return false;  // 如果无法读取文件，则返回
			TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
			TiXmlElement* pElem;            // 指向元素的指针
			pElem = hDoc.FirstChildElement().Element(); //指向根节点
			TiXmlHandle hRoot(pElem);       // hRoot是根节点

			// 读取x,y，它们放在network->nodes->node节点中
			TiXmlElement* nodeElem = hRoot.FirstChild(teststr).Element(); //当前指向了Goal节点
			int  s;
			nodeElem->QueryIntAttribute("onoff", &s);
			return s;
		}

		vector<bool> testAllConfigOpen(string dir)
		{
			vector<bool> result;
			ifstream infile;//定义读取文件流，相对于程序来说是in
			infile.open(dir);//打开文件
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				exit(0);
			}
			TiXmlDocument doc(dir);    // 读入XML文件
			if (!doc.LoadFile())
				exit(0);  // 如果无法读取文件，则返回
			TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
			TiXmlElement* pElem;            // 指向元素的指针
			pElem = hDoc.FirstChildElement().Element(); //指向根节点
			TiXmlHandle hRoot(pElem);       // hRoot是根节点

			// 读取x,y，它们放在network->nodes->node节点中
			ConfigVectorPushBack(hRoot, "RoadConfig", result);
			ConfigVectorPushBack(hRoot, "internetConfig", result);
			ConfigVectorPushBack(hRoot, "ProbConfig", result);
			return result;
		}

		void ConfigVectorPushBack(TiXmlHandle hRoot, string testStr, vector<bool>& res)
		{
			int  s;
			TiXmlElement* nodeElem = hRoot.FirstChild(testStr).Element(); //当前指向了Goal节点
			nodeElem->QueryIntAttribute("onoff", &s);
			if (s)
				res.push_back(true);
			else
				res.push_back(false);
		}

		bool setRoadRegionFromXML(string dir, std::vector <Menge::Olympic::roadRegionType>& roadRegionInfo) {
			double Rad_to_deg = 45.0 / atan(1.0);//角度转弧度
			float data[5] = { 0 };
			int capacityData = 0;
			int count = 0;
			TiXmlDocument doc(dir);    // 读入XML文件
			if (!doc.LoadFile())
				return false;  // 如果无法读取文件，则返回
			TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
			TiXmlElement* pElem;            // 指向元素的指针
			pElem = hDoc.FirstChildElement().Element(); //指向根节点
			TiXmlHandle hRoot(pElem);       // hRoot是根节点

			// 读取x,y，它们放在network->nodes->node节点中
			TiXmlElement* nodeElem = hRoot.FirstChild("GoalSet").FirstChild("Goal").Element(); //当前指向了Goal节点
			// 记录移动到了哪个node节点，并且把该node节点的信息录入到顺序对应的data中
			for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
			{ // 挨个读取node节点的信息
				Menge::Olympic::roadRegionType roadRegionTemp;//声明一个临时变量
				nodeElem->QueryFloatAttribute("x", &data[0]);  //把x放到data[0]中，属性值读法
				nodeElem->QueryFloatAttribute("y", &data[1]);  //把y放到data[1]中，属性值读法
				nodeElem->QueryFloatAttribute("width", &data[2]);  //把width放到data[2]中，属性值读法
				nodeElem->QueryFloatAttribute("height", &data[3]);  //把height放到data[3]中，属性值读法
				nodeElem->QueryFloatAttribute("angle", &data[4]);  //把angle放到data[4]中，属性值读法
				nodeElem->QueryIntAttribute("capacity", &capacityData);  //把angle放到data[4]中，属性值读法
				roadRegionTemp.obbRoadbRegion.set(Menge::Math::Vector2(data[0], data[1]), data[2], data[3], data[4] / Rad_to_deg);
				roadRegionTemp.peopleNumInThisRoad = 0;//初始化为0
				roadRegionTemp.capacity = capacityData;
				roadRegionInfo.push_back(roadRegionTemp);//插入
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
			ifstream infile;//定义读取文件流，相对于程序来说是in
			infile.open(dir);//打开文件
			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			TiXmlDocument doc(dir);    // 读入XML文件
			if (!doc.LoadFile())
				return false;  // 如果无法读取文件，则返回
			TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
			TiXmlElement* pElem;            // 指向元素的指针
			pElem = hDoc.FirstChildElement().Element(); //指向根节点
			TiXmlHandle hRoot(pElem);       // hRoot是根节点
			TiXmlElement* nodeElem = hRoot.FirstChild("internet").Element(); //当前指向了internet节点
			nodeElem->QueryIntAttribute("method", &choose);  //把x放到method中，属性值读法
			nodeElem->QueryValueAttribute("ip", &ip);  //把x放到ip中，属性值读法
			nodeElem->QueryIntAttribute("port", &port);  //把x放到port中，属性值读法
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
				//找到了AgentGroup的属性组
				if (child->ValueStr() == "AgentGroup") {
					TiXmlElement* nodeChild;
					for (nodeChild = child->FirstChildElement(); nodeChild; nodeChild = nodeChild->NextSiblingElement()) {
						//找到了ProfileSelector的属性组
						if (nodeChild->ValueStr() == "ProfileSelector")
						{
							TiXmlAttribute* attr;
							attr = nodeChild->FirstAttribute();
							if (strncmp(attr->Name(), "name", 4) != 0 || strncmp(attr->Value(), "tourist", 7) != 0)
								break;
						}
						//找到了Generator的属性组
						if (nodeChild->ValueStr() == "Generator") {
							nodeChild->Clear();
							nodeChild->SetAttribute("type", "explicit");
							int numAgent = SIMULATOR->getNumAgents();
							for (int i = 0; i < numAgent; i++) {
								Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
								if (agent->_class == 0)//游客class为0   leader的class为1
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
			ifstream infile;//定义读取文件流，相对于程序来说是in
			infile.open(dir);//打开文件

			if (!infile.is_open())
			{
				cout << "open file error!" << endl;
				return false;
			}
			TiXmlDocument doc(dir);    // 读入XML文件
			if (!doc.LoadFile())
				return false;  // 如果无法读取文件，则返回
			TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
			TiXmlElement* pElem;            // 指向元素的指针
			pElem = hDoc.FirstChildElement().Element(); //指向根节点
			TiXmlHandle hRoot(pElem);       // hRoot是根节点

			// 读取x,y，它们放在network->nodes->node节点中
			TiXmlElement* nodeElem = hRoot.FirstChild("ShopSet").FirstChild("Shop").Element(); //当前指向了Goal节点
			int count = 0;  // 记录移动到了哪个node节点，并且把该node节点的信息录入到顺序对应的data中
			for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
			{ // 挨个读取node节点的信息
				nodeElem->QueryIntAttribute("type", &data[0]);  //把x放到data[count][0]中，属性值读法
				nodeElem->QueryIntAttribute("number", &data[1]);  //把y放到data[count][1]中，属性值读法
				nodeElem->QueryIntAttribute("serviceMax", &data[2]);  //把width放到data[count][2]中，属性值读法
				nodeElem->QueryIntAttribute("blockMax", &data[3]);  //把height放到data[count][3]中，属性值读法
				Menge::Olympic::Shoptype shoptemp;
				for (int i = 0; i < data[1]; i++)
				{
					shoptemp.type = data[0];
					shoptemp.serviceMax = data[2];
					shoptemp.blockMax = data[3];
					shopInfo.insert(make_pair(count, shoptemp));//插入
					count++;
				}
			}
			infile.close();//读取完成之后关闭文件
			return true;
		}

	}

}