/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *	@file		Core.h
 *	@brief		A set of global variables for use by the entire
 *				finite state machine.
 */

#ifndef __CORE_H__
#define	__CORE_H__

#include "MengeCore/CoreConfig.h"
#include "MengeCore/Math/Geometry2D.h"
#include "MengeCore/Agents/BaseAgent.h"
#include <string>
#include <map>
#include<queue>
#define BUSINESS 1
#define EVACUATION 2
#define BUSINESSREALITY 3
#define BUSINESSLEARNING 4
#define THEMEPARK 5
#define OLYMPIC 6

/*!
 * @namespace Menge
 * @brief	The core namespace.  All elements of Menge are contained in this namespace.
 */
namespace Menge {

	// forward declarations
	namespace Agents {
		class SpatialQuery;
		class SimulatorInterface;
		class Elevation;
	}

	namespace BFSM {
		class FSM;
	}
	
	class MatrixDim2;
	class MatrixDim3;
	class MatrixDim4;
	class EventSystem;

	/*!
	 *	@brief		The fsm running for the simulation.
	 */
	extern MENGE_API BFSM::FSM * ACTIVE_FSM;

	/*!
	 *	@brief		The global simulation time.
	 */
	extern MENGE_API float SIM_TIME;

	/*!
	 *	@brief		The simulation time step.
	 */
	extern MENGE_API float SIM_TIME_STEP;

	/*!
	 *	@brief		The spatial query structure for the simulation.
	 */
	extern MENGE_API Agents::SpatialQuery * SPATIAL_QUERY;

	/*!
	 *	@brief		The elevation structure for the simulation.
	 */
	extern MENGE_API Agents::Elevation * ELEVATION;

	/*!
	 *	@brief		The simulator for use with some plugins that need it
	 */
	extern MENGE_API Agents::SimulatorInterface * SIMULATOR;

	/*!
	 *	@brief		The event system.
	 */
	extern MENGE_API EventSystem * EVENT_SYSTEM;

	extern MENGE_API size_t PROJECTNAME;

	

	namespace Business {
		extern MENGE_API MatrixDim2* ProbStatesNormal;//第一维：顾客类型；第二维：选择state类型；值：概率
		extern MENGE_API MatrixDim2* ProbStatesCoupon;
		extern MENGE_API MatrixDim3* ProbGoalsNormal;//第一维：顾客类型；第二维：当前state类型；第三维：店铺id；值：选择店铺的概率
		extern MENGE_API MatrixDim3* ProbGoalsCoupon;
	}

	namespace BusinessReality {
		extern MENGE_API MatrixDim4* ProbGoals;
		//第一维：顾客类型；第二维：当前的30家店铺；第三维：店铺类型；第四维：店铺id；值：选择店铺的概率
	}


	namespace ThemePark {
		//三类人群的数组
		extern MENGE_API std::vector<Agents::BaseAgent*> leaderAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> panicAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> normalAgentSet;
		extern MENGE_API bool evacuationState; //控制event疏散部分状态启动和关闭
	}

	namespace Olympic {
		//三类人群的数组
		extern MENGE_API std::vector<Agents::BaseAgent*> leaderAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> panicAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> normalAgentSet;
		extern MENGE_API bool evacuationState; //控制event疏散部分状态启动和关闭
		extern MENGE_API struct Shoptype
		{
			int serviceMax;
			int blockMax;
			int type;
			int goalSet;//哪种商店  也就是goalset
			std::queue<int> serviceQ;
			std::queue<int> blockQ;
		};
		extern MENGE_API std::map<int, Shoptype>  shopInfo;
		extern MENGE_API std::map<size_t, int>  goalSetInfo;//储存goalset内有多少个goal
		extern MENGE_API std::map< size_t, bool >	_reachedAgents;//存储agent是否到达目标点
		extern MENGE_API std::map<size_t, int > nowReachTimes;//储存agent在当前goalset的不同goal的次数
	}

	namespace BaseScene {
		extern MENGE_API MatrixDim2* ProbMatrix;//通用场景下输入文件的矩阵

		//通用场景下人流量检测
		extern MENGE_API   std::vector<size_t> ExitReagionInfo;//exitRegionID-population

		extern MENGE_API   std::vector<size_t> ExitAgentInfo;//AgentID-AgentState

		extern MENGE_API   std::vector<size_t> ExitReagionCapacity;//exitRegionID-Capacity

		extern MENGE_API   size_t DetectReagionNum;//detect region num

	}



}	// namespace Menge



namespace MengeVis {

	// forward declarations
	namespace Viewer {
		class GLViewer;
	}

	extern MENGE_API Viewer::GLViewer* SimViewer;

}	// namespace MengeVis

#endif	//__CORE_H__
