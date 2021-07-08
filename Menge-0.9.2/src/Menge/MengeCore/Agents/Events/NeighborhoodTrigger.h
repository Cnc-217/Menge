/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *	@file		NeighborhoodTrigger.h
 *	@brief		The definitin of event triggers which key on Neighborhood Agent.
 */

#ifndef __NEIGHBORHOOD_EVT_TRIGGER_H__
#define __NEIGHBORHOOD_EVT_TRIGGER_H__

#include <vector>   
#include "MengeCore/Agents/BaseAgent.h"
#include <MengeCore/Agents/SimulatorInterface.h>
#include "MengeCore/Agents/Events/EventTrigger.h"
#include "MengeCore/Agents/Events/EventTriggerFactory.h"

namespace Menge {
	
	// forward declaration
	//class NeighborhoodEvtTriggerFactory;

	namespace BFSM {
		class State;
	}	

	//////////////////////////////////////////////////////////////////////////

	/*!
	 *	@brief		The base class event triggers which depend on time properties.
	 */
	class MENGE_API NeighborhoodEvtTrigger : public EventTrigger {
	public:
		/*!
		 *	@brief		Constructor.
		 */
		NeighborhoodEvtTrigger();

		/*!
		 *	@brief		Allows the trigger to finish initializing itself from its
		 *				parsed state to its running state.
		 */
		virtual void finalize();

		friend class NeighborhoodEvtTriggerFactory;

	protected:

	};

	//////////////////////////////////////////////////////////////////////////

	/*!
	 *	@brief		The factory for state event triggers.
	 */
	class MENGE_API NeighborhoodEvtTriggerFactory : public EventTriggerFactory {
	public:
		/*!
		 *	@brief		Constructor.
		 */
		NeighborhoodEvtTriggerFactory();

		/*!
		 *	@brief		The identifier for the "time" string attribute.
		 */
		size_t _timeSimulateIndex;
		

	protected:
		/*!
		 *	@brief		Given a pointer to an EventTrigger instance, sets the appropriate fields
		 *				from the provided XML node.
		 *
		 *	It is assumed that the value of the `type` attribute is this EventTrigger's type.
		 *	(i.e. EventTrigger::thisFactory has already been called and returned true.)
		 *	If sub-classes of EventTriggerFactory introduce *new* EventTrigger parameters, then the
		 *	sub-class should override this method but explicitly call the parent class's
		 *	version.
		 *
		 *	@param		trigger		A pointer to the event trigger whose attributes are to be set.
		 *	@param		node		The XML node containing the event trigger attributes.
		 *	@param		specFldr	The path to the specification file.  If the EventTrigger
		 *							references resources in the file system, it should be defined
		 *							relative to the specification file location.  This is the
		 *							folder containing that path. 
		 *	@returns	A boolean reporting success (true) or failure (false).
		 */
		virtual bool setFromXML( EventTrigger * trigger, TiXmlElement * node,
								 const std::string & specFldr ) const;

		

	};

	//////////////////////////////////////////////////////////////////////////

	/*!
	 *	@brief		A trigger that fires off when a state's population increases.
	 */
	class MENGE_API NeighborhoodDetectedTrigger : public NeighborhoodEvtTrigger {
	public:
		/*!
		 *	@brief		Constructor.
		 */
		NeighborhoodDetectedTrigger();

		/*!
		 *	@brief		Allows the trigger to finish initializing itself from its
		 *				parsed state to its running state.
		 *
		 *	@throws		EventException if there is a problem finalizing.
		 */
		virtual void finalize();
		static float _timeSimulate;
		static float _lastTimestamp;
		static bool _flag;
		static std::vector<Agents::BaseAgent*> _leaderSet;
		static std::vector<Agents::BaseAgent*> _panicSet;
		static std::vector<Agents::BaseAgent*> _normalSet;


	protected:

		/*!
		 *	@brief		Evaluates the condition to see if it has been met.
		 *
		 *	This must be implemented by all sub-classes.
		 *
		 *	@returns	True if the condition has been met, false otherwise.
		 */
		virtual bool testCondition();

		/*!
		 *	@brief		The last timestamp of pause.
		 */
		
	};

	//////////////////////////////////////////////////////////////////////////

	/*!
	 *	@brief		The factory for StatePopIncreaseTrigger event triggers.
	 */
	class MENGE_API NeighborhoodDetectedTriggerFactory : public NeighborhoodEvtTriggerFactory {
	public:
		/*!
		 *	@brief		The name of the trigger type.
		 *
		 *	The trigger's name must be unique among all registered triggers.  
		 *	Each trigger factory must override this function.
		 *
		 *	@returns	A string containing the unique trigger name.
		 */
		virtual const char * name() const { return "neighborhood_search"; }

		/*!
		 *	@brief		A description of the event trigger.
		 *
		 *	Each trigger factory must override this function.
		 *
		 *	@returns	A string containing the trigger description.
		 */
		virtual const char * description() const {
			return "Event trigger which fires when Neighborhoo Detected.";
		};

	protected:
		/*!
		 *	@brief		Create an instance of this class's event trigger implementation.
		 *
		 *	All EventTriggerFactory sub-classes must override this by creating (on the heap)
		 *	a new instance of its corresponding trigger type.  The various field values
		 *	of the instance will be set in a subsequent call to EventTriggerFactory::setFromXML.
		 *	The caller of this function takes ownership of the memory.
		 *
		 *	@returns		A pointer to a newly instantiated EventTrigger class.
		 */
		EventTrigger * instance() const { return new NeighborhoodDetectedTrigger(); }
	};

}	// namespace Menge
#endif	// __TIME_EVT_TRIGGER_H__
