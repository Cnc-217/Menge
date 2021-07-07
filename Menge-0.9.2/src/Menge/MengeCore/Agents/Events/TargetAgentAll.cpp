#include "MengeCore/Agents/Events/TargetAgentAll.h"

#include "MengeCore/Core.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Runtime/Logger.h"

namespace Menge {

	/////////////////////////////////////////////////////////////////////
	//					Implementation of TargetAgentAll
	/////////////////////////////////////////////////////////////////////

	void TargetAgentAll::update() {
		_elements.clear();
		const size_t AGENT_COUNT = SIMULATOR->getNumAgents();
		for (size_t i = 0; i < AGENT_COUNT; ++i) {
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			_elements.push_back(agent);
		}
		AgentEventTarget::update();
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of TargetAgentAllFactory
	/////////////////////////////////////////////////////////////////////

	TargetAgentAllFactory::TargetAgentAllFactory() : AgentEventTargetFactory() {

	}

	/////////////////////////////////////////////////////////////////////

	bool TargetAgentAllFactory::setFromXML(EventTarget * target, TiXmlElement * node,
		const std::string & specFldr) const {
		TargetAgentAll * idTarget = dynamic_cast< TargetAgentAll * >(target);
		assert(idTarget != 0x0 && "Trying to set attributes of a target-by-id event target "
			"on an incompatible object");

		if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

		return true;
	}

}	// namespace Menge
