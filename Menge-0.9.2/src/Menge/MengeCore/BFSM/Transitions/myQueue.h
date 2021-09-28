#pragma once
/*
店铺的结构体{
店铺最大容量：决定服务队列的长度
类型：吃  买  玩  ->决定服务时间
服务队列：
阻塞队列：
}
*/
/*
#include<queue>
struct Shoptype {
	int type;
	int servicetime;
}Stype;
struct Shop
{
	int Maximum;
	Shoptype Stype;
	queue< int > serviceQ;
	queue< int > blockQ;
};
*/
#pragma once
#include<queue>
using namespace std;
template<typename A>
struct Node
{
	A item;
	Node* next;
	Node* prev;
};
template<typename T>
class LQueue
{
private:
	Node<T>* head;
	Node<T>* rear;
	int cnt;
public:
	LQueue();//构造函数
	~LQueue();//析构函数
	void enqueue(T item);//入队
	void dequeue();//出队
	unsigned int size() { return cnt; }//获取队列当前元素个数
	bool isempty() { return cnt == 0 ? true : false; }//判断队列是否为空
	T getHead();//获取队头的元素
};

