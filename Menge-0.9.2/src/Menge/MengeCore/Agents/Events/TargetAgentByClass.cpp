#include "MengeCore/Agents/Events/TargetAgentByClass.h"

#include "MengeCore/Core.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Runtime/Logger.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/BaseAgent.h"
#include <cstdlib>
#include <ctime>

namespace Menge {

	/////////////////////////////////////////////////////////////////////
	//					Implementation of TargetAgentByClass
	/////////////////////////////////////////////////////////////////////

	void TargetAgentByClass::update() {
		_elements.clear();
		const size_t AGENT_COUNT = SIMULATOR->getNumAgents();
		for (size_t i = 0; i < AGENT_COUNT; ++i) {
			Agents::BaseAgent* agent = SIMULATOR->getAgent(i);
			if (agent->_class == _classId && _probChoose> (rand() % 100) * 0.01) {
				_elements.push_back(agent);
			}
		}
		AgentEventTarget::update();
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of TargetAgentByIdFactory
	/////////////////////////////////////////////////////////////////////

	TargetAgentByClassFactory::TargetAgentByClassFactory() : AgentEventTargetFactory() {
		_classID = _attrSet.addSizeTAttribute("id", true /*required*/);
		_prob = _attrSet.addFloatAttribute("prob", true /*required*/,1.0);//部分人选中

	}

	/////////////////////////////////////////////////////////////////////

	bool TargetAgentByClassFactory::setFromXML(EventTarget * target, TiXmlElement * node,
		const std::string & specFldr) const {
		TargetAgentByClass * idTarget = dynamic_cast<TargetAgentByClass* >(target);
		assert(idTarget != 0x0 && "Trying to set attributes of a target-by-id event target "
			"on an incompatible object");

		if (!AgentEventTargetFactory::setFromXML(target, node, specFldr)) return false;

		idTarget->_classId = _attrSet.getSizeT(_classID);
		idTarget->_probChoose = _attrSet.getFloat(_prob);
		return true;
	}

}	// namespace Menge
