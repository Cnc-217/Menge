/*
 Menge Crowd Simulation Framework
 Copyright and trademark 2019 Sean Curtis
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
 or
	LICENSE.txt in the root of the Menge repository.
 Any questions or comments should be sent to the authors menge@cs.unc.edu
 <http://gamma.cs.unc.edu/Menge/>
*/

#include <memory>

#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/PrefVelocity.h"
#include "MengeCore/BFSM/Goals/GoalAgent.h"

namespace Menge {
	namespace BFSM {

		using Math::Vector2;
		using std::vector;

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AgentGoal
		/////////////////////////////////////////////////////////////////////

		const std::string AgentGoal::NAME = "agent";

		/////////////////////////////////////////////////////////////////////

		void AgentGoal::move(float time_step) { //控制goal如何移动
		
			//思路1：一个goal对应一个agent的id，goal坐标=agent的坐标即可
			CircleShape* geo = (CircleShape*)_geometry;
			geo->setCenter(Menge::SIMULATOR->getAgent(_leaderID)->_pos);
			//_p_WGo = Menge::SIMULATOR->getAgent(_leaderID)->_pos;
			//SIMULATOR->getBFSM()->getGoal(0,0).
			//set_velocity();

			
		}
	

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of AgentGoalFactory
		/////////////////////////////////////////////////////////////////////

		AgentGoalFactory::AgentGoalFactory() {
			_shapeId = _attrSet.addStringAttribute("shape", true /* required */);
			_leaderId = _attrSet.addSizeTAttribute("idleader", true);
		}

		/////////////////////////////////////////////////////////////////////

		bool AgentGoalFactory::setFromXML(Goal* goal, TiXmlElement* node,
			const std::string& behaveFldr) const {
			AgentGoal* aGoal = dynamic_cast<AgentGoal*>(goal);
			assert(aGoal != 0x0 && "Trying to set path goal attributes on an incompatible object.");

			if (!GoalFactory::setFromXML(aGoal, node, behaveFldr)) return false;

		
			TiXmlElement* child{};

			Math::Geometry2D* geometry = createGeometry(node);
			// NOTE: Error messages have already been logged by createGeometry() in the event of not returning
			// a valid pointer.
			if (geometry == nullptr) return false;

			aGoal->setGeometry(geometry);
			size_t leaderID = _attrSet.getSizeT(_leaderId);
			aGoal->_leaderID = leaderID;
			
			return true;
		}
	}  // namespace BFSM
}  // namespace Menge