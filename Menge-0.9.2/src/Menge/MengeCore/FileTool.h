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
		bool setRoadRegionFromXML(string dir, vector<Menge::Olympic::roadRegionType>& roadRegionInfo);//��XML�ļ������õ�·��Ϣ
		void testParallel(string dir);
		bool getIpFromXml(string dir, int& method, string& ip, int& port);
		void projectNameExtract(string folderPath);//��ȡ��Ŀ��
		void sceneParallelXML(string senceXmlFliePath);//��¼��Ⱥλ�ã�дһ���µ�xml�ļ�
		bool shopInit(string dir, map<int, Menge::Olympic::Shoptype>& shopInfo);
	}

	

}
#endif