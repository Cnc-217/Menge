
#ifndef __BaseScene_H__
#define	__BaseScene_H__


#include <string>
#include <winsock.h>
#include "MengeCore/MatrixMy.h"
#include "MengeCore/Json/json.hpp"
#pragma comment(lib, "ws2_32.lib")
/*!
 * @namespace Menge
 * @brief	BaseScene
 */
using namespace nlohmann;
namespace Menge {
    namespace BaseScene {
        
        void sockerServerListen(SOCKET socketServer);
        void loadMatrixFromTxt(const char* fileName);
        void modifyMatrix(char* matrixStr, SOCKET serConn,json j);
        void sendMatrix(SOCKET serConn, json j);
    }

    namespace ThemePark {
        void evacuateModeStart(SOCKET serConn, json j);
    }


    namespace Olympic {
        void evacuateModeStart(SOCKET serConn, json j);

        void sendMatrixFlowScene(SOCKET serConn, json j);

        void sendMatrixBusinessScene(SOCKET serConn, json j);

    }

}	// namespace Menge
#endif	//__BaseScene_H__
