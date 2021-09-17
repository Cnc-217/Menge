#include "DistanceStressAction.h"

#include <algorithm>

#include "DistanceStressFunction.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/Geometry2D.h"
#include "MengeCore/Math/RandGenerator.h"

namespace StressGAS {

	using namespace std;
	using Menge::logger;
	using Menge::Logger;
	using Menge::Agents::BaseAgent;
	using Menge::BFSM::Action;
	using Menge::Math::FloatGenerator;
	using Menge::Math::Geometry2D;
	using Menge::Math::createGeometry;
	using Menge::Math::createGeometrys;

	/////////////////////////////////////////////////////////////////////
	//                   Implementation of DistanceStressAction
	/////////////////////////////////////////////////////////////////////

	StressFunction * DistanceStressAction::makeStressFunction( BaseAgent * agent,
															AgentStressor * stressor,
															float coolDuration )
	{
		//return new DistanceStressFunction( _geometry, _innerDist->getValue(), 
		//								   _outerDist->getValue(), _func, agent, stressor, 
		//								   _coolDuration->getValue() );//修改构造函数
		return new DistanceStressFunction( _geometrys, _innerDist->getValue(), 
										   _outerDist->getValue(), _func, agent, stressor, 
										   _coolDuration->getValue() );//修改构造函数
	}

	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//                   Implementation of DistanceStressActionFactory
	/////////////////////////////////////////////////////////////////////

	DistanceStressActionFactory::DistanceStressActionFactory() {
		_outerId = _attrSet.addFloatDistAttribute( "outer_", true, 10.f, 1.f );
		_innerId = _attrSet.addFloatDistAttribute( "inner_", true, 10.f, 1.f );
		_funcId = _attrSet.addStringAttribute( "fall_off", false, "linear" );
		_shapeNumber = _attrSet.addSizeTAttribute("shape_number", true/*required*/, 0/*default*/);
	}

	/////////////////////////////////////////////////////////////////////
	/* 格式：
	<Action type="distance_stress" exit_behavior="cool down"
		inner_dist = "c" inner_value = "100"
		outer_dist = "c" outer_value = "120"
		fall_off = "linear"
		shape_number="3" max_x1 = "3700" min_x1 = "3363" max_y1 = "2178" min_y1 = "1658"
		max_x2 = "3700" min_x2 = "3363" max_y2 = "2178" min_y2 = "1658"
		max_x3 = "3700" min_x3 = "3363" max_y3 = "2178" min_y3 = "1658"
		/ >
	*/
	bool DistanceStressActionFactory::setFromXML( Action * action, TiXmlElement * node,
											   const std::string & behaveFldr ) const {
		DistanceStressAction * dAction = dynamic_cast< DistanceStressAction * >( action );
		assert( dAction != 0x0 &&
				"Trying to set distance stress action properties on an incompatible object" );
		if ( !BaseStressActionFactory::setFromXML( action, node, behaveFldr ) ) {
			return false;
		}

		// problems with reading the geometry should be logged by createGeoemetry
		//Geometry2D * geometry = createGeometry( node );//这个位置需要修改，改成有一个Geometry map，存储多个Geometry；方法也改成createGeometrys,返回map类型
		map<size_t, Geometry2D*> geometrys = createGeometrys(node, _attrSet.getSizeT(_shapeNumber));//新功能
		
		//if ( geometry == 0x0 ) return false;
		//if ( dAction->_geometry != 0x0 ) delete dAction->_geometry;
		//dAction->_geometry = geometry;
		if (geometrys.empty()) return false;
		if ( !dAction->_geometrys.empty() ) dAction->_geometrys.clear();
		dAction->_geometrys = geometrys;
		//cout << "set geometrys map ok" << endl;

		if ( dAction->_innerDist != 0x0 ) delete dAction->_innerDist;
		dAction->_innerDist = _attrSet.getFloatGenerator( _innerId );

		if ( dAction->_outerDist != 0x0 ) delete dAction->_outerDist;
		dAction->_outerDist = _attrSet.getFloatGenerator( _outerId );

		std::string func = _attrSet.getString( _funcId );
		std::transform( func.begin(), func.end(), func.begin(), ::tolower );

		// exit behavior
		if ( func == "linear" ) dAction->_func = DistanceStressFunction::LINEAR;
		else if ( func == "quadratic" ) dAction->_func = DistanceStressFunction::QUADRATIC;
		else {
			logger << Logger::WARN_MSG << "Parameter \"fall_off\" has an unrecognizable " \
				"value: " << func << ".  Should be one of: linear or quadratic.  Default to " \
				"linear.";
			dAction->_func = DistanceStressFunction::LINEAR;
		}

		return true;
	}
}	// namespace StressGAS
