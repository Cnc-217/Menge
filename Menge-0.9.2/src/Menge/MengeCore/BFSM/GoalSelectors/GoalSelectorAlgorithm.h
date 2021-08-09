

/*!
 *	@file		GoalSelectorAlgorithm.h
 *	@brief		The definition of the Algorithm goal selector for Evacuation. 
 */

#ifndef __GOAL_SELECTOR_ALGORITHM_H__
#define __GOAL_SELECTOR_ALGORITHM_H__

#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorFactory.h"
#include "MengeCore/BFSM/GoalSet.h"

namespace Menge {

	namespace BFSM {
		using namespace std;
		/*!
		 *	@brief		The Algorithm goal selector selects the goal from a goal set that
		 *				using the Algorithm.
		 */
		class MENGE_API AlgorithmGoalSelector : public GoalSelector {

		public:
			/*!
			 *	@brief		Interface function for acquiring per-agent goals.
			 *
			 *	@param		agent		The agent for whom a goal is extracted.
			 *	@returns	A pointer to a goal.  
			 *	// TODO: Figure out who owns this goal.
			 */

			AlgorithmGoalSelector();

			/*!
			 *	@brief		Gives the instance the opportunity to set the goal set.
			 *
			 *	This does nothing for instances which don't require a goal set.  Otherwise,
			 *	it extracts its required goal set.
			 *
			 *	@param		goalSets	A mapping from goal set identifier to goal set pointers.
			 */
			void setGoalSet(std::map< size_t, GoalSet* >& goalSets);

			/*!
			 *	@brief		Sets the goal set identifier.
			 *
			 *	@param		id		The goal set's id that belongs to this selector.
			 */
			void setGoalSetID(size_t id) { _goalSetID = id; }
			void setAlgorithmID(size_t id) { _algorithmID = id; }

			virtual Goal * getGoal( const Agents::BaseAgent * agent ) const;	

			static bool _flag;
			static map <size_t, Goal*> _bestGoals;

		protected:
			/*!
			 *	@brief		Allows the goal selector to lock any resources it
			 *				requires.
			 *
			 *	This is primarily here so that GoalSelectors which use shared
			 *	resources have a chance to lock them (see SetGoalSelector).
			 *	A call to lockResources should always be followed by a call
			 *	to releeaseResources.
			 */
			virtual void lockResources() { _goalSet->lockRead(); }

			/*!
			 *	@brief		Allows the goal selector to release previously locked
			 *				resources.
			 *
			 *	Should be used in conjunction with lockResources.
			 */
			virtual void releaseResources() { _goalSet->releaseRead(); }

			/*!
			 *	@brief		The goal set associated with this goal selector.
			 *				During parsing, it contains the id of the goal set.
			 *				After FSM construction, it contains a pointer to the
			 *				actual goal set.
			 */

			size_t		_goalSetID;		///< The identifier for the goal set
			GoalSet* _goalSet;		///< The pointer to the goal set.
			size_t	_algorithmID;
		};

		/*!
		 *	@brief		Factory for the AlgorithmGoalSelector.
		 */
		class MENGE_API AlgorithmGoalSelectorFactory : public GoalSelectorFactory {
		public:
			AlgorithmGoalSelectorFactory();
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
				 *	@brief		Given a pointer to a GoalSelector instance, sets the appropriate fields
				 *				from the provided XML node.
				 *
				 *	@param		selector	A pointer to the goal whose attributes are to be set.
				 *	@param		node		The XML node containing the goal selector attributes.
				 *	@param		behaveFldr	The path to the behavior file.  If the goal selector
				 *							references resources in the file system, it should be
				 *							defined relative to the behavior file location.  This is
				 *							the folder containing that path.
				 *	@returns	A boolean reporting success (true) or failure (false).
				 */
			virtual bool setFromXML(GoalSelector* selector, TiXmlElement* node,
				const std::string& behaveFldr) const;

			/*!
			 *	@brief		The identifier for the "goal_set" size_t attribute.
			 */
			size_t	_goalSetID;
			size_t	_algorithmID;
			GoalSelector * instance() const {
				return new AlgorithmGoalSelector(); 
			}
		};
	}	// namespace BFSM
}	// namespace Menge

#endif // __GOAL_SELECTOR_ALGORITHM_H__
