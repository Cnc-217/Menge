#include<MengeCore/BFSM/SceneFactory.h>

namespace Menge
{
	Scene* SceneFactory::creatscene(string dir)
	{
		initRoadAndShopBase* roadTemp = nullptr;
		initSocketBase* socketTemp = nullptr;
		initProbMatrixBase* probTemp = nullptr;
		vector<bool>configVector = FileTool::testAllConfigOpen(dir);
		if (configVector[0])
			roadTemp = new initRoadAndShop();
		else
			roadTemp = new initRoadAndShopDefault();

		if (configVector[1])
			socketTemp = new initSocketMethod();
		else 
			socketTemp = new initSocketDefault();

		if (configVector[2])
			probTemp = new initProbMatrix();
		else
			probTemp = new initProbMatrixDefault();

		if( roadTemp != nullptr && socketTemp != nullptr && probTemp != nullptr)
			return new Scene(roadTemp,socketTemp,probTemp);
	}
}

