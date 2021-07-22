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
		//�����ж�agent�Ƿ���regions����
		map <size_t, Geometry2D*> ::iterator it;
		it = _regions.begin();
		Geometry2D* region = it->second;
		while (it != _regions.end()) {
			region = it->second;
			if (region->containsPoint(_agent->_pos)) {
                if (Menge::WherePeopleIs.count(_agent) == 0) {
                    //�����⵽������������ˣ�����������ڵ�ӳ��map�в���������ˣ�
                    //˵������˸ոս�ȥ�����򣬴�ʱ�ڸ������������++������ ������ڵ�ӳ��map �м�����˶�Ӧ�ĳ��ڱ��
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
			    //�����ʱ��������û������ˣ����жϴ����Ƿ�ոմӸ������߳���
			    //���������ڵ�ӳ��map�д�������ˣ�����value���ڸ������id��˵������˽�ȥ��������
			    //��ʱ�������߳�������򣬸���������--������ ������ڵ�ӳ��map �н����˶�Ӧ�ĳ��ڱ����0
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
