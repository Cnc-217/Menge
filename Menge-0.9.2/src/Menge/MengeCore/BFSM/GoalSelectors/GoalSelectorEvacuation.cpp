

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorEvacuation.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorRandom.h"
#include "MengeCore/resources/Graph.h"
#include "MengeCore/Core.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalAgent.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/BFSM/init.h"

#include <cassert>

namespace Menge {

	namespace BFSM {

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AlgorithmGoalSelector
		/////////////////////////////////////////////////////////////////////

		using namespace std;

		bool EvacuationGoalSelector::_flag;
		map <size_t, Goal*> EvacuationGoalSelector::_bestGoals;

		EvacuationGoalSelector::EvacuationGoalSelector() :GoalSelector(), _goalSetID(-1) {
			EvacuationGoalSelector::_flag = false;
			EvacuationGoalSelector::_bestGoals.clear();
			_agentGoalSet = new GoalSet();
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

		void EvacuationGoalSelector::setAgentGoalSet() {
			for (int idx = 0; idx < Menge::SIMULATOR->getNumAgents(); idx++) {
				Menge::Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(idx);
				AgentGoal* agentGoal = new AgentGoal();
				CircleShape* geo = new CircleShape(Vector2(agent->_pos._x, agent->_pos._y), 30);
				agentGoal->setGeometry(geo);
				agentGoal->setID(idx);
				agentGoal->_leaderID = idx;
				_agentGoalSet->addGoal(idx, agentGoal);
				//���ɺܶ�agentgoal��Ȼ��getgoal�滻������Ҫ�Ĵ���
			}
			ACTIVE_FSM->addGoalSet(_agentGoalSet);
			cout << "algorithmID: " << _algorithmID << endl;
		}

		Goal* EvacuationGoalSelector::getGoal(const Agents::BaseAgent* agent) const {
			vector<Olympic::roadRegionType>* roadRegionInfo = (ACTIVE_SCENE->roadRegionInfo);//�ֲ�����
			Agents::BaseAgent* agentTmp = (Agents::BaseAgent*)agent;
			//leader,���㷨ѡ����� 
			//if (agent->_class == 1) {
			if (true) {
				if (_flag == false) {
					//cout << "AlgorithmGoalSelector start, AlgorithmID is: " << _algorithmID << endl;
					//�ж�ϵͳ����
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

					//�洢ÿ��agent�����г��ڵ��ܾ��룬��СΪNUM_AGENT
					vector<float_t> distanceSum(NUM_AGENT, 0);
					//�洢ÿ��agent�����г��ڵ�·������������СΪNUM_AGENT
					vector<float_t> pathPeopleNum(NUM_AGENT, 0);
					//�洢ĳ��agent��ÿ�����ڵľ���
					//vector<float_t> distance(NUM_GOAL, 0);
					//�洢ÿ��agent�����г��ڵľ���vector
					vector<vector<float_t>> distanceVec(NUM_AGENT, vector<float_t>(NUM_GOAL + 1, 0));
					//�洢ÿ��agent�����г��ڵ�·�ϵ�����
					vector<vector<float_t>> peopleNumVec(NUM_AGENT, vector<float_t>(NUM_GOAL + 1, 0));
					//cout << "yes2" << endl;
					vector<float_t> temp(2, 0);
					for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
						Agents::BaseAgent* agentCur = Menge::SIMULATOR->getAgent(i);//��ǰagent
						Graph::graphLoad->countPeople(agentCur);

					}
					//cout << "һ����" << Graph::graphLoad->number.size() << "������" << "\n";
					for (int i = 0; i < Graph::graphLoad->number.size(); ++i) {
						//cout << "��" << i << "��vertex������Ϊ��" << Graph::graphLoad->number[i] << "\n";
					}
					//cout << "yes3" << endl;
					for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
						for (int j = 0; j < NUM_GOAL; j++) { //����ÿ������
							Agents::BaseAgent* agentTest = Menge::SIMULATOR->getAgent(i);//��ǰagent
							//cout << "A*�㷨ǰ���ɹ�" << endl;
							//��graph.h�������˾�̬��Ա������������A*�㷨
							vector<float_t> tmp = Graph::graphLoad->pathPeopleNumberAndLength(agentTest, _goalSet->getGoalByID(j));
							distanceVec[i][j] = tmp[0];
							distanceSum[i] += tmp[0];
							peopleNumVec[i][j] = tmp[1];
							pathPeopleNum[i] += tmp[1];
						}
						//cout << "���ǵ�" << i << "���˵����г���·�ϵ�������" << pathPeopleNum[i] << endl;
					}
					//cout << "aaaa  " << endl;
					Goal* bestGoal = _goalSet->getGoalByID(1);
					size_t populationExit = 0;
					float_t populationCapacityRatio = 0;
						
					populationExit += roadRegionInfo->at(1).peopleNumInThisRoad;//���г��ڵ�������
					populationExit += roadRegionInfo->at(20).peopleNumInThisRoad;//���г��ڵ�������
					populationExit += roadRegionInfo->at(21).peopleNumInThisRoad;//���г��ڵ�������
					populationCapacityRatio += (roadRegionInfo->at(1).peopleNumInThisRoad+ roadRegionInfo->at(20).peopleNumInThisRoad)
						/ (roadRegionInfo->at(1).capacity+ roadRegionInfo->at(20).capacity);//���г��ڵ����˿�������
					populationCapacityRatio += roadRegionInfo->at(21).peopleNumInThisRoad / roadRegionInfo->at(21).capacity;//���г��ڵ����˿�������
						
					//cout << "xxx wrong" << endl;
					//����������ȼ�
					float_t priorityMax;
					float_t priorityTmp;
					vector<int> ExitReagionInfo{ roadRegionInfo->at(20).peopleNumInThisRoad + roadRegionInfo->at(1).peopleNumInThisRoad,roadRegionInfo->at(21).peopleNumInThisRoad };
					vector<int> populationCapacity{ roadRegionInfo->at(20).capacity + roadRegionInfo->at(1).capacity,roadRegionInfo->at(21).capacity };
					for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
						priorityMax = 0; //���ȼ��㷨�õ������ȼ�Խ��Խ��
						priorityTmp = 0;
						for (int j = 0; j < NUM_GOAL; j++) {//����ÿ��goal,�������ȼ�
							if (_algorithmID == 0) {

								priorityTmp = 1 - distanceVec[i][j] / distanceSum[i];	
							}
							else if (_algorithmID == 1) {
								if (populationExit == 0) priorityTmp = 1 - distanceVec[i][j] / distanceSum[i];
								else priorityTmp = 0.7 * (1 - ExitReagionInfo[j] / populationExit) + 0.3 * (1 - distanceVec[i][j] / distanceSum[i]);
							}
							else if (_algorithmID == 2) {
								if (populationCapacityRatio == 0) {
									priorityTmp = 1 - distanceVec[i][j] / distanceSum[i];
									//cout << "��" << i << "��agent�ĵ�" << j << "�����ڵķ���Ϊ" << priorityTmp << endl;
								}
								else  {
									priorityTmp = 1 * (1 - (ExitReagionInfo[j] / populationCapacity[j]) / populationCapacityRatio) + 1 * (1 - distanceVec[i][j] / distanceSum[i]);
									//cout << "��" << i << "��agent�ĵ�" << j << "�����ڵķ���Ϊ" << priorityTmp << endl;
								}
							}
							else if (_algorithmID == 3) {
								if (populationCapacityRatio == 0 || pathPeopleNum[i] == 0) priorityTmp = 0.01 * (1 - distanceVec[i][j] / distanceSum[i]);
								else priorityTmp = 0.4 * (1 - (ExitReagionInfo[j] / populationCapacity[j]) / populationCapacityRatio) + 0 * (1 - distanceVec[i][j] / distanceSum[i]) + 0.3 * (1 - peopleNumVec[i][j] / pathPeopleNum[i]);
									
							}
							else {
								cout << "AlgorithmID wrong" << endl;
								return 0x0;
							}


							//cout << " priorityMax:  " << priorityMax << "  priorityTmp: " << priorityTmp << endl;
							if (priorityTmp > priorityMax) {
								//if (priorityMax != 0) cout << "��" << i << "��agent��������" << endl;
								priorityMax = priorityTmp;
								Menge::Olympic::AGENT_SCORES[i] = priorityMax;
								bestGoal = _goalSet->getGoalByID(j);
							}
						}
						_bestGoals.insert(pair <size_t, Goal*>(i, bestGoal));
					}

					_flag = true;

				}
				return _bestGoals.find(agent->_id)->second;
			}
			
			//panic�����������leader�͸���leader������û��leader�ͼ��ԭ����ѡ��
			else if (agent->_class == 2) {
				if (agent->_leadersID != -1) {
					Agents::BaseAgent* follower = Menge::SIMULATOR->getAgent(agent->_leadersID);
					if (follower->_class == 1) return _agentGoalSet->getGoalByID(agent->_leadersID);
				}
				float_t personal = (Menge::SIM_TIME - Menge::Olympic::startSimTime)/100;

				//�õ�������agent
				std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
				vector<Menge::Agents::NearAgent>::iterator nearAgent;
				for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //����������agent
					//cout << "here"<<Menge::Olympic::AGENT_SCORES[(*nearAgent).agent->_id] << endl;
					if ((*nearAgent).agent->_class == 1 && Menge::Olympic::AGENT_SCORES[(*nearAgent).agent->_id] > personal) {
						//������leader����leader��Ӱ�����ϴ�
						agentTmp->_leadersID = (*nearAgent).agent->_id;
						return _agentGoalSet->getGoalByID((*nearAgent).agent->_id);
					}
				}

				//����û��leader���жϴ�ʱ�ǲ��ǵ�һ��ѡ��
				//������ǵ�һ��ѡ��ֱ�ӷ���ԭ���ĳ���
				if (Menge::Olympic::AGENT_GOALS[agent->_id] != -1) return _goalSet->getGoalByID(Menge::Olympic::AGENT_GOALS[agent->_id]);

				//����ǵ�һ��ѡ�����ѡ��һ���³���
				size_t NUM_GOAL = _goalSet->size();
				//���������
				LARGE_INTEGER seed;
				QueryPerformanceFrequency(&seed);
				QueryPerformanceCounter(&seed);
				srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
				size_t idx = rand() % NUM_GOAL;
				Menge::Olympic::AGENT_GOALS[agent->_id] = idx;
				return _goalSet->getGoalByID(idx);

			}

			//normal�����������panic�͸���panic�����������leader�͸���leader
			//�����û�о��ۺ��˶�ĵ�·���Լ��������ܶ���ȥѡ��
			else if (agent->_class == 3) {
				if (agent->_leadersID != -1) {
					Agents::BaseAgent* follower=Menge::SIMULATOR->getAgent(agent->_leadersID);
					if (follower->_class == 1) return _agentGoalSet->getGoalByID(agent->_leadersID);
				}
				float_t personal = (Menge::SIM_TIME - Menge::Olympic::startSimTime) / 100;

				//������leader
				std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
				vector<Menge::Agents::NearAgent>::iterator nearAgent;
				int agentChoose = -1;
				for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) {
					if ((*nearAgent).agent->_class == 1 && Menge::Olympic::AGENT_SCORES[(*nearAgent).agent->_id] > personal) {
						//������leader����leader��Ӱ�����ϴ�
						agentTmp->_leadersID = (*nearAgent).agent->_id;
						return _agentGoalSet->getGoalByID((*nearAgent).agent->_id);

					}
					else if ((*nearAgent).agent->_class == 2 && Menge::Olympic::AGENT_SCORES[(*nearAgent).agent->_id] > personal) {
						//������panic����panic��Ӱ�����ϴ�
						//�ݶ���ǰagent��goalΪ��panic��goal
						agentChoose = (*nearAgent).agent->_id;
					}
				}

				if (agent->_leadersID != -1) {
					Agents::BaseAgent* follower = Menge::SIMULATOR->getAgent(agent->_leadersID);
					if (follower->_class == 2) return _agentGoalSet->getGoalByID(agent->_leadersID);
				}

				//ȷ����leader�������panic�ͷ���panic
				if (agentChoose >= 0) {
					//EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
					//return selector->_goalSet->getGoalByID(agentChoose);
					agentTmp->_leadersID = agentChoose;
					return _agentGoalSet->getGoalByID(agentChoose);
				}
				else {//���û��panic���ۺ��˶�ĵ�·���Լ��������ܶ���ȥѡ��
					size_t NUM_GOAL = _goalSet->size();
					//��ǰagent�����г��ڵ�·�ϵ�������
					size_t totalNum = _goalSet->size();
					//��ǰagent��ÿ�����ڵ�·��������
					vector<float_t> pathPeopleNum(NUM_GOAL, 0);
					for (int i = 0; i < NUM_GOAL; i++) { //����ÿ������
						//cout << "A*�㷨ǰ���ɹ�" << endl;
						//��graph.h�������˾�̬��Ա������������A*�㷨
						vector<float_t> tmp = Graph::graphLoad->pathPeopleNumberAndLength(agent, _goalSet->getGoalByID(i));
						pathPeopleNum[i] = tmp[1];
						totalNum += tmp[1];
					}
					for (int i = 0; i < NUM_GOAL; i++) { //����ÿ������
						if (1 - pathPeopleNum[i] / totalNum > personal) {
							Menge::Olympic::AGENT_GOALS[agent->_id] = i;
							return _goalSet->getGoalByID(i);
						}
					}
					return _goalSet->getGoalByID(Menge::Olympic::AGENT_GOALS[agent->_id]);
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
