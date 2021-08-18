
#include "MengeCore/Scene/ThemePark.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/GoalAgent.h"
#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/Agents/BaseAgent.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"


using namespace std;
using namespace Menge::BFSM;


namespace Menge {
	/////////////////////////////////////////////////////////////////////
	//					Implementation of ThemePark
	/////////////////////////////////////////////////////////////////////
	void ThemePark::evacuateModeStart() {
		//0.Ԥ�ȶ���������ߵ�agentgoalset�����ֻ��ߵ�goalset�޷���ǰ����ã���������ǰ������˶����goal��idƥ�䣬Ȼ��Ϊgoalset�������agentgoal,Ŀǰֻ���ֶ����
		//1.����agent��� 8:2 ��ͨ���ֻ� ����������
		//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
		//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
		//4.agentת�Ƶ���state
		//5.��дevent neighborhoodtrigger�б�дthemepark��agent�����߼�������������ض���agent�������½���evacuation״̬��ѡ��Ŀ��
		
		//1.����agent��ݣ���������
		int numAgent = SIMULATOR->getNumAgents();
		float normalPercentage = 0.8;
		for (int i = 0; i < numAgent; i++) {
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			//�ο�
			if (agent->_class == 0) {
				//���������
				LARGE_INTEGER seed;
				QueryPerformanceFrequency(&seed);
				QueryPerformanceCounter(&seed);
				srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
				float random = ((rand() % 100) * 0.01);
				//normalAgent�ı���
				if (random <= normalPercentage) {
					agent->_class = 3;//normal
					normalAgentSet.push_back(agent);
				}
				else {
					agent->_class = 2;//panic
					panicAgentSet.push_back(agent);
				}
			}
			//leader
			else if (agent->_class == 1) {
				leaderAgentSet.push_back(agent);
			}
			

			
			

			
			
		}


		//2.��д�ض���goalselctor�����������ߣ�ʹ���㷨���������ڣ�������ͨ�˺Ϳֻ��ߣ�ʹ��near_agent����������
		//3.action�У����ó���λ�ã�agent�ִ���������Զ�ת����stop״̬
		//4.agentת�Ƶ���state
		for (int i = 0; i < numAgent; i++) {
			cout << i << endl;
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
			currentState->leave(agent);
			State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
			nextState->enter(agent);
			Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
		}

		//5.��дevent,event��Ҫ����״̬��
		evacuationState = true;


	}
	
}




