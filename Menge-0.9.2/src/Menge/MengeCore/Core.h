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
#include <string>

#define BUSINESS 1
#define EVACUATION 2
#define BUSINESSREALITY 3
#define BUSINESSLEARNING 4

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

}	// namespace Menge



namespace MengeVis {

	// forward declarations
	namespace Viewer {
		class GLViewer;
	}

	extern MENGE_API Viewer::GLViewer* SimViewer;

}	// namespace MengeVis

#endif	//__CORE_H__
