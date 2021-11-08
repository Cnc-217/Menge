
#ifndef __BaseScene_H__
#define	__BaseScene_H__

#include "MengeCore/MatrixMy.h"
#include "MengeCore/Json/json.hpp"
#include <string>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")


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
        void sendMatrix(SOCKET serConn, json j);
        void projectNameExtract(string folderPath);//提取项目名

    }

    namespace ThemePark {
        void evacuateModeStart();
    }


    namespace Olympic {

        void evacuateModeStart();

        string matrixFlowScene();

        string matrixBusinessScene();


		bool shopInit(string dir);

        void parameterInit(SOCKET socketClient);

        string getSimData();

    }

}	// namespace Menge
#endif	//__BaseScene_H__
