
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
<<<<<<< HEAD
        void evacuateModeStart(SOCKET serConn, json j);

        void sendMatrixFlowScene(SOCKET serConn, json j);

        void sendMatrixBusinessScene(SOCKET serConn, json j);

=======
        void evacuateModeStart();
		void Shopinit();
>>>>>>> c4c79ab4b93521008725e96b31d4fc3a6baf00e3
    }

}	// namespace Menge
#endif	//__BaseScene_H__
