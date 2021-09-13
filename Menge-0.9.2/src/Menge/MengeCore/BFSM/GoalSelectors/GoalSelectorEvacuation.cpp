

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorEvacuation.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorRandom.h"
#include "MengeCore/resources/Graph.h"
#include "MengeCore/Core.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/FSM.h"
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
		using Menge::BaseScene::ExitReagionInfo;
		using Menge::BaseScene::ExitAgentInfo;
		using Menge::BaseScene::ExitReagionCapacity;

		bool EvacuationGoalSelector::_flag;
		map <size_t, Goal*> EvacuationGoalSelector::_bestGoals;

		EvacuationGoalSelector::EvacuationGoalSelector() :GoalSelector(), _goalSetID(-1) {
			EvacuationGoalSelector::_flag = false;
			EvacuationGoalSelector::_bestGoals.clear();
		}

		/////////////////////////////////////////////////////////////////////

		void EvacuationGoalSelector::setGoalSet(std::map< size_t, GoalSet* >& goalSets) {
			if (goalSets.count(_goalSetID) == 1) {
				_goalSet = goalSets[_goalSetID];
			}
			else {
				logger << Logger::ERR_MSG << "Error selecting goal set ";
				logger << _goalSetID << " from those defined.  It does not exist.";
				throw GoalSelectorException();
			}
		}

		Goal* EvacuationGoalSelector::getGoal(const Agents::BaseAgent* agent) const {

			if (Menge::PROJECTNAME == THEMEPARK || Menge::PROJECTNAME == OLYMPIC) {
				//leader,用算法选择出口 
				if (agent->_class == 1) {
					if (_flag == false) {
						cout << "AlgorithmGoalSelector start, AlgorithmID is: " << _algorithmID << endl;
						//判断系统错误
						assert(agent != 0x0 && "AlgorithmGoalSelector requires a valid base agent!");
						const size_t GOAL_COUNT = _goalSet->size();
						if (GOAL_COUNT == 0) {
							logger << Logger::ERR_MSG;
							logger << "AlgorithmGoalSelector was unable to provide a goal for agent ";
							logger << agent->_id << ".  There were no available goals in the goal set.";
							return 0x0;
						}

						EvacuationGoalSelector::_bestGoals.clear();
						const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
						const size_t NUM_GOAL = _goalSet->size();
						float_t distance = 0;
						//存储每个agent到所有出口的总距离，大小为NUM_AGENT
						vector<float_t> distanceSum(NUM_AGENT, 0);
						//存储某个agent到每个出口的距离
						//vector<float_t> distance(NUM_GOAL, 0);
						//存储每个agent到所有出口的距离vector
						vector<vector<float_t>> distanceVec(NUM_AGENT, vector<float_t>(NUM_GOAL + 1, 0));

						for (int i = 0; i < NUM_AGENT; i++) { //遍历每个agent
							distance = 0;
							for (int j = 0; j < NUM_GOAL; j++) { //遍历每个出口
								Agents::BaseAgent* agentTest = Menge::SIMULATOR->getAgent(i);//当前agent
								//在graph.h中增加了静态成员变量，下面是A*算法
								distance = Graph::graphLoad->getPathLenth(agentTest, _goalSet->getGoalByID(j));//得到路径长度
								distanceVec[i][j] = distance;
								distanceSum[i] += distance;
							}
						}

						Goal* bestGoal = _goalSet->getGoalByID(1);
						size_t populationExit = 0;
						float_t populationCapacity = 0;
						for (size_t i = 0; i < ExitReagionInfo.size(); i++)
						{
							populationExit += ExitReagionInfo[i];//所有出口的总人数
							populationCapacity += ExitReagionInfo[i] / ExitReagionCapacity[i];//所有出口的总容量
						}

						//下面计算优先级
						float_t priorityMax;
						float_t priorityTmp;
						for (int i = 0; i < NUM_AGENT; i++) { //遍历每个agent
							priorityMax = 0; //优先级算法得到的优先级越大越好
							priorityTmp = 0;
							for (int j = 0; j < NUM_GOAL; j++) {//遍历每个goal,计算优先级

								if (_algorithmID == 0) {

									priorityTmp = 1 / distanceVec[i][j];
								}
								else if (_algorithmID == 1) {
									if (populationExit == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
									else priorityTmp = 3 * (1 - ExitReagionInfo[j] / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]);
								}
								else if (_algorithmID == 2) {
									if (populationExit == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
									else priorityTmp = 3 * (1 - (ExitReagionInfo[j] / populationCapacity) / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]);
								}
								else {
									cout << "AlgorithmID wrong" << endl;
									return 0x0;
								}


								if (priorityTmp > priorityMax) {
									priorityMax = priorityTmp;
									bestGoal = _goalSet->getGoalByID(j);
								}
							}
							_bestGoals.insert(pair <size_t, Goal*>(i, bestGoal));
						}

						_flag = true;

					}
					return _bestGoals.find(agent->_id)->second;
				}

				//panicer，如果附近有leader就跟随leader，附近没有leader就随机选择出口
				else if (agent->_class == 2){
					//附近有leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //遍历附近的agent
						if ((*nearAgent).agent->_class == 1) { //有leader，将对应的agentGoal返回
							//强制转换到goalset为agent的那个selector，借用他的goalset
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							return selector->_goalSet->getGoalByID((*nearAgent).agent->_id);
						}
					}

					//附近没有leader，随机选一个出口
					size_t NUM_GOAL = _goalSet->size();
					//随机数生成
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					size_t idx = rand()% NUM_GOAL;
					return _goalSet->getGoalByID(idx);
				
				}
				
				//normal，如果附近有panic就跟随panic，如果附近有leader就跟随leader，附近没有就随机选择出口
				else if (agent->_class == 3){
					//附近有leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					int agentChoose = -1;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //遍历附近的agent
						if ((*nearAgent).agent->_class == 1) { //有leader，将对应的agentGoal返回
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							return selector->_goalSet->getGoalByID((*nearAgent).agent->_id);
						}
						else if ((*nearAgent).agent->_class == 2) {//有panic，需要查看是否还有leader
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							agentChoose = (*nearAgent).agent->_id;
						}
					}
					//确认无leader，返回panic
					if (agentChoose >= 0) {
						EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
						return selector->_goalSet->getGoalByID(agentChoose);
					}
					else {
						//随机选择出口
						size_t NUM_GOAL = _goalSet->size();
						//随机数生成
						LARGE_INTEGER seed;
						QueryPerformanceFrequency(&seed);
						QueryPerformanceCounter(&seed);
						srand(seed.QuadPart);//取硬件计时器，精度更高
						size_t idx = rand() % NUM_GOAL;
						return _goalSet->getGoalByID(idx);
					}
					
				}
			}

			

		}

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AlgorithmGoalSelectorFactory
		/////////////////////////////////////////////////////////////////////

		EvacuationGoalSelectorFactory::EvacuationGoalSelectorFactory() : GoalSelectorFactory() {
			_goalSetID = _attrSet.addSizeTAttribute("goal_set", true /*required*/);
			_algorithmID = _attrSet.addSizeTAttribute("algorithmID", true /*required*/);
		}

		/////////////////////////////////////////////////////////////////////

		bool EvacuationGoalSelectorFactory::setFromXML(GoalSelector* selector, TiXmlElement* node,
			const std::string& behaveFldr) const {
			EvacuationGoalSelector* sgs = dynamic_cast<EvacuationGoalSelector*>(selector);
			assert(sgs != 0x0 &&
				"Trying to set goal set goal selector attributes on an incompatible object.");

			if (!GoalSelectorFactory::setFromXML(sgs, node, behaveFldr)) return false;

			sgs->setGoalSetID(_attrSet.getSizeT(_goalSetID));
			sgs->setAlgorithmID(_attrSet.getSizeT(_algorithmID));

			return true;
		}
		

		

	}	// namespace BFSM
 
}	// namespace Menge
