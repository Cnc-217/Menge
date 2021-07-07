#include "DistanceStressFunction.h"
#include "AgentStressor.h"

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
