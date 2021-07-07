

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorAlgorithm.h"
#include "MengeCore/resources/Graph.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#include <cassert>

namespace Menge {

	namespace BFSM {

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AlgorithmGoalSelector
		/////////////////////////////////////////////////////////////////////

		using namespace std;

		bool AlgorithmGoalSelector::_flag;
		map <size_t, Goal*> AlgorithmGoalSelector::_bestGoals;

		AlgorithmGoalSelector::AlgorithmGoalSelector() {
			AlgorithmGoalSelector::_flag = false;
			AlgorithmGoalSelector::_bestGoals.clear();
		}

		Goal* AlgorithmGoalSelector::getGoal(const Agents::BaseAgent* agent) const {
			if (_flag == false) {
				//判断系统错误
				assert(agent != 0x0 && "AlgorithmGoalSelector requires a valid base agent!");
				const size_t GOAL_COUNT = _goalSet->size();
				if (GOAL_COUNT == 0) {
					logger << Logger::ERR_MSG;
					logger << "AlgorithmGoalSelector was unable to provide a goal for agent ";
					logger << agent->_id << ".  There were no available goals in the goal set.";
					return 0x0;
				}

				const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
				const size_t NUM_GOAL = _goalSet->size();
				size_t agent_id;
				float_t speed;
				float_t distance;
				float_t time_predict;
				size_t i = 0;
				size_t j = 0;
				//存储每个goal的所有agent-time map，大小为NUM_GOAL key=goalID value=map<agentID,time>
				map<size_t, map<size_t, float_t>> map_goal;
				//存储每个agent到i出口的time，大小为NUM_AGENT key=agentID value=time
				map<size_t, float_t> map_agent;

				for (i = 0; i < NUM_GOAL; i++) { //遍历每个出口
					j = 0;
					for (j = 0; j < NUM_AGENT; j++) { //遍历每个agent
						Agents::BaseAgent* agentTest = Menge::SIMULATOR->getAgent(j);//当前agent
						speed = agentTest->_prefSpeed;//初始速度
						//在graph.h中增加了静态成员变量，下面是A*算法
						distance = Graph::graphLoad->getPathLenth(agentTest, _goalSet->getIthGoal(i));//得到路径长度
						time_predict = distance / speed + 50;//这里是公式的实现，后面的吞吐量暂时不知道怎么设置
						//存agent到i出口的time，不考虑顺序
						map_agent.insert(pair < size_t, float_t >(j, time_predict));
					}
					map_goal.insert(pair < size_t, map<size_t, float_t> >(i, map_agent));
					map_agent.clear();
				}

 				Goal* bestGoal = _goalSet->getIthGoal(0);

				map <size_t, map<size_t, float_t>> ::iterator itGoal;
				map < size_t, float_t > ::iterator itAgent;
				map_agent = map_goal.begin()->second; //取出第一个goal的map
				itAgent = map_agent.begin();

				//找第一个map_agent对里面的元素进行逐个查找
				while (itAgent != map_agent.end()) {  //遍历每个agent
					agent_id = itAgent->first;
					time_predict = itAgent->second;
					itGoal = map_goal.begin();
					itGoal++;
					bestGoal = _goalSet->getIthGoal(0);
					while (itGoal != map_goal.end()) {  //遍历每个goal
						if (itGoal->second.find(agent_id)->second <= time_predict) {//找到时间更少的出口了
							time_predict = itGoal->second.find(agent_id)->second;
							bestGoal = _goalSet->getIthGoal(itGoal->first);
						}
						itGoal++;
					}
					_bestGoals.insert(pair <size_t, Goal*>(agent_id, bestGoal));
					itAgent++;
				}
				_flag=true;
			}
			
			return _bestGoals.find(agent->_id)->second;
		}


	}	// namespace BFSM
 
}	// namespace Menge
