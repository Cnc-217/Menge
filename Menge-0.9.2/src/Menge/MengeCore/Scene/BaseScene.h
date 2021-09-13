
#ifndef __BaseScene_H__
#define	__BaseScene_H__


#include <string>
#include <winsock.h>
#include "MengeCore/MatrixMy.h"
#pragma comment(lib, "ws2_32.lib")
/*!
 * @namespace Menge
 * @brief	BaseScene
 */
namespace Menge {
    namespace BaseScene {
        
        void sockerServerListen(SOCKET socketServer);
        void loadMatrixFromTxt(const char* fileName);
        void modifyMatrix(char* matrixStr);
    }

    namespace ThemePark {
        void evacuateModeStart();
    }


    namespace Olympic {
        void evacuateModeStart();
    }

}	// namespace Menge
#endif	//__BaseScene_H__
