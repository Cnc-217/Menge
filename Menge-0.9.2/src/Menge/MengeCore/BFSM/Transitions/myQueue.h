#pragma once
/*
���̵Ľṹ��{
�����������������������еĳ���
���ͣ���  ��  ��  ->��������ʱ��
������У�
�������У�
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
	LQueue();//���캯��
	~LQueue();//��������
	void enqueue(T item);//���
	void dequeue();//����
	unsigned int size() { return cnt; }//��ȡ���е�ǰԪ�ظ���
	bool isempty() { return cnt == 0 ? true : false; }//�ж϶����Ƿ�Ϊ��
	T getHead();//��ȡ��ͷ��Ԫ��
};

