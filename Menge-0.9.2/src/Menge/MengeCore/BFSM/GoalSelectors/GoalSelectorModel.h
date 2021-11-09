/*!
 *	@file		GoalSelectorModel.h
 *	@brief		GoalSelectorModel. 
 */

#ifndef __GOAL_SELECTOR_MODEL_H__
#define __GOAL_SELECTOR_MODEL_H__

#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorSet.h"
#include "MengeCore/Socket.h"


namespace Menge {

	namespace BFSM {
		/*!
		 *	@brief		The weighted goal selector selects a goal from a goal set with
		 *				weighted probability (i.e., it uses the user-specified goal weights).
		 */
		class MENGE_API ModelGoalSelector : public SetGoalSelector {
		public:
			ModelGoalSelector();
			/*!
			 *	@brief		Interface function for acquiring per-agent goals.
			 *
			 *	@param		agent		The agent for whom a goal is extracted.
			 *	@returns	A pointer to a goal.  
			 *	// TODO: Figure out who owns this goal.
			 */
			virtual Goal * getGoal( const Agents::BaseAgent * agent ) const;

			SOCKET client;

		};

		/*!
		 *	@brief		Factory for the MatrixGoalSelector.
		 */
		class MENGE_API ModelGoalSelectorFactory : public SetGoalSelectorFactory {
		public:

			/*!
			 *	@brief		The name of the goal selector type.
			 *
			 *	The goal selector's name must be unique among all registered goal selectors.
			 *	Each goal selector factory must override this function.
			 *
			 *	@returns	A string containing the unique goal selector name.
			 */
			virtual const char * name() const { return "Model"; }

			/*!
			 *	@brief		A description of the goal selector.
			 *
			 *	Each goal selector factory must override this function.
			 *
			 *	@returns	A string containing the goal selector description.
			 */
			virtual const char * description() const {
				return  "A goal selector.  Assigns the agent a goal drawn randomly "
						"from a goal set with weighted  probability.";
			};

		protected:
			/*!
			 *	@brief		Create an instance of this class's goal selector.
			 *
			 *	@returns		A pointer to a newly instantiated GoalSelector class.
			 */
			GoalSelector * instance() const { return new ModelGoalSelector(); }
		};
	}	// namespace BFSM
}	// namespace Menge

#endif // __GOAL_SELECTOR_REALITY_H__
