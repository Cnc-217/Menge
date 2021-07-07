/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *	@file		Bussiness.h
 *	@brief		
 */

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
