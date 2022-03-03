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
#include <queue>
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
	class Graph;
	class Scene;

	/*!
	 *	@brief		The fsm running for the simulation.
	 */
	extern MENGE_API BFSM::FSM * ACTIVE_FSM;

	extern MENGE_API Scene* ACTIVE_SCENE;

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

	extern MENGE_API Graph* GRAPH;

	extern MENGE_API size_t PROJECTNAME;

	extern MENGE_API std::string BehaveFilePath;

	extern MENGE_API std::string SceneFilePath;

	extern MENGE_API std::string DirectoryPath;

	namespace Olympic {
		//������Ⱥ������
		extern MENGE_API std::vector<Agents::BaseAgent*> leaderAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> panicAgentSet;
		extern MENGE_API std::vector<Agents::BaseAgent*> normalAgentSet;

		extern MENGE_API bool evacuationState; //����event��ɢ����״̬�����͹ر�
		extern MENGE_API bool parallelState; //�Ƿ���ƽ��ģʽ
		struct Shoptype{
			int serviceMax;
			int blockMax;
			int type;
			std::queue<int> serviceQ;
			std::queue<int> blockQ;
		};
		extern MENGE_API std::vector<bool>   verticesCanGo;//·�ϣ�falseΪ����ͨ����Ĭ��ȫΪtrue
		extern MENGE_API std::string goalSeclectorType; //�洢goalSelector�����ͣ�reality��model��
		extern MENGE_API std::vector<float> Influence;//Olympic��model���͵�goalselector��Ҫ�õ���Ӱ������
		struct roadRegionType {
			Menge::Math::OBBShape obbRoadbRegion;//һ����״  ����ת����
			int peopleNumInThisRoad;//������¼��������ڵ�����
			int capacity;//�����������������
		};//����һ�����ݽṹ  ������ʾͳ������������
		extern MENGE_API std::vector<int> agentInWhichRegion;
		extern MENGE_API std::vector<int> agentGoingShop;
		
		extern MENGE_API std::float_t startSimTime;
		extern MENGE_API size_t AGENT_NUM;
		extern MENGE_API std::vector<float_t> AGENT_SCORES;
		extern MENGE_API std::vector<int> AGENT_GOALS;

	}

	namespace BaseScene {
		extern MENGE_API MatrixDim2* ProbMatrix;//ͨ�ó����������ļ��ľ���

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
