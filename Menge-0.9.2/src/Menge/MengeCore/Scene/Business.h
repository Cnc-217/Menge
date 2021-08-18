
#ifndef __Business_H__
#define	__Business_H__


#include <string>
#include "MengeCore/MatrixMy.h"
/*!
 * @namespace Menge
 * @brief	Bussinss
 */
namespace Menge {
    namespace Business {
        char* dataInit();

        void fsmToMartix();

        void MartixToFsm();

    }


    namespace BusinessReality {

        char* dataInit();
        
        void martixInit();

    }


}	// namespace Menge



#endif	//__Business_H__
