

#ifndef __Socket_H__
#define	__Socket_H__
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <string>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
/*!
 * @namespace Menge
 * @brief	Socket
 */
namespace Menge {
    namespace Socket {


        SOCKET socketClientInit(char* ip,int host);

        SOCKET socketServerInit(char* ip, int host);

        //Business的socket交互
        int* socketGetCouponBusiness(char* message);

        //BusinessReality的socket交互
        int* socketGetCouponBusinessReality(char* message);

		int getIpinfo();
		int getIp();
		int getIp2();

    }
}	// namespace Menge



#endif	//__Socket_H__
