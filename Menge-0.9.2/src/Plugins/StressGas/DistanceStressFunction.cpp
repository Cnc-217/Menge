#include "DistanceStressFunction.h"
#include "AgentStressor.h"

#include <map>
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/Geometry2D.h"

namespace StressGAS {

	using Menge::SIM_TIME_STEP;
	using Menge::Agents::BaseAgent;
	using Menge::Math::Geometry2D;
	using std::map;

	/////////////////////////////////////////////////////////////////////
	//                   Implementation of DistanceStressFunction
	/////////////////////////////////////////////////////////////////////
	/*
	DistanceStressFunction::DistanceStressFunction( Geometry2D const * const region,
													float inner, float outer, 
													StressInterpEnum funcType,
													BaseAgent * agent, 
													AgentStressor * stressor,
													float coolDuration ) :
													StressFunction( agent, stressor, coolDuration ),
													_region(region), _inner(inner), _outer(outer),
													_funcType(funcType) {}
	*/

	DistanceStressFunction::DistanceStressFunction(map<size_t, Geometry2D*> regions,
													float inner, float outer, 
													StressInterpEnum funcType,
													BaseAgent * agent, 
													AgentStressor * stressor,
													float coolDuration ) :
													StressFunction( agent, stressor, coolDuration ),
													_regions(regions), _inner(inner), _outer(outer),
													_funcType(funcType){}

	/////////////////////////////////////////////////////////////////////

	float DistanceStressFunction::updateStress() {

		//if (Menge::ThemePark::evacuationState == true) {
			// natural cool down
			_stressLevel -= _coolDownRate * SIM_TIME_STEP;
			// increase to distance if greater
			float target = 0.f;
			//下面判断agent是否在regions里面
			map <size_t, Geometry2D*> ::iterator it;
			it = _regions.begin();
			Geometry2D* region = it->second;
			//遍历每一个区域
			while (it != _regions.end()) {
				region = it->second;
				if (region->containsPoint(_agent->_pos)) {//这里的agent不是所有的agent，是在某个state中的一个agent

					//vector=1000表示agent之前不在region里，现在进入了
					if (Menge::BaseScene::ExitAgentInfo[_agent->_id] == 1000) {
						Menge::BaseScene::ExitAgentInfo[_agent->_id] = it->first;
						//cout << "agentState" << Menge::Evacuation::ExitAgentInfo[_agent->_id] << endl;
						Menge::BaseScene::ExitReagionInfo[it->first] = Menge::BaseScene::ExitReagionInfo[it->first] + 1;
						cout << "agentID: " << _agent->_id << " reagionID: " << it->first << " population: " << Menge::BaseScene::ExitReagionInfo[it->first] << endl;;
					}

					float d = sqrt(region->squaredDistance(_agent->_pos));
					if (d > _outer) target = 0.f;
					else if (d < _inner) target = 1.f;
					else {
						target = 1.f - (d - _inner) / (_outer - _inner);
						switch (_funcType) {
						case LINEAR:
							break;
						case QUADRATIC:
							target *= target;
						}
					}

					_stressLevel = (target > _stressLevel) ? target : _stressLevel;
					return _stressLevel;
				}
				else {
					//agent之前在此区域内，现在出去了
					if (Menge::BaseScene::ExitAgentInfo[_agent->_id] == it->first) {
						Menge::BaseScene::ExitAgentInfo[_agent->_id] = 1000;
						Menge::BaseScene::ExitReagionInfo[it->first] = Menge::BaseScene::ExitReagionInfo[it->first] - 1;
					}

				}
				it++;
			}
			return 0;
		//}
		//else return 0;
		
	}

} // namespace StressGAS
