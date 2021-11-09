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

#include "MengeCore/Agents/Events/TimeEvtTrigger.h"

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
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Socket.h"

#include "MengeCore/Core.h"
#include "MengeVis/Viewer/GLViewer.h"


#include <sstream>

//extern MengeVis::Viewer::GLViewer* SimViewer;

namespace Menge {

	using namespace std;
	using namespace Menge::BFSM;

	float TimeReachedTrigger::_timeSimulate = 10;
	float TimeReachedTrigger::_lastTimestamp = 0;

	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTrigger
	/////////////////////////////////////////////////////////////////////

	TimeEvtTrigger::TimeEvtTrigger() : EventTrigger() {
	}

	/////////////////////////////////////////////////////////////////////

	void TimeEvtTrigger::finalize() {
		
	}
	
	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTriggerFactory
	/////////////////////////////////////////////////////////////////////

	TimeEvtTriggerFactory::TimeEvtTriggerFactory() : EventTriggerFactory() {
		_timeSimulateIndex = _attrSet.addFloatAttribute( "timeSimulate", true, 0.0 );//设置仿真时间
	}


	/////////////////////////////////////////////////////////////////////

	bool TimeEvtTriggerFactory::setFromXML( EventTrigger * trigger, TiXmlElement * node,
											 const std::string & specFldr ) const {
		TimeEvtTrigger* tTrigger = dynamic_cast<TimeEvtTrigger* >( trigger );
		assert(tTrigger != 0x0 &&
				"Trying to set attributes of a state event trigger on an incompatible object" );

		if ( ! EventTriggerFactory::setFromXML(tTrigger, node, specFldr ) ) return false;

		TimeReachedTrigger::_timeSimulate = _attrSet.getFloat(_timeSimulateIndex);

		return true;
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of StatePopIncreaseTrigger
	/////////////////////////////////////////////////////////////////////

	// _lastPop is initialized to a ridiculously high number in order to keep
	//	the event from triggering upon initialization.  The first call to
	//	testCondition will bring it back down.
	TimeReachedTrigger::TimeReachedTrigger() : TimeEvtTrigger() {
		TimeReachedTrigger::_timeSimulate = 10;
		TimeReachedTrigger::_lastTimestamp = 0;
	}

	/////////////////////////////////////////////////////////////////////

	void TimeReachedTrigger::finalize() {
		_lastTimestamp = Menge::SIM_TIME;//完成后获取当前的仿真时间
	}

	/////////////////////////////////////////////////////////////////////

	bool TimeReachedTrigger::testCondition() { //检测触发条件
		if ((Menge::SIM_TIME - _lastTimestamp) > _timeSimulate) {
			cout << "trigger condition met at :"<<Menge::SIM_TIME << endl;
			_lastTimestamp = Menge::SIM_TIME;
			//MengeVis::SimViewer->_pause = !MengeVis::SimViewer->_pause;
			cout << "pause" << endl;
			
			//重新开始
			//MengeVis::SimViewer->_pause = !MengeVis::SimViewer->_pause;
			cout << "Timestamp: " << _lastTimestamp << endl;
			return true;
		}

		return false;
	}

}	// namespace Menge 
