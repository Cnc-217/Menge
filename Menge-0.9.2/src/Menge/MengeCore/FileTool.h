#pragma once
#ifndef __FILETOOL__
#define __FILETOOL__

#include <stdlib.h>
#include<iostream>
#include<fstream>
#include <windows.h>

using namespace std;
namespace Menge{

	namespace FileTool {

		void copyBehaveFile();
		void copySceneFile();
		string getDirectoryPath(string str);

	}

}
#endif