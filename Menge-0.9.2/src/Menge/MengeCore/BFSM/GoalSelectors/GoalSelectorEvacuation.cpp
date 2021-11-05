

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
				//leader,���㷨ѡ����� 
				if (agent->_class == 1) {
					if (_flag == false) {
						cout << "AlgorithmGoalSelector start, AlgorithmID is: " << _algorithmID << endl;
						//�ж�ϵͳ����
						assert(agent != 0x0 && "AlgorithmGoalSelector requires a valid base agent!");
						const size_t GOAL_COUNT = _goalSet->size();
						if (GOAL_COUNT == 0) {
							logger << Logger::ERR_MSG;
							logger << "AlgorithmGoalSelector was unable to provide a goal for agent ";
							logger << agent->_id << ".  There were no available goals in the goal set.";
							return 0x0;
						}
						cout << "yes" << endl;
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
						cout << "yes2" << endl;
						vector<float_t> temp(2, 0);
						for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
							Agents::BaseAgent* agentCur = Menge::SIMULATOR->getAgent(i);//��ǰagent
							Graph::graphLoad->countPeople(agentCur);

						}
						cout << "һ����" << Graph::graphLoad->number.size() << "������" << "\n";
                        for (int i = 0; i < Graph::graphLoad->number.size(); ++i) {
                            cout << "��" << i << "��vertex������Ϊ��" << Graph::graphLoad->number[i] << "\n";
                        }
						cout << "yes3" << endl;
						for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
							for (int j = 0; j < NUM_GOAL; j++) { //����ÿ������
								Agents::BaseAgent* agentTest = Menge::SIMULATOR->getAgent(i);//��ǰagent
								cout << "A*�㷨ǰ���ɹ�" << endl;
								//��graph.h�������˾�̬��Ա������������A*�㷨
                                vector<float_t> tmp = Graph::graphLoad->pathPeopleNumberAndLength(agentTest, _goalSet->getGoalByID(j));
								distanceVec[i][j] = tmp[0];
								distanceSum[i] += tmp[0];
								peopleNumVec[i][j] = tmp[1];
								pathPeopleNum[i] += tmp[1];
							}
							cout << "���ǵ�" << i << "���˵����г���·�ϵ�������" << pathPeopleNum[i] << endl;
						}
						cout << "aaaa  " << endl;
						Goal* bestGoal = _goalSet->getGoalByID(1);
						size_t populationExit = 0;
						float_t populationCapacity = 0;
						for (size_t i = 0; i < ExitReagionInfo.size(); i++)
						{
							populationExit += ExitReagionInfo[i];//���г��ڵ�������
							if (i > 0)populationCapacity += ExitReagionInfo[i] / ExitReagionCapacity[i];//���г��ڵ�������
						}
						cout << "xxx wrong" << endl;
						//����������ȼ�
						float_t priorityMax;
						float_t priorityTmp;
						for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
							priorityMax = 0; //���ȼ��㷨�õ������ȼ�Խ��Խ��
							priorityTmp = 0;
							for (int j = 0; j < NUM_GOAL; j++) {//����ÿ��goal,�������ȼ�
								
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
								else if (_algorithmID == 3) {
									if (populationExit == 0 || pathPeopleNum[i] == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
									else priorityTmp = 3 * (1 - (ExitReagionInfo[j] / populationCapacity) / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]) + (1 - peopleNumVec[i][j] / pathPeopleNum[i]);
								}
								else {
									cout << "AlgorithmID wrong" << endl;
									return 0x0;
								}
								
							
								cout <<" priorityMax:  "<< priorityMax << "  priorityTmp: " << priorityTmp << endl;
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

				//panicer�����������leader�͸���leader������û��leader�����ѡ�����
				else if (agent->_class == 2){
					//������leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //����������agent
						if ((*nearAgent).agent->_class == 1) { //��leader������Ӧ��agentGoal����
							//ǿ��ת����goalsetΪagent���Ǹ�selector����������goalset
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							return selector->_goalSet->getGoalByID((*nearAgent).agent->_id);
						}
					}

					//����û��leader�����ѡһ������
					size_t NUM_GOAL = _goalSet->size();
					//���������
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
					size_t idx = rand()% NUM_GOAL;
					return _goalSet->getGoalByID(idx);
				
				}
				
				//normal�����������panic�͸���panic�����������leader�͸���leader������û�о����ѡ�����
				else if (agent->_class == 3){
					//������leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					int agentChoose = -1;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //����������agent
						if ((*nearAgent).agent->_class == 1) { //��leader������Ӧ��agentGoal����
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							return selector->_goalSet->getGoalByID((*nearAgent).agent->_id);
						}
						else if ((*nearAgent).agent->_class == 2) {//��panic����Ҫ�鿴�Ƿ���leader
							EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
							agentChoose = (*nearAgent).agent->_id;
						}
					}
					//ȷ����leader������panic
					if (agentChoose >= 0) {
						EvacuationGoalSelector* selector = (EvacuationGoalSelector*)Menge::ACTIVE_FSM->getNode("Agent")->getGoalSelector();
						return selector->_goalSet->getGoalByID(agentChoose);
					}
					else {
						//���ѡ�����
						size_t NUM_GOAL = _goalSet->size();
						//���������
						LARGE_INTEGER seed;
						QueryPerformanceFrequency(&seed);
						QueryPerformanceCounter(&seed);
						srand(seed.QuadPart);//ȡӲ����ʱ�������ȸ���
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
