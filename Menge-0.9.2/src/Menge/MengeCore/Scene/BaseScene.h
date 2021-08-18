
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

    }

    namespace ThemePark {
        void evacuateModeStart();
    }

}	// namespace Menge
#endif	//__BaseScene_H__
