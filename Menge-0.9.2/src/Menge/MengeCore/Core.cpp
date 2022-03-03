/*

License

Menge
Copyright ?and trademark ?2012-14 University of North Carolina at Chapel Hill. 
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation 
for educational, research, and non-profit purposes, without fee, and without a 
written agreement is hereby granted, provided that the above copyright notice, 
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North 
Carolina at Chapel Hill. The software program and documentation are supplied "as is," 
without any accompanying services from the University of North Carolina at Chapel 
Hill or the authors. The University of North Carolina at Chapel Hill and the 
authors do not warrant that the operation of the program will be uninterrupted 
or error-free. The end-user understands that the program was developed for research 
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE 
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY 
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY 
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND 
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include <map>
#include <MengeCore/Math/Geometry2D.h>
#include "MengeCore/Core.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/MatrixMy.h"
#include "MengeCore/Math/Geometry2D.h"

namespace Menge {

	BFSM::FSM * ACTIVE_FSM = 0x0;

	Scene* ACTIVE_SCENE = 0x0;

	float SIM_TIME = 0.f;

	float SIM_TIME_STEP = 0.f;

	Agents::SpatialQuery * SPATIAL_QUERY = 0x0;

	Agents::Elevation * ELEVATION = 0x0;

	Agents::SimulatorInterface * SIMULATOR = 0x0;

	Menge::EventSystem * EVENT_SYSTEM = new Menge::EventSystem();

	Menge::Graph* GRAPH = 0x0;

	size_t PROJECTNAME = 0;

	std::string BehaveFilePath;

	std::string SceneFilePath;

	std::string DirectoryPath;

	namespace Olympic {
		std::vector<Agents::BaseAgent*> leaderAgentSet;
		std::vector<Agents::BaseAgent*> panicAgentSet;
		std::vector<Agents::BaseAgent*> normalAgentSet;
		bool parallelState = false; //是否是平行模式
		bool evacuationState = false;//是否是疏散模式
		//std::map<int, Shoptype>  shopInfo;//已经转移到scene类中
		std::vector<bool>   verticesCanGo;//
		std::string goalSeclectorType;
		vector<float> Influence;//如果是model类型的goalselector，需要用到Influence
		//std::vector <roadRegionType> roadRegionInfo;//着手开始转移
		std::vector<int>agentGoingShop;//agent要去的店铺
		std::vector<int> agentInWhichRegion;//agent在哪个区域

		std::float_t startSimTime = 0;
		size_t AGENT_NUM;
		std::vector<float_t> AGENT_SCORES;
		std::vector<int> AGENT_GOALS;

	}

	namespace BaseScene {
		MatrixDim2* ProbMatrix = 0x0;//如果是matrix类型的goalselector，需要用到ProbMatrix

		size_t DetectReagionNum = 0;

	}

}
namespace MengeVis {

	Viewer::GLViewer* SimViewer = 0x0;

}
