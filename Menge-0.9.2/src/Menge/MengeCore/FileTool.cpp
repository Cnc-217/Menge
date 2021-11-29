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
	}

}