
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
            //�ȴ��ͻ��˵�����
            serConn = accept(socketServer, (SOCKADDR*)&clientsocket, &len);
            char receiveBuf[100];
            //���տͻ��˴�������Ϣ
            int lenBuf;
            lenBuf = recv(serConn, receiveBuf, 100, 0);
            receiveBuf[lenBuf] = '\0';
            cout << "socketServer receive command: "<< receiveBuf << endl;
            //����ͻ��˴�����Evacuate�������״̬ת�Ƶĺ���
            if (!strcmp(receiveBuf, "Evacuate")) {
                cout << "Evacuate mode start" << endl;
                MengeVis::SimViewer->_pause = true;//false����������
                //��ͣ�������Ǹ��ݲ�ͬ��Ŀ�Ķ��ƻ����벿��
                if(Menge::PROJECTNAME==THEMEPARK) ThemePark::evacuateModeStart();
                MengeVis::SimViewer->_pause = false;
            }

            char sendBuf[100] = "Menge has receive your commend";
            //sprintf(sendBuf, "server : welcome %s to server.", inet_ntoa(clientsocket.sin_addr));
            //�ڶ�Ӧ��IP�����ҽ������ִ�ӡ������
            send(serConn, sendBuf, strlen(sendBuf) + 1, 0);
        }
	}

}




