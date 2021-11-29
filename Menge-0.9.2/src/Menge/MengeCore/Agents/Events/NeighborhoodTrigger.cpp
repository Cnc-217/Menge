/*

License

Menge
Copyright ?and trademark ?2012-14 University of North Carolina at Chapel Hill. 
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation 
for educational, research, and non-profit purposes, without fee, and without a 
written agreement is hereby granted, provided that the above copyright notice, 
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North 
Carolina at Chapel Hill. The software program and documentation are supplied "as is," 
without any accompanying services from the University of North Carolina at Chapel 
Hill or the authors. The University of North Carolina at Chapel Hill and the 
authors do not warrant that the operation of the program will be uninterrupted 
or error-free. The end-user understands that the program was developed for research 
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE 
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY 
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY 
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND 
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "MengeCore/Agents/Events/NeighborhoodTrigger.h"

#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorAlgorithm.h"
#include "MengeCore/MatrixMy.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#include "MengeCore/Core.h"
#include "MengeVis/Viewer/GLViewer.h"

#include <sstream>
#include <MengeCore\Scene\BaseScene.h>
using namespace Menge::Olympic;

namespace Menge {

	using namespace std;
	using namespace Menge::BFSM;

	//�Ծ�̬������ʼ��
	float NeighborhoodDetectedTrigger::_timeSimulate = 10;
	float NeighborhoodDetectedTrigger::_lastTimestamp = 0;
	bool NeighborhoodDetectedTrigger::_flag = false;
	std::vector<Agents::BaseAgent*> NeighborhoodDetectedTrigger::_leaderSet;
	std::vector<Agents::BaseAgent*> NeighborhoodDetectedTrigger::_panicSet;
	std::vector<Agents::BaseAgent*> NeighborhoodDetectedTrigger::_normalSet;
	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTrigger
	/////////////////////////////////////////////////////////////////////

	NeighborhoodEvtTrigger::NeighborhoodEvtTrigger() : EventTrigger() {
	}

	/////////////////////////////////////////////////////////////////////

	void NeighborhoodEvtTrigger::finalize() {
		
	}
	
	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTriggerFactory
	/////////////////////////////////////////////////////////////////////

	NeighborhoodEvtTriggerFactory::NeighborhoodEvtTriggerFactory() : EventTriggerFactory() {
		_timeSimulateIndex = _attrSet.addFloatAttribute( "timeSimulate", true, 0.0 );//���÷���ʱ��
	}


	/////////////////////////////////////////////////////////////////////

	bool NeighborhoodEvtTriggerFactory::setFromXML( EventTrigger * trigger, TiXmlElement * node,
											 const std::string & specFldr ) const {
		NeighborhoodEvtTrigger* tTrigger = dynamic_cast<NeighborhoodEvtTrigger* >( trigger );
		assert(tTrigger != 0x0 &&
				"Trying to set attributes of a state event trigger on an incompatible object" );

		if ( ! EventTriggerFactory::setFromXML(tTrigger, node, specFldr ) ) return false;

		NeighborhoodDetectedTrigger::_timeSimulate = _attrSet.getFloat(_timeSimulateIndex);

		return true;
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of StatePopIncreaseTrigger
	/////////////////////////////////////////////////////////////////////

	NeighborhoodDetectedTrigger::NeighborhoodDetectedTrigger() : NeighborhoodEvtTrigger() {
		NeighborhoodDetectedTrigger::_timeSimulate = 10;
		NeighborhoodDetectedTrigger::_lastTimestamp = 0;
		NeighborhoodDetectedTrigger::_flag = false;
		NeighborhoodDetectedTrigger::_leaderSet.clear();
		NeighborhoodDetectedTrigger::_panicSet.clear();
		NeighborhoodDetectedTrigger::_normalSet.clear();
	}

	/////////////////////////////////////////////////////////////////////

	void NeighborhoodDetectedTrigger::finalize() {
		_lastTimestamp = Menge::SIM_TIME;//��ɺ��ȡ��ǰ�ķ���ʱ��
	}

	/////////////////////////////////////////////////////////////////////

	bool NeighborhoodDetectedTrigger::testCondition() { //��ⴥ������

		if(PROJECTNAME == OLYMPIC) {
			//��ɢ״̬�Ŀ����ź���
			if (Menge::Olympic::evacuationState == true) {
			if (_flag == false) {//��һ�ν��룬����Ⱥ�ֽ�vector��
				for (int idx = 0; idx < Menge::SIMULATOR->getNumAgents(); idx++) {
					Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(idx);
					if (agent->_class == 1) _leaderSet.push_back(agent);
					else if (agent->_class == 2) _panicSet.push_back(agent);
					else _normalSet.push_back(agent);
				}
				_flag = true;
				}
				

				//ÿһ��ʱ����һ��leader��goal,������ѡ�����
				if ((Menge::SIM_TIME - _lastTimestamp) > _timeSimulate) {//ʱ�䵽��
					cout << "trigger condition met at :" << Menge::SIM_TIME << endl;
					vector<Agents::BaseAgent*>::iterator agent;
					for (agent = _leaderSet.begin(); agent != _leaderSet.end(); agent++) {
						
						State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
						if (currentState->getName() == "Stop") continue;
						currentState->leave(*agent);
						AlgorithmGoalSelector* algorithmGoalSelector = (AlgorithmGoalSelector*)currentState->getGoalSelector();
						if (agent == _leaderSet.begin()) algorithmGoalSelector->_flag = false;//�㷨һ��ֻ��Ҫ����һ��
						currentState->enter(*agent);
						cout << "leader ID: " << (*agent)->_id << " choose new goal" << endl;
					}
					_lastTimestamp = Menge::SIM_TIME;

					vector<size_t>::iterator it;
					int idx = 0;
					for (it = Menge::BaseScene::ExitReagionInfo.begin(); it != Menge::BaseScene::ExitReagionInfo.end(); it++) {
						cout << "reagionID: " << idx << " reagionPopulation: " << (*it) << endl;
						idx++;
					}
				}

				//ÿһ���������ж��Ƿ�����˳�������
				vector<Agents::BaseAgent*>::iterator agent;
				for (agent = Menge::Olympic::leaderAgentSet.begin(); agent != Menge::Olympic::leaderAgentSet.end(); agent++) {
					State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
					if (currentState->getName() == "Stop") continue;
					else if (Menge::BaseScene::ExitAgentInfo[(*agent)->_id] != 1000) {//������ɢ���ڸ���
						currentState->leave((*agent));
						State* nextState = Menge::ACTIVE_FSM->getNode("Stop");
						nextState->enter((*agent));
						Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
					}
				}

				//ÿһ���ֻ����ж���û�н�����������Ƿ����������ڸ������еĻ��͸���
				for (agent = Menge::Olympic::panicAgentSet.begin(); agent != Menge::Olympic::panicAgentSet.end(); agent++) {
					State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
					if (currentState->getName() == "Stop") continue;
					else if (Menge::BaseScene::ExitAgentInfo[(*agent)->_id] != 1000) {//������ɢ���ڸ���
						currentState->leave((*agent));
						State* nextState = Menge::ACTIVE_FSM->getNode("Stop");
						nextState->enter((*agent));
						Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
						continue;
					}

					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					//����ÿһ����������
					for (nearAgent = (*agent)->_nearAgents.begin(); nearAgent != (*agent)->_nearAgents.end(); ++nearAgent) {
						if ((*nearAgent).agent->_class == 1) {
							currentState->leave((*agent));
							State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");//���½���Evacuation��ѡ���µ�Ŀ��
							nextState->enter((*agent));
							Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
							break;
						}
					}

				}

				//ÿһ����ͨ���ж��ж���û�н���������򣻸������޿ֻ��ߺ������ߣ��еĻ��͸��棬����������
				for (agent = Menge::Olympic::normalAgentSet.begin(); agent != Menge::Olympic::normalAgentSet.end(); agent++) {
					State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
					if (currentState->getName() == "Stop") continue;
					else if (Menge::BaseScene::ExitAgentInfo[(*agent)->_id] != 1000) {//�����˳�������
						currentState->leave((*agent));
						State* nextState = Menge::ACTIVE_FSM->getNode("Stop");
						nextState->enter((*agent));
						Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
						continue;
					}

					//�жϸ�������
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					for (nearAgent = (*agent)->_nearAgents.begin(); nearAgent != (*agent)->_nearAgents.end(); ++nearAgent) {
						if ((*nearAgent).agent->_class == 1 || (*nearAgent).agent->_class == 2) {
							currentState->leave((*agent));
							State* nextState = Menge::ACTIVE_FSM->getNode("Evacuation");
							nextState->enter((*agent));
							Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
							break;
						}
					}

				}
			}
			/*
			else
			{
			Menge::BaseScene::updateRoadNum();
			for (size_t i = 0; i < roadRegionInfo.size(); i++)
				cout << "roadID : "<<i<<" people num: "<<roadRegionInfo[i].peopleNumInThisRoad << endl;
			}
			*/
			
		}
		
		return false;
	}
	
}	// namespace Menge 
