#pragma once

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
 *	@file	CodeEffectAgentState.h
 *	@brief	Provides the definition of code that changes an agent's state.
 */

#include "MengeCore/Agents/Events/AgentEventEffect.h"

#include <cassert>

namespace Menge {
	// forward declarations.
	namespace Agents {
		class StateSelector;
	}
	namespace BFSM {
		class State;
	}

	/*!
	 *	@brief		An event effect that can change the state the target agents are in.
	 *
	 *	This class uses the StateSelector to determine the target state. If the agent is already
	 *	in the state selected by the Effect's selector, the effect can either cause the agent to
	 *	exit and re-enter the state, or to do nothing. 
	 *
	 *	@code{xml}
	 *	<Effect name="xxx" type="set_state_by_code" > 不需要reenter，因为我们一定是在无优惠券人群中发放
	 *	</Effect>
	 *	@endcode
	 *
	 *	- The value `name` must be unique and is referenced in the Event response.
	 *	- The `type` value specifies this effect -- setting the state of the agent.
	 *	- This EventEffect instance requires a child object: an instance of 
	 *		@ref Menge::Agents::StateSelector "StateSelector". The state the agent moves to will be determined by the
	 *		@ref Menge::Agents::StateSelector "StateSelector".
	 */
	class MENGE_API CodeEffectAgentState : public AgentEventEffect {
		/*!
		 *	@brief		Default constructor.
		 */
		CodeEffectAgentState();

		/*!
		 *	@brief		Destructor.
		 */
		~CodeEffectAgentState();

		friend class CodeEffectAgentStateFactory;

	protected:
		/*!
		 *	@brief		The actual work of the effect.
		 *
		 *	Sub-classes should implement this.  It is the action to be taken for each
		 *	agent.
		 *
		 *	@param		agent		The agent to operate on.
		 */
		void agentEffect(Agents::BaseAgent * agent) override;


	private:

	};

	/*!
	 *	@brief		The factory for generating instances of EventEffectAgentState from XML.
	 */
	class MENGE_API CodeEffectAgentStateFactory : public EventEffectFactory {
	public:
		/*!
		 *	@brief		Default constructor.
		 */
		CodeEffectAgentStateFactory();

		/*!
		 *	@brief		The name of the effect.
		 *
		 *	The effect's name must be unique among all registered effect.
		 *	Each effect factory must override this function.
		 *
		 *	@returns	A string containing the unique effect name.
		 */
		const char * name() const override { return "set_agent_state_by_code"; }

		/*!
		 *	@brief		A description of the effect.
		 *
		 *	Each effect factory must override this function.
		 *
		 *	@returns	A string containing the effect description.
		 */
		const char * description() const override {
			return "Moves the target agent from their current state to the state determined by "
				"the specified StateSelector. If the agent is already in the target state, "
				"nothing happens";
		};
	protected:
		/*!
		 *	@brief		Create an instance of this class's effect.
		 *
		 *	All EventEffectFactory sub-classes must override this by creating (on the heap)
		 *	a new instance of its corresponding event effect type.  The various field values
		 *	of the instance will be set in a subsequent call to EventEffectFactory::setFromXML.
		 *	The caller of this function takes ownership of the memory.
		 *
		 *	@returns		A pointer to a newly instantiated EventEffect class.
		 */
		EventEffect * instance() const override {
			return new CodeEffectAgentState();
		}

		/*!
		 *	@brief		Given a pointer to an AgentPropertyEffect instance, sets the appropriate
		 *				fields from the provided XML node.
		 *
		 *	It is assumed that the value of the `type` attribute is this AgentPropertyEffect's
		 *	type. (i.e. EventEffectFactory::thisFactory has already been called and returned true.)
		 *	If sub-classes of EventEffectFactory introduce *new* EventEffect parameters, then the
		 *	sub-class should override this method but explicitly call the parent class's
		 *	version.
		 *
		 *	@param		effect		A pointer to the effect whose attributes are to be set.
		 *	@param		node		The XML node containing the event effect attributes.
		 *	@param		behaveFldr	The path to the behavior file.  If the action references
		 *							resources in the file system, it should be defined relative
		 *							to the behavior file location.  This is the folder containing
		 *							that path.
		 *	@returns	A boolean reporting success (true) or failure (false).
		 */
		virtual bool setFromXML(EventEffect * effect, TiXmlElement * node,
			const std::string & behaveFldr) const override;

	};
}	// namespace Menge
