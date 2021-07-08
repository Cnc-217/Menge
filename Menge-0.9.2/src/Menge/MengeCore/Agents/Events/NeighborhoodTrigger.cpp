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

	//对静态变量初始化
	float NeighborhoodDetectedTrigger::_timeSimulate = 10;
	float NeighborhoodDetectedTrigger::_lastTimestamp = 0;
	bool NeighborhoodDetectedTrigger::_flag = false;
	std::vector<Agents::BaseAgent*>NeighborhoodDetectedTrigger::_leaderSet;
	std::vector<Agents::BaseAgent*>NeighborhoodDetectedTrigger::_panicSet;
	std::vector<Agents::BaseAgent*>NeighborhoodDetectedTrigger::_normalSet;
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
		_timeSimulateIndex = _attrSet.addFloatAttribute( "timeSimulate", true, 0.0 );//设置仿真时间
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
		_lastTimestamp = Menge::SIM_TIME;//完成后获取当前的仿真时间
	}

	/////////////////////////////////////////////////////////////////////

	bool NeighborhoodDetectedTrigger::testCondition() { //检测触发条件

		//如果是第一次进入testcondition，将所有agent的信息存入hashset便于后面遍历
		if (_flag == false) {
			const size_t AGT_COUNT = Menge::SIMULATOR->getNumAgents();
			for (size_t a = 0; a < AGT_COUNT; ++a) {
				Agents::BaseAgent* agt = Menge::SIMULATOR->getAgent(a);
				if (agt->_class == 0) _leaderSet.push_back(agt); //leader
				else if (agt->_class == 1) _panicSet.push_back(agt);//panic
				else _normalSet.push_back(agt);
			}
			_flag = true;
		}

		if (PROJECTNAME == EVACUATION) {
			//每一段时间检测一次leader的goal
			if ((Menge::SIM_TIME - _lastTimestamp) > _timeSimulate) {
				cout << "trigger condition met at :" << Menge::SIM_TIME << endl;
				_lastTimestamp = Menge::SIM_TIME;

				vector<Agents::BaseAgent*>::iterator iter;
				for (iter = _leaderSet.begin(); iter != _leaderSet.end(); ++iter) {
					Agents::BaseAgent* agentLeader = const_cast<Agents::BaseAgent*>(*iter);
					State* currentState = Menge::ACTIVE_FSM->getCurrentState(agentLeader);
					currentState->leave(agentLeader);
					AlgorithmGoalSelector* algorithmGoalSelector = (AlgorithmGoalSelector*)currentState->getGoalSelector();
					if(iter == _leaderSet.begin()) algorithmGoalSelector->_flag = false;
					currentState->enter(agentLeader);
					cout << "leader ID: " << agentLeader->_id << "choose new goal"<<endl;
				}

			return true;
			}

			//随时检测其他类型agent周围是否有其他类型
			vector<Agents::BaseAgent*>::iterator iter;
			for (iter = _panicSet.begin(); iter != _leaderSet.end(); ++iter) {
				Agents::BaseAgent* agentLeader = const_cast<Agents::BaseAgent*>(*iter);
				
			}

			//vector<Agents::BaseAgent*>::iterator iter;
			for (iter = _normalSet.begin(); iter != _leaderSet.end(); ++iter) {
				Agents::BaseAgent* agentLeader = const_cast<Agents::BaseAgent*>(*iter);

			}

		}
		else {
			//do nothing
		}


		return false;
	}

}	// namespace Menge 
