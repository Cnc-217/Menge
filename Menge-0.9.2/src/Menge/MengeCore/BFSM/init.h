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
	//��ʼ��·����
	class initRoadAndShopBase
	{
	public:
		std::map<int, Olympic::Shoptype> shopinfo2;
		std::vector <Olympic::roadRegionType> roadRegionInfo2;
		virtual void init(string roadRegionDir) = 0;
	protected:
	};
	//�̳л���  ��дinit����
	class initRoadAndShop:public initRoadAndShopBase
	{
	public:

		void init(string roadRegionDir);
	};
	//�̳л���  ��дinit����
	class initRoadAndShopDefault :public initRoadAndShopBase
	{
	public:

		void init(string roadRegionDir);
	};

	//��ʼ�����ʾ������
	class initProbMatrixBase
	{
	public:
		virtual void init(int goalSetSize) = 0;
	protected:
	};
	//�̳л���  ��дinit����  configΪ1ʱ
	class initProbMatrix:public initProbMatrixBase
	{
	public:
		void init(int goalSetSize);
	};
	//configΪ0ʱ
	class initProbMatrixDefault :public initProbMatrixBase
	{
	public:
		void init(int goalSetSize);
	};

	//��ʼ��socket����
	class initSocketBase
	{
	public:
		virtual void init() = 0;
	protected:
	};
	//��ʼ��socket  �̳л���  defualt����
	class initSocketDefault :public initSocketBase
	{
	public:
		void init();
	};
	//��ʼ��socket  �̳л���  method����
	class initSocketMethod :public initSocketBase
	{
	public:
		int method;
		string ip;
		int port;
		void init();
	};

	/*����һ������SCENE
	˽�б����������ֱ���������ʼ��  ����ָ�봫ֵ
	����������  �ֱ�Ϊ˽����ָ��ķ���*/
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