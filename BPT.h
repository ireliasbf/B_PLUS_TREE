#pragma once
#include <queue>
#include "BPTNode.h"
template<class T>
class BPlusTree {
public:
	BPlusTree(int order = 3);//构造函数
	bool insert(T data);//插入函数
	bool myDelete(T data);//删除函数
	bool find(T data);//查找数据，返回是否找到
	bool setOrder(int n);//设定阶数
	bool isEmpty();//返回B+树是否为空
	void printData();//打印树中所有数据
	void printTree();//按树的结构打印整个树
private:
	BPTNode<T>* Root;
	int order;//阶数，即子树最大数量,关键字最大数量加1，默认为3
	int count;//已有元素数量，初始为0
	BPTNode<T>* getHead();//返回叶节点的第一个节点
	BPTNode<T>* findLoc(T data);//查找函数，返回数据所在位置或应该插入位置
};

template<class T>
inline BPlusTree<T>::BPlusTree(int order)//根据阶数初始化B+树
{
	this->order = order;
	count = 0;
	Root = new BPTNode<T>;
	Root->father = nullptr;
	Root->setorder(order);
}

template<class T>
bool BPlusTree<T>::insert(T data)//找到应该插入数据的节点并插入数据，若数据已经存在则插入失败
{
	BPTNode<T>* tmp = findLoc(data);
	bool flag = tmp->insert(data,Root);
	if (flag) count++;
	if (!flag)std::cout << "插入失败，数据" << data << "己存在！" << endl;
	return flag;
}

template<class T>
bool BPlusTree<T>::myDelete(T data)//找到应该删除数据的节点并删除数据，若数据不存在则删除失败
{
	BPTNode<T>* tmp = findLoc(data);
	bool flag = tmp->myDelete(data,Root);
	if (flag) count--;
	if (!flag)std::cout << "删除失败，找不到数据" << data << "!" << endl;
	return flag;
}

template<class T>
inline bool BPlusTree<T>::find(T data)//在树中查找指定数据，返回查找结果
{
	BPTNode<T>* tmp = findLoc(data);
	return tmp->find(data);
}

template<class T>
bool BPlusTree<T>::setOrder(int n)//重新设定阶数，只有在树为空树时可以进行此操作
{
	if (!isEmpty())
		return false;
	order = n;
	return true;
}

template<class T>
bool BPlusTree<T>::isEmpty()//返回树是否为空
{
	if (count)
		return false;
	return true;
}

template<class T>
inline void BPlusTree<T>::printData()//输出树中所有数据
{
	if (Root->isEmpty()) {
		cout << "Tree is empty!\n";
		return;
	}
	BPTNode<T>* tmp = getHead();
	while (tmp && !tmp->isEmpty()) {
		int len = tmp->key.size();
		std::cout << "(";
		for (int i = 0; i < len; i++) {
			std::cout << tmp->key[i].data << " ";
			if (i == len - 1)
				tmp = tmp->next;	
		}
		std::cout << "\b)->";
	}
	std::cout << "\b\b";
	std::cout << "  " << endl;
}

template<class T>
inline void BPlusTree<T>::printTree()//输出整个树的结构
{
	if (Root->isEmpty()) {
		cout << "Tree is empty!\n";
		return;
	}
	queue<BPTNode<T>*> qu;
	qu.push(Root);
	while (!qu.empty()) {
		int len = qu.size();
		for (int i = 0; i < len; i++) {
			BPTNode<T>* tmp = qu.front();
			qu.pop();
			for (int j = 0; j < tmp->key.size(); j++) {
				if (tmp->key[j].left)
					qu.push(tmp->key[j].left);
				if (j == tmp->key.size() - 1 && tmp->key[j].right)
					qu.push(tmp->key[j].right);
			}
			std::cout << "(";
			for (int j = 0; j < tmp->key.size(); j++) {
				std::cout << tmp->key[j].data << " ";
			}
			std::cout << "\b)->";
		}
		std::cout << "\b\b  " << endl;
	}
}

template<class T>
inline BPTNode<T>* BPlusTree<T>::getHead()//找到树中最小的数据所在结点
{
	BPTNode<T>* tmp = Root;
	while (tmp->key.size() > 0 && tmp->key[0].left)
		tmp = tmp->key[0].left;
	return tmp;
}

template<class T>
BPTNode<T>* BPlusTree<T>::findLoc(T data)//找到指定数据应该插入的结点位置
{
	if (count == 0) return Root;
	BPTNode<T>* tmp = Root;
	if (Root->key[0].left == nullptr) return tmp;
	while (tmp->key[0].left != nullptr) {
		for (int i = 0; i < tmp->key.size(); i++) {
			if (data < tmp->key[i].data) {
				tmp = tmp->key[i].left;
				break;
			}
			if (i == tmp->key.size() - 1) {
				tmp = tmp->key[i].right;
				break;
			}
		}
	}
	return tmp;
}


