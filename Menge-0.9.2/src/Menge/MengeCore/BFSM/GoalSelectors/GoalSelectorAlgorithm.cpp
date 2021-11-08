

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorAlgorithm.h"
#include "MengeCore/resources/Graph.h"
#include "MengeCore/Core.h"

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
		using Menge::BaseScene::ExitReagionInfo;
		using Menge::BaseScene::ExitAgentInfo;
		using Menge::BaseScene::ExitReagionCapacity;

		bool AlgorithmGoalSelector::_flag;
		map <size_t, Goal*> AlgorithmGoalSelector::_bestGoals;

		AlgorithmGoalSelector::AlgorithmGoalSelector() :GoalSelector(), _goalSetID(-1) {
			AlgorithmGoalSelector::_flag = false;
			AlgorithmGoalSelector::_bestGoals.clear();
		}

		/////////////////////////////////////////////////////////////////////

		void AlgorithmGoalSelector::setGoalSet(std::map< size_t, GoalSet* >& goalSets) {
			if (goalSets.count(_goalSetID) == 1) {
				_goalSet = goalSets[_goalSetID];
			}
			else {
				logger << Logger::ERR_MSG << "Error selecting goal set ";
				logger << _goalSetID << " from those defined.  It does not exist.";
				throw GoalSelectorException();
			}
		}

		Goal* AlgorithmGoalSelector::getGoal(const Agents::BaseAgent* agent) const {
			if (_flag == false) {
				cout << "AlgorithmGoalSelector start, AlgorithmID is: " << _algorithmID << endl;
				//�ж�ϵͳ����
				//assert(agent != 0x0 && "AlgorithmGoalSelector requires a valid base agent!");
				const size_t GOAL_COUNT = _goalSet->size();
				if (GOAL_COUNT == 0) {
					logger << Logger::ERR_MSG;
					logger << "AlgorithmGoalSelector was unable to provide a goal for agent ";
					logger << agent->_id << ".  There were no available goals in the goal set.";
					return 0x0;
				}
				cout << "yes" << endl;
				AlgorithmGoalSelector::_bestGoals.clear();
				const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
				const size_t NUM_GOAL = _goalSet->size();
                vector<vector<float_t>> distanceAndNum;
				float_t distance = 0;
				//�洢ÿ��agent�����г��ڵ��ܾ��룬��СΪNUM_AGENT
				vector<float_t> distanceSum(NUM_AGENT, 0);
                //�洢ÿ��agent�����г��ڵ�·������������СΪNUM_AGENT
                vector<float_t> pathPeopleNum(NUM_AGENT, 0);
				//�洢ĳ��agent��ÿ�����ڵľ���
				//vector<float_t> distance(NUM_GOAL, 0);
				//�洢ÿ��agent�����г��ڵľ���vector
				vector<vector<float_t>> distanceVec(NUM_AGENT, vector<float_t>(NUM_GOAL+1,0));
                //�洢ÿ��agent�����г��ڵ�·�ϵ�����
                vector<vector<float_t>> peopleNumVec(NUM_AGENT, vector<float_t>(NUM_GOAL+1,0));
				cout << "yes2" << endl;
                vector<float_t> temp(2, 0);
				for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
					Agents::BaseAgent* agentCur = Menge::SIMULATOR->getAgent(i);//��ǰagent
					Graph::graphLoad->countPeople(agentCur);
					
				}
				cout << "yes3" << endl;
				for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
					distance = 0;
					for (int j = 0; j < NUM_GOAL; j++) { //����ÿ������
						Agents::BaseAgent* agentTest = Menge::SIMULATOR->getAgent(i);//��ǰagent
						//��graph.h�������˾�̬��Ա������������A*�㷨
						distanceAndNum[i] = Graph::graphLoad->pathPeopleNumberAndLength(agentTest, _goalSet->getGoalByID(j));
						distance = Graph::graphLoad->getPathLenth(agentTest, _goalSet->getGoalByID(j));//�õ�·������
						distanceVec[i][j] = distanceAndNum[i][0];
						distanceSum[i] += distanceAndNum[i][0];
						peopleNumVec[i][j] = distanceAndNum[i][1];
						pathPeopleNum[i] += distanceAndNum[i][1];
					}
					cout <<"���ǵ�i����  "<< pathPeopleNum[i] << endl;
				}
				cout << "aaaa  "  << endl;
				Goal* bestGoal = _goalSet->getGoalByID(1);
				size_t populationExit = 0;
				float_t populationCapacity = 0;
				for (size_t i = 0; i < ExitReagionInfo.size(); i++)
				{
					populationExit += ExitReagionInfo[i];//���г��ڵ�������
					if(i>0)populationCapacity += ExitReagionInfo[i]/ ExitReagionCapacity[i];//���г��ڵ�������
				}
				cout << "xxx wrong" << endl;
				//����������ȼ�
				float_t priorityMax;
				float_t priorityTmp;
				for (int i = 0; i < NUM_AGENT; i++) { //����ÿ��agent
					priorityMax = 0; //���ȼ��㷨�õ������ȼ�Խ��Խ��
					priorityTmp = 0;
					for (int j = 0; j < NUM_GOAL; j++) {//����ÿ��goal,�������ȼ�
						/**/
						if (_algorithmID == 0) {

							priorityTmp = 1/distanceVec[i][j];
						}
						else if (_algorithmID == 1) {
							if (populationExit == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
							else priorityTmp = 3*(1 - ExitReagionInfo[j] / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]);
						}
						else if (_algorithmID == 2) {
							if (populationExit == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
							else priorityTmp = 3*(1 - (ExitReagionInfo[j]/ populationCapacity) / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]);
						}
						else if (_algorithmID == 3) {
                            if (populationExit == 0 || pathPeopleNum[i] == 0) priorityTmp = 2 - distanceVec[i][j] / distanceSum[i];
                            else priorityTmp = 3*(1 - (ExitReagionInfo[j]/ populationCapacity) / populationExit) + (1 - distanceVec[i][j] / distanceSum[i]) + (1 - peopleNumVec[i][j] / pathPeopleNum[i]);
						}
						else {
							cout <<"AlgorithmID wrong" << endl;
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

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AlgorithmGoalSelectorFactory
		/////////////////////////////////////////////////////////////////////

		AlgorithmGoalSelectorFactory::AlgorithmGoalSelectorFactory() : GoalSelectorFactory() {
			_goalSetID = _attrSet.addSizeTAttribute("goal_set", true /*required*/);
			_algorithmID = _attrSet.addSizeTAttribute("algorithmID", true /*required*/);
		}

		/////////////////////////////////////////////////////////////////////

		bool AlgorithmGoalSelectorFactory::setFromXML(GoalSelector* selector, TiXmlElement* node,
			const std::string& behaveFldr) const {
			AlgorithmGoalSelector* sgs = dynamic_cast<AlgorithmGoalSelector*>(selector);
			assert(sgs != 0x0 &&
				"Trying to set goal set goal selector attributes on an incompatible object.");

			if (!GoalSelectorFactory::setFromXML(sgs, node, behaveFldr)) return false;

			sgs->setGoalSetID(_attrSet.getSizeT(_goalSetID));
			sgs->setAlgorithmID(_attrSet.getSizeT(_algorithmID));

			return true;
		}
		

		

	}	// namespace BFSM
 
}	// namespace Menge
