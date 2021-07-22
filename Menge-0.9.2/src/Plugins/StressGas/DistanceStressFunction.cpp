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
		// natural cool down
		_stressLevel -= _coolDownRate * SIM_TIME_STEP;

		// increase to distance if greater
		float target = 0.f;
		//下面判断agent是否在regions里面
		map <size_t, Geometry2D*> ::iterator it;
		it = _regions.begin();
		Geometry2D* region = it->second;
		while (it != _regions.end()) {
			region = it->second;
			if (region->containsPoint(_agent->_pos)) {
                if (Menge::WherePeopleIs.count(_agent) == 0) {
                    //如果检测到区域中有这个人，但是人与出口的映射map中不存在这个人，
                    //说明这个人刚刚进去该区域，此时在该区域的人数中++，并在 人与出口的映射map 中加入该人对应的出口编号
                    Menge::WherePeopleIs.insert(std::pair<Menge::Agents::BaseAgent*, size_t>(_agent, it->first));
                    int num = Menge::ExitPeopleCount.find(region)->second.num;
                    Menge::ExitPeopleCount.insert(std::pair<Geometry2D*, Menge::ExitPeople>(region, Menge::ExitPeople(num+1, 0)));
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
			else{
			    //如果此时该区域中没有这个人，得判断此人是否刚刚从该区域走出来
			    //如果人与出口的映射map中存在这个人，并且value等于该区域的id，说明这个人进去过该区域
			    //此时他正好走出这个区域，该区域人数--，并在 人与出口的映射map 中将该人对应的出口编号置0
                if (Menge::WherePeopleIs.count(_agent) == 0 && Menge::WherePeopleIs.find(_agent)->second == it->first) {
                    Menge::WherePeopleIs.find(_agent)->second = 0;
                    int num = Menge::ExitPeopleCount.find(region)->second.num;
                    Menge::ExitPeopleCount.insert(std::pair<Geometry2D*, Menge::ExitPeople>(region, Menge::ExitPeople(num-1, 0)));
                }
			}
			it++;
		}
		return 0;
		/*
		float d = sqrt( region->squaredDistance( _agent->_pos ) );
		if ( d > _outer ) target = 0.f;
		else if ( d < _inner ) target = 1.f;
		else {
			target = 1.f - ( d - _inner ) / ( _outer - _inner );
			switch ( _funcType ) {
				case LINEAR:
					break;
				case QUADRATIC:
					target *= target;
			}
		}
		
		_stressLevel = ( target > _stressLevel ) ? target : _stressLevel;
		return _stressLevel;
		*/
	}

} // namespace StressGAS
