#ifndef __Socket_H__
#define	__Socket_H__
#include <string>
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")


/*!
 * @namespace Menge
 * @brief	Socket
 */
using namespace std;

namespace Menge {

    namespace Socket {

        SOCKET socketClientInit(char* ip,int host);

        SOCKET socketServerInit(char* ip, int host);

        void socketSend(const char* str, SOCKET socket);

        string socketListen(SOCKET socket);

    }
}	// namespace Menge



#endif	//__Socket_H__
