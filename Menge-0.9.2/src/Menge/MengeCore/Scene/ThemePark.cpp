
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
		//0.预先定义好引导者的agentgoalset，但恐慌者的goalset无法提前定义好，解决方案是把所有人都变成goal，id匹配，然后为goalset代码添加agentgoal,目前只能手动添加
		//1.分配agent身份 8:2 普通：恐慌 ，存入数组
		//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
		//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
		//4.agent转移到新state
		//5.编写event neighborhoodtrigger中编写themepark的agent跟随逻辑，如果附近有特定的agent，就重新进入evacuation状态，选择目标
		
		//1.分配agent身份，存入数组
		int numAgent = SIMULATOR->getNumAgents();
		float normalPercentage = 0.8;
		for (int i = 0; i < numAgent; i++) {
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			//游客
			if (agent->_class == 0) {
				//随机数生成
				LARGE_INTEGER seed;
				QueryPerformanceFrequency(&seed);
				QueryPerformanceCounter(&seed);
				srand(seed.QuadPart);//取硬件计时器，精度更高
				float random = ((rand() % 100) * 0.01);
				//normalAgent的比例
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


		//2.编写特定的goalselctor，对于引导者，使用算法来决定出口，对于普通人和恐慌者，使用near_agent来决定跟随
		//3.action中，定好出口位置，agent抵达出口区域，自动转换到stop状态
		//4.agent转移到新state
		for (int i = 0; i < numAgent; i++) {
			cout << i << endl;
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
			currentState->leave(agent);
			State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
			nextState->enter(agent);
			Menge::ACTIVE_FSM->setCurrentState((agent), nextState->getID());
		}

		//5.编写event,event需要启动状态码
		evacuationState = true;


	}
	
}




