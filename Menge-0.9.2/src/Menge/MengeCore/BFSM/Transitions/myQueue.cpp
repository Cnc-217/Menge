#include "MengeCore/BFSM/Transitions/myQueue.h"
template<typename T>
LQueue<T>::LQueue()
{
	cnt = 0;
	Node<T>* anode = new Node<T>;
	anode->next = NULL;
	anode->prev = NULL;
	head = anode;
	rear = anode;
}
template<typename T>
LQueue<T>::~LQueue()
{
	Node<T>* p = head;
	for (int i = 0; i < cnt; i++)
	{
		dequeue();
	}
}
template<typename T>
void LQueue<T>::enqueue(T item)
{
	if (cnt == 0)
	{
		rear->item = item;
		cnt++;
	}
	else
	{
		Node<T>* anode = new Node<T>;
		anode->item = item;
		anode->next = rear;
		rear->prev = anode;
		rear = anode;
		rear->prev = NULL;
		cnt++;
	}
}
template<typename T>
void LQueue<T>::dequeue()
{
	if (isempty())
	{
		cout << "Error: This LQueue is empty" << endl;
		cout << "File Path =" << __FILE__ << endl;
		cout << "Function Name =" << __FUNCTION__ << endl;
		cout << "Line =" << __LINE__ << endl;
	}
	Node<T>* p = head;
	head = p->prev;
	delete p;
	cnt--;
}
template<typename T>
T LQueue<T>::getHead()
{
	return head->item;
}