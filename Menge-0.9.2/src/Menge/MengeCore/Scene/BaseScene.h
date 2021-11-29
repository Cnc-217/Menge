
#ifndef __BaseScene_H__
#define	__BaseScene_H__

#include "MengeCore/MatrixMy.h"
#include "MengeCore/Json/json.hpp"
#include "tinyxml/tinyxml.h"
#include <string>
#include <winsock.h>
#include<Menge/MengeCore/Math/Geometry2D.h>


/*!
 * @namespace Menge
 * @brief	BaseScene
 */
using namespace nlohmann;
using namespace std;

namespace Menge {
    
    namespace BaseScene {
        
        void sockerServerListen(SOCKET socketServer);
        void sockerClientListen(SOCKET socketClient);
        void loadMatrixFromTxt(const char* fileName);
        void modifyMatrix(char* matrixStr);
        vector<bool> strToVectorBool (char* listStr);
        vector<float> strToVectorFloat (char* listStr);
        void projectNameExtract(string folderPath);//提取项目名
<<<<<<< HEAD
        void sceneParallelXML(string senceXmlFliePath);//记录人群位置，写一个新的xml文件
=======
		bool setRoadRegionFromXML(string dir);//设置道路信息
		int checkRegion(Menge::Math::OBBShape region);
		void updateRoadNum();

>>>>>>> f684803ff931159ac6e86cfa3b6bdc4fcab9f200
    }

    namespace Olympic {

        void evacuateModeStart();

        string matrixFlowScene();

        string matrixBusinessScene();

		bool shopInit(string dir);

		bool roadRegionInit(string dir);

        void parameterInit(SOCKET socketClient);

        string getSimData();

    }

}	// namespace Menge
#endif	//__BaseScene_H__
