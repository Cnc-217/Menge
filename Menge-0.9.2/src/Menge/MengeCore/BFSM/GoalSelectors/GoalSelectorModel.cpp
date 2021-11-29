

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorModel.h"

#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"

#include "MengeCore/Json/json.hpp"
//#include "MengeCore/Http/http_client.hpp"

#include <cassert>
#include <mutex>

using namespace nlohmann;
using namespace std;

namespace Menge {

	namespace BFSM {
		
		/////////////////////////////////////////////////////////////////////
		//                   Implementation of ModelGoalSelector
		/////////////////////////////////////////////////////////////////////
		
		ModelGoalSelector::ModelGoalSelector() {
			client = Socket::socketClientInit("127.0.0.1", 5001);
			Menge::Olympic::goalSeclectorType = "Model";
		}

		Goal * ModelGoalSelector::getGoal( const Agents::BaseAgent * agent ) const {
			
			if (!agent->_shopGone.empty()) {
				
				json j;
				j["data"] = agent->_shopGone;
				string sendBuf = j.dump();
				send(client, sendBuf.c_str(), strlen(sendBuf.c_str()), 0);
				char receiveBuf[1024] = {};
				recv(client, receiveBuf, 1024, 0);
				
				//json½âÎö
				j.clear();
				j = json::parse(receiveBuf);
				vector<float> probList = j["data"];
				for (int i = 0; i < probList.size(); i++) probList[i] += Olympic::Influence[i];
				

				return _goalSet->getGoalFromProbs(agent, probList);
			}
			
			return _goalSet->getGoalByID(34);

		}
		
	}	// namespace BFSM
}	// namespace Menge
