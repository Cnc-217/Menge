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
	}

}