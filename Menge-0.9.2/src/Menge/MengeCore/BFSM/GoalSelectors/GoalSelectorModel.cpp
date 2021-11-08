

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorModel.h"

#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Core.h"


#include <cassert>

namespace Menge {

	namespace BFSM {
		
		/////////////////////////////////////////////////////////////////////
		//                   Implementation of ModelGoalSelector
		/////////////////////////////////////////////////////////////////////
		

		ModelGoalSelector::ModelGoalSelector(){
			client = Socket::socketClientInit("127.0.0.1", 12661);
		}

		Goal * ModelGoalSelector::getGoal( const Agents::BaseAgent * agent ) const {
			


			return _goalSet->getGoalFromMatrix(agent);
			

		}
		
	}	// namespace BFSM
}	// namespace Menge
