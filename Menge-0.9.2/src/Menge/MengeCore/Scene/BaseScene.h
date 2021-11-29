
#ifndef __BaseScene_H__
#define	__BaseScene_H__

#include "MengeCore/MatrixMy.h"
#include "MengeCore/Json/json.hpp"
#include "tinyxml/tinyxml.h"
#include <string>
#include <winsock.h>



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
        vector<bool> strToVectorBool (char* listStr);
        vector<float> strToVectorFloat (char* listStr);
        void projectNameExtract(string folderPath);//��ȡ��Ŀ��
        void sceneParallelXML(string senceXmlFliePath);//��¼��Ⱥλ�ã�дһ���µ�xml�ļ�
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
