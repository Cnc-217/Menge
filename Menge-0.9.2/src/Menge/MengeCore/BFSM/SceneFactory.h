#ifndef FACTOR_H
#define FACTOR_H

#include<MengeCore/BFSM/init.h>
#include<Menge/MengeCore/FileTool.h>
namespace Menge
{
	class SceneFactory
	{
	public:
		Scene* creatscene(string dir);
	private:
	};

}
enum SCENE
{
	BASESCENE,
	OLYMPICSCENE
};

#endif


