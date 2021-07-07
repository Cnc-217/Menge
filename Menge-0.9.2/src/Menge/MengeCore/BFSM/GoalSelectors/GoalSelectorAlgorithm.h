

/*!
 *	@file		GoalSelectorAlgorithm.h
 *	@brief		The definition of the Algorithm goal selector for Evacuation. 
 */

#ifndef __GOAL_SELECTOR_ALGORITHM_H__
#define __GOAL_SELECTOR_ALGORITHM_H__

#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorSet.h"


namespace Menge {

	namespace BFSM {
		using namespace std;
		/*!
		 *	@brief		The Algorithm goal selector selects the goal from a goal set that
		 *				using the Algorithm.
		 */
		class MENGE_API AlgorithmGoalSelector : public SetGoalSelector {

		public:
			/*!
			 *	@brief		Interface function for acquiring per-agent goals.
			 *
			 *	@param		agent		The agent for whom a goal is extracted.
			 *	@returns	A pointer to a goal.  
			 *	// TODO: Figure out who owns this goal.
			 */

			AlgorithmGoalSelector();
			virtual Goal * getGoal( const Agents::BaseAgent * agent ) const;	

			static bool _flag;
			static map <size_t, Goal*> _bestGoals;
		};

		/*!
		 *	@brief		Factory for the NearestGoalSelector.
		 */
		class MENGE_API AlgorithmGoalSelectorFactory : public SetGoalSelectorFactory {
		public:
			/*!
			 *	@brief		The name of the goal selector type.
			 *
			 *	The goal selector's name must be unique among all registered goal selectors.
			 *	Each goal selector factory must override this function.
			 *
			 *	@returns	A string containing the unique goal selector name.
			 */
			virtual const char * name() const { return "algorithm"; }

			/*!
			 *	@brief		A description of the goal selector.
			 *
			 *	Each goal selector factory must override this function.
			 *
			 *	@returns	A string containing the goal selector description.
			 */
			virtual const char * description() const {
				return  "A goal selector.  Assigns the agent the goal in the given "
						"goal set that is *Algorithm* the agent.";
			};

		protected:
			/*!
			 *	@brief		Create an instance of this class's goal selector.
			 *
			 *	@returns		A pointer to a newly instantiated GoalSelector class.
			 */
			GoalSelector * instance() const {
				return new AlgorithmGoalSelector(); 
			}
		};
	}	// namespace BFSM
}	// namespace Menge

#endif // __GOAL_SELECTOR_ALGORITHM_H__
