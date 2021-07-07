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

#pragma once

/*!
 *	@file		TargetAgentByClass.h
 *	@brief		Defines the EventTarget that targets agents by class.
 */

#include "MengeCore/Agents/Events/AgentEventTarget.h"

namespace Menge {
	/*!
	 *	@brief		This class defines the target of an event as being a single agent with the
	 *				given class id.
	 *
	 *	@code{xml}
	 *	<Target name="agent_0" type="class_id" id="0" />
	 *	@endcode
	 *
	 *	- The value `name` must be unique and is referenced in the Event response.
	 *	- The `type` value specifies this target -- a single agent with the given id.
	 *	- The `id` value specifies the targeted agent, by its identifier.
	 */
	class MENGE_API TargetAgentByClass : public AgentEventTarget {
	public:
		/*!
		 *	@brief		Default constructor.
		 */
		TargetAgentByClass() : AgentEventTarget() {}

		/*!
		 *	@brief		Gives the target a chance to update its knowledge of the
		 *				target entities.
		 *
		 *	If a sub-class needs to do particular computation to evaluate the target,
		 *	it should be implemented here.
		 */
		virtual void update();
		
		friend class TargetAgentByClassFactory;

	protected:
		/*!
		 *	@brief		The id of the agent to target.
		 */
		size_t _classId{ 1 };
		float _probChoose;
	};

	/*!
	 *	@brief		The factory to generate TargetAgentById instances.
	 */
	class MENGE_API TargetAgentByClassFactory : public AgentEventTargetFactory {
	public:
		/*!
		*	@brief		Constructor.
		*/
		TargetAgentByClassFactory();

		/*!
		 *	@brief		The name of the target.
		 *
		 *	The target's name must be unique among all registered targets.
		 *	Each target factory must override this function.
		 *
		 *	@returns	A string containing the unique target name.
		 */
		virtual const char * name() const { return "class_id"; }

		/*!
		 *	@brief		A description of the target.
		 *
		 *	Each target factory must override this function.
		 *
		 *	@returns	A string containing the target description.
		 */
		virtual const char * description() const {
			return "Defines an agent as a target based on its unique identifier.";
		};

	protected:
		/*!
		 *	@brief		Create an instance of this class's target.
		 *
		 *	All EventTargetFactory sub-classes must override this by creating (on the heap)
		 *	a new instance of its corresponding event target type.  The various field values
		 *	of the instance will be set in a subsequent call to EventTargetFactory::setFromXML.
		 *	The caller of this function takes ownership of the memory.
		 *
		 *	@returns		A pointer to a newly instantiated EventTarget class.
		 */
		EventTarget * instance() const { return new TargetAgentByClass(); }

		/*!
		 *	@brief		Given a pointer to an EventTarget instance, sets the appropriate fields
		 *				from the provided XML node.
		 *
		 *	It is assumed that the value of the `type` attribute is this EventTarget's type.
		 *	(i.e. EventTargetFactory::thisFactory has already been called and returned true.)
		 *	If sub-classes of EventTargetFactory introduce *new* EventTarget parameters, then the
		 *	sub-class should override this method but explicitly call the parent class's
		 *	version.
		 *
		 *	@param		target			A pointer to the target whose attributes are to be set.
		 *	@param		node			The XML node containing the target attributes.
		 *	@param		behaveFldr		The path to the behavior file.  If the target references
		 *								resources in the file system, it should be defined relative
		 *								to the behavior file location.  This is the folder
		 *								containing that path.
		 *	@returns	A boolean reporting success (true) or failure (false).
		 */
		virtual bool setFromXML(EventTarget * target, TiXmlElement * node,
			const std::string & behaveFldr) const;

		/*!
		*	@brief		The identifier for the "id" size_t attribute.
		*/
		size_t	_classID;

		/*!
		*	@brief		The identifier for the "prob" size_t attribute.
		*/
		size_t _prob;
	};
}	// namespace Menge
