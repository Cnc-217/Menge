
#include "RoadMapPlusConfig.h"
#include "VelComponentPlus.h"


#include <sstream>
#include <iomanip>



namespace ROAD {
	using Menge::PathMap;
	using Menge::Logger;
	using Menge::logger;
	using Menge::Agents::BaseAgent;
	using Menge::Agents::PrefVelocity;
	using Menge::BFSM::Goal;
	using Menge::BFSM::VelCompFactory;
	using Menge::BFSM::VelComponent;
	using Menge::Math::Vector2;
	using Menge::BFSM::VelCompFatalException;
	using Menge::ResourceException;

	/////////////////////////////////////////////////////////////////////
		//                   Implementation of RoadMapVelComponentPlus
		/////////////////////////////////////////////////////////////////////

	const std::string RoadMapVelComponentPlus::NAME = "road_map_plus";

	/////////////////////////////////////////////////////////////////////

	RoadMapVelComponentPlus::RoadMapVelComponentPlus() :VelComponent(), _roadmap(0x0) {
	}

	/////////////////////////////////////////////////////////////////////

	RoadMapVelComponentPlus::RoadMapVelComponentPlus(const GraphPtr& graph) : VelComponent(),
		_roadmap(graph) {
	}

	/////////////////////////////////////////////////////////////////////

	RoadMapVelComponentPlus::~RoadMapVelComponentPlus() {
		PathMap::iterator itr = _paths.begin();
		for (; itr != _paths.end(); ++itr) {
			delete itr->second;
		}
		_paths.clear();
	}

	/////////////////////////////////////////////////////////////////////

	void RoadMapVelComponentPlus::onExit(BaseAgent* agent) {
		// This test is necessary.  Because state advancement can jump MULTIPLE states in a
		//	singletime step, it is possible to enter and exit a state with a roadmap velocity
		//	without ever actually calling "setPrefVelocity" on that velocity component.
		//
		//	Roadmap initializes the path in setPrefVelocity - so, things don't get properly
		//	initialized.
		_lock.lockWrite();
		PathMap::iterator itr = _paths.find(agent->_id);
		if (itr != _paths.end()) {
			delete itr->second;
			_paths.erase(agent->_id);
		}
		_lock.releaseWrite();
	}

	/////////////////////////////////////////////////////////////////////

	void RoadMapVelComponentPlus::setPrefVelocity(const BaseAgent* agent,
		const Goal* goal,
		Menge::Agents::PrefVelocity& pVel) const {
		_lock.lockRead();
		PathMap::const_iterator itr = _paths.find(agent->_id);
		Menge::RoadMapPath* path = 0x0;
		if (itr == _paths.end()) {
			_lock.releaseRead();
			// compute the path and add it to the map
			//	Create path for the agent
			Vector2 goalPoint = goal->getCentroid();
			path = _roadmap->getPath(agent, goal);
			_lock.lockWrite();
			_paths[agent->_id] = path;
			_lock.releaseWrite();
		}
		else {
			path = itr->second;
			_lock.releaseRead();
		}
		pVel.setSpeed(agent->_prefSpeed);
		if (!path->setPrefDirection(agent, pVel)) {
			// Path got lost; replan and retry. If that couldn't produce a trajectory it
			// becomes an irrecoverable error.
			delete path;
			Vector2 goalPoint = goal->getCentroid();
			path = _roadmap->getPath(agent, goal);
			// While this operation doesn't change the structure of the map (agent->_id is already a key),
			// we lock it to prevent any *other* write operation from interfering.
			_lock.lockWrite();
			_paths[agent->_id] = path;
			_lock.releaseWrite();
			if (!path->setPrefDirection(agent, pVel)) {
				throw VelCompFatalException(
					"Agent " + std::to_string(agent->_id) +
					" following a roadmap path could *not* set preferred velocity");
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	//                   Implementation of VelComponetPlusFactory
	/////////////////////////////////////////////////////////////////////

	VelComponetPlusFactory::VelComponetPlusFactory() : VelCompFactory() {
		_fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
	}

	/////////////////////////////////////////////////////////////////////

	bool VelComponetPlusFactory::setFromXML(VelComponent* vc, TiXmlElement* node,
		const std::string& behaveFldr) const {
		RoadMapVelComponentPlus* rmvcp = dynamic_cast<RoadMapVelComponentPlus*>(vc);
		assert(rmvcp != 0x0 &&
			"Trying to set attributes of a road map velocity component on an incompatible "
			"object");
		if (!VelCompFactory::setFromXML(rmvcp, node, behaveFldr)) return false;
		// get the file name
		std::string fName;
		std::string path = Menge::os::path::join(2, behaveFldr.c_str(),
			_attrSet.getString(_fileNameID).c_str());
		Menge::os::path::absPath(path, fName);
		GraphPtr gPtr;
		try {
			gPtr = loadGraph(fName); //fName是graph.txt文件的路径
		}
		catch (ResourceException) {
			logger << Logger::ERR_MSG;
			logger << "Couldn't instantiate the road map referenced on line ";
			logger << node->Row() << ".";
			return false;
		}
		rmvcp->setRoadMap(gPtr);

		return true;
	}




}	// namespace ROAD
