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


		if (PROJECTNAME == EVACUATION) {

			if (_flag == false) {//��һ�ν��룬����Ⱥ�ֽ�vector��
				for (int idx = 0; idx < Menge::SIMULATOR->getNumAgents(); idx++) {
					Agents::BaseAgent* agent = Menge::SIMULATOR->getAgent(idx);
					if (agent->_class == 0) _leaderSet.push_back(agent);
					else if (agent->_class == 1) _panicSet.push_back(agent);
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
					currentState->leave(*agent);
					AlgorithmGoalSelector* algorithmGoalSelector = (AlgorithmGoalSelector*)currentState->getGoalSelector();
					if (agent == _leaderSet.begin()) algorithmGoalSelector->_flag = false;
					currentState->enter(*agent);
					cout << "leader ID: " << (*agent)->_id << "choose new goal" << endl;
				}
				_lastTimestamp = Menge::SIM_TIME;
			}

			//ÿһ���ֻ����ж���û���������ڸ������еĻ��͸���
			vector<Agents::BaseAgent*>::iterator agent;
			for (agent = _panicSet.begin(); agent != _panicSet.end(); agent++) {
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
				if (currentState->getName() == "Follow_leader") continue;
				vector<Menge::Agents::NearAgent>::iterator nearAgent;

				//����ÿһ����������
				for (nearAgent = (*agent)->_nearAgents.begin(); nearAgent != (*agent)->_nearAgents.end(); ++nearAgent) {
					//for (nearAgent = Menge::SIMULATOR->getAgent(idx)->_nearAgents.begin(); nearAgent != Menge::SIMULATOR->getAgent(idx)->_nearAgents.end(); ++nearAgent) {
						/*
						if ((*agent)->_id == 31) {
							cout << "idx: " << idx << " " << (*nearAgent).agent->_id << " class : " << (*nearAgent).agent->_class << endl;
							if ((*nearAgent).agent->_class == 0) cout << "pop" << endl;
						}*/
					//������leader
					if ((*nearAgent).agent->_class == 0) {
						currentState->leave((*agent));
						State* nextState = Menge::ACTIVE_FSM->getNode("Follow_leader");//������state
						nextState->enter((*agent));
						Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
						break;
					}
				}

			}

			//��ͨ���жϸ������޿ֻ��ߺ������⣬�еĻ��͸��棬����������
			for (agent = _normalSet.begin(); agent != _normalSet.end(); agent++) {
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(*agent);
				if (currentState->getName() == "Follow_leader") continue;
				vector<Menge::Agents::NearAgent>::iterator nearAgent;
				for (nearAgent = (*agent)->_nearAgents.begin(); nearAgent != (*agent)->_nearAgents.end(); ++nearAgent) {
					{
						//������leader
						if ((*nearAgent).agent->_class == 0) {
							currentState->leave((*agent));
							State* nextState = Menge::ACTIVE_FSM->getNode("Follow_leader");
							nextState->enter((*agent));
							Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
							break;
						}
						//������panicer
						else if ((*nearAgent).agent->_class == 1) {
							currentState->leave((*agent));
							State* nextState = Menge::ACTIVE_FSM->getNode("Follow_panic");//������state��������ôѡ��goal��goalselecter
							nextState->enter((*agent));
							Menge::ACTIVE_FSM->setCurrentState((*agent), nextState->getID());
							break;
						}
					}

				}

			}
		}
		else {
			//do nothing
		}

		

		return false;
	}

}	// namespace Menge 
