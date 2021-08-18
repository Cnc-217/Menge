
#include "MengeCore/Scene/BaseScene.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/Events/EventSystem.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#include "MengeVis/Viewer/GLViewer.h"


using namespace std;
using namespace Menge::BFSM;


namespace Menge {
	/////////////////////////////////////////////////////////////////////
	//					Implementation of BaseScene
	/////////////////////////////////////////////////////////////////////

	void BaseScene::sockerServerListen(SOCKET socketServer) {
		SOCKADDR_IN clientsocket;
		int len = sizeof(SOCKADDR);
		SOCKET serConn;
		
        while (1) {
            //等待客户端的连接
            serConn = accept(socketServer, (SOCKADDR*)&clientsocket, &len);
            char receiveBuf[100];
            //接收客户端传来的信息
            int lenBuf;
            lenBuf = recv(serConn, receiveBuf, 100, 0);
            receiveBuf[lenBuf] = '\0';
            cout << "socketServer receive command: "<< receiveBuf << endl;
            //如果客户端传来了Evacuate，则进入状态转移的函数
            if (!strcmp(receiveBuf, "Evacuate")) {
                cout << "Evacuate mode start" << endl;
                MengeVis::SimViewer->_pause = true;//false是正常运行
                //暂停，下面是根据不同项目的定制化代码部分
                if(Menge::PROJECTNAME==THEMEPARK) ThemePark::evacuateModeStart();
                MengeVis::SimViewer->_pause = false;
            }

            char sendBuf[100] = "Menge has receive your commend";
            //sprintf(sendBuf, "server : welcome %s to server.", inet_ntoa(clientsocket.sin_addr));
            //在对应的IP处并且将这行字打印到那里
            send(serConn, sendBuf, strlen(sendBuf) + 1, 0);
        }
	}

}




