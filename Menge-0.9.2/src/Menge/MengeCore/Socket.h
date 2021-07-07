

#ifndef __Socket_H__
#define	__Socket_H__
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>

/*!
 * @namespace Menge
 * @brief	Socket
 */
namespace Menge {
    namespace Socket {


        SOCKET socketInit(char* ip,int host);

        //Business的socket交互
        int* socketGetCouponBusiness(char* message);

        //BusinessReality的socket交互
        int* socketGetCouponBusinessReality(char* message);


    }
}	// namespace Menge



#endif	//__Socket_H__
