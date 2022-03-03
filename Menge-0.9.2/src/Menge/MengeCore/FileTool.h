#pragma once
#ifndef __FILETOOL__
#define __FILETOOL__

#include <stdlib.h>
#include<iostream>
#include<fstream>
#include <windows.h>
#include <vector>

#include "MengeCore/Core.h"
#include"MengeCore/Math/Geometry2D.h"
#include"MengeCore/Math/Vector2.h"
#include "MengeCore/Agents/SimulatorInterface.h"

using namespace std;
namespace Menge{

	namespace FileTool {
		void copyBehaveFile();
		void copySceneFile();
		string getDirectoryPath(string str);
		bool testConfigOpen(string dir, string teststr);
		std::vector<bool> testAllConfigOpen(string dir);
		void ConfigVectorPushBack(TiXmlHandle hRoot, string testStr, vector<bool>& res);
		bool setRoadRegionFromXML(string dir, vector<Menge::Olympic::roadRegionType>& roadRegionInfo);//从XML文件中设置道路信息
		void testParallel(string dir);
		bool getIpFromXml(string dir, int& method, string& ip, int& port);
		void projectNameExtract(string folderPath);//提取项目名
		void sceneParallelXML(string senceXmlFliePath);//记录人群位置，写一个新的xml文件
		bool shopInit(string dir, map<int, Menge::Olympic::Shoptype>& shopInfo);
	}

	

}
#endif