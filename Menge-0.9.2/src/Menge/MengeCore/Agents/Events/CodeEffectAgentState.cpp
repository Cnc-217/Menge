#include "MengeCore/Agents/Events/CodeEffectAgentState.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/Events/EventException.h"
#include "MengeCore/Agents/StateSelectors/StateSelectorDatabase.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Core.h"
#include <cstdlib>
#include <ctime>

namespace Menge {

	using Agents::BaseAgent;
	using Agents::StateSelectorDB;
	using BFSM::State;
	using std::string;

	/////////////////////////////////////////////////////////////////////
	//					Implementation of CodeEffectAgentState
	/////////////////////////////////////////////////////////////////////

	CodeEffectAgentState::CodeEffectAgentState() : AgentEventEffect() {}

	/////////////////////////////////////////////////////////////////////

	CodeEffectAgentState::~CodeEffectAgentState() {
		
	}

	/////////////////////////////////////////////////////////////////////

	void CodeEffectAgentState::agentEffect(BaseAgent * agent) {

		if (PROJECTNAME ==BUSINESS || PROJECTNAME == BUSINESSLEARNING) {
			//自己定义的方法给agent指定下一个state
			//例如：无优惠券的agent（当前state=0~11），指定其下一步转移到特定state（优惠券state的入口）
			State * currState = ACTIVE_FSM->getCurrentState(agent);
			size_t tmp = currState->getID();

			//按照我们的逻辑，一定不会出现agent已经在优惠state的时候重新进入优惠state（tmp<12）
			//方案1：agent收到优惠券，重新选择当前类型state的另一家店铺，eat仍然是eat，但店铺变化了
			//于是引入agent接收优惠券的概率，暂时设置为0.5
			//if( tmp<12 && 0.5 >(rand() % 100) * 0.01 ){
			if (tmp<12) {
				State* nextState = ACTIVE_FSM->getNode(tmp + 12);
				currState->leave(agent);
				nextState->enter(agent);
				ACTIVE_FSM->setCurrentState(agent, nextState->getID());
			}
		}
		
		else if (PROJECTNAME == BUSINESSREALITY) {
			//do nothing
		}
		
	}


	

	/////////////////////////////////////////////////////////////////////
	//					Implementation of EventEffectAgentStateFactory
	/////////////////////////////////////////////////////////////////////

	CodeEffectAgentStateFactory::CodeEffectAgentStateFactory() : EventEffectFactory() {
		
	}

	/////////////////////////////////////////////////////////////////////
	
	bool CodeEffectAgentStateFactory::setFromXML(EventEffect * effect, TiXmlElement * node,
		const std::string & behaveFldr) const {
		CodeEffectAgentState* aEffect = dynamic_cast<CodeEffectAgentState* >(effect);
		assert(aEffect != 0x0 && "Trying to set agent state event effect on an "
			"incompatible object");

		// This parses the target
		if (!EventEffectFactory::setFromXML(aEffect, node, behaveFldr)) return false;

		// Extract the StateSelector from the XML.
		auto selectorNode = node->FirstChildElement("StateSelector");
		assert(selectorNode != nullptr && "The 'set_agent_state' event effect requries a "
			"StateSelector child.");
		
		
		return true;
	}

}	// namespace Menge
