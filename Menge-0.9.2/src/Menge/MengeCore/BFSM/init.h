#ifndef TEST_H
#define TEST_H
#include<stdio.h>
#include<string>
#include<MengeCore/Socket.h>
#include<thread>
#include<map>
#include<vector>
#include "MengeCore/Scene/BaseScene.h"
#include<Menge/MengeCore/FileTool.h>
#include<Menge/MengeCore/MatrixMy.h>
#include"Menge/MengeCore/Math/Geometry2D.h"

using namespace std;
namespace Menge
{
	//初始化路基类
	class initRoadAndShopBase
	{
	public:
		std::map<int, Olympic::Shoptype> shopinfo2;
		std::vector <Olympic::roadRegionType> roadRegionInfo2;
		virtual void init(string roadRegionDir) = 0;
	protected:
	};
	//继承基类  重写init函数
	class initRoadAndShop:public initRoadAndShopBase
	{
	public:

		void init(string roadRegionDir);
	};
	//继承基类  重写init函数
	class initRoadAndShopDefault :public initRoadAndShopBase
	{
	public:

		void init(string roadRegionDir);
	};

	//初始化概率矩阵基类
	class initProbMatrixBase
	{
	public:
		virtual void init(int goalSetSize) = 0;
	protected:
	};
	//继承基类  重写init函数  config为1时
	class initProbMatrix:public initProbMatrixBase
	{
	public:
		void init(int goalSetSize);
	};
	//config为0时
	class initProbMatrixDefault :public initProbMatrixBase
	{
	public:
		void init(int goalSetSize);
	};

	//初始化socket基类
	class initSocketBase
	{
	public:
		virtual void init() = 0;
	protected:
	};
	//初始化socket  继承基类  defualt方法
	class initSocketDefault :public initSocketBase
	{
	public:
		void init();
	};
	//初始化socket  继承基类  method方法
	class initSocketMethod :public initSocketBase
	{
	public:
		int method;
		string ip;
		int port;
		void init();
	};

	/*定义一个基类SCENE
	私有变量有三个分别是三个初始化  由类指针传值
	有三个方法  分别为私有类指针的方法*/
	class Scene
	{
	public:
		Scene(initRoadAndShopBase* p_initRoadAndShop, initSocketBase* p_initSocket, initProbMatrixBase* p_initProbMatrix)
		{
			pinitRoadAndShop = p_initRoadAndShop;
			pinitSocket = p_initSocket;
			pinitProbMatrix = p_initProbMatrix;
			shopInfo = &(p_initRoadAndShop->shopinfo2);
			roadRegionInfo = &(p_initRoadAndShop->roadRegionInfo2);
		}
		void init_RoadandShop(string roadRegionDir);
		void init_Socket();
		void init_ProbMatrix(int goalSetSize);
		void init_all(string roadRegionDir, int goalSetSize);
		map<int, Olympic::Shoptype>* shopInfo;
		std::vector<Olympic::roadRegionType>* roadRegionInfo;

	private:
		initRoadAndShopBase* pinitRoadAndShop;
		initSocketBase* pinitSocket;
		initProbMatrixBase* pinitProbMatrix;
	};
}
#endif