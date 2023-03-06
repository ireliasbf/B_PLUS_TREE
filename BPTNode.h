#pragma once
#include <vector>
#include <algorithm>
using namespace std;

template <class T>
class BPTNode
{
public:
	template<class T>//关键字结点类
	class keyNode
	{
	public:
		keyNode() {};
		keyNode(const T data) :data(data) {};//构造函数
		keyNode(const T data, BPTNode<T>* left, BPTNode<T>* right) :data(data), left(left), right(right) {};
		T data;//关键字数据
		BPTNode<T>* left = nullptr;//左孩子，叶节点左孩子默认为空
		BPTNode<T>* right = nullptr;//右孩子，叶节点右孩子默认为空
	};
	vector<keyNode<T>> key = {};
	BPTNode<T>* father = nullptr;
	BPTNode<T>* next = nullptr;
	BPTNode();//默认构造函数，默认初始化为值为0的叶结点
	BPTNode(const T data, int order = 3);//根据单一数据和阶数初始化B+树结点
	BPTNode(vector<T>& key, int order = 3);//根据数据数组和阶数初始化B+树结点
	bool find(const T keyValue);//在当前结点查找对应数据是否存在
	bool insert(const T keyValue, BPTNode<T>*& Root);//叶结点专用，插入键值
	bool myDelete(const T keyValue, BPTNode<T>*& Root);//叶结点专用，删除键值
	bool isEmpty();//结点是否为空
	bool setorder(int order);//修改阶数，只有修改后键值数仍小于阶数才可以修改
private:
	int order = 3;
	bool insert(keyNode<T>* newKey, BPTNode<T>*& Root);//内部结点专用，插入键值
	bool myDelete(BPTNode<T>* now, BPTNode<T>*& Root);//内部结点专用，删除键值
	BPTNode<T>* getLeftBro(BPTNode<T>* now,BPTNode<T>* Root);//找到当前结点的左兄弟结点
	BPTNode<T>* getHead(BPTNode<T>* Root);//找到当前结点所在树的最小结点
};

template<class T>
inline BPTNode<T>::BPTNode() {

}

template<class T>
inline BPTNode<T>::BPTNode(const T data, int order)
{
	key = {};
	this->order = order;
	keyNode<T> tmp(data);
	key.push_back(tmp);
}

template<class T>
inline BPTNode<T>::BPTNode(vector<T>& key,int order)//新建内部结点
{
	this->order = order;
	sort(key.begin(), key.end());
	for (auto p : key) {
		keyNode<T> tmp(p);
		this->key.push_back(tmp);
	}
}

template<class T>
inline bool BPTNode<T>::find(const T keyValue)
{
	for (auto& p : key)
		if (p.data == keyValue)
			return true;
	return false;
}

template<class T>
inline bool BPTNode<T>::insert(const T keyValue, BPTNode<T>*& Root)//叶节点插入数据
{
	if (find(keyValue)) return false;//如果结点中已经存在该数据，插入失败
	if (key.size() == 0)//结点为空直接插入
		key.push_back(keyNode<T>(keyValue));
	else if (keyValue > (*key.rbegin()).data)//插入数据比已有数据最大值还大，则直接插入最后位置
		key.push_back(keyNode<T>(keyValue));
	else {//找到应该插入位置并插入数据
		int len = key.size();
		int i = 0;
		for (i = 0; i < len; i++)
			if (keyValue < key[i].data) break;
		key.emplace(key.begin() + i, keyNode<T>(keyValue));
	}
	if (key.size() < order) {//空间充足,插入后仍符合定义
		return true;
	}
	else {//叶节点空间不够，插入后分裂成两个叶节点，并把中间的键值进位到父结点中
		int m = order / 2;
		keyNode<T>* newKey = new keyNode<T>(key[m]);//复制中间位置的键值
		BPTNode<T>* newBPTNode = new BPTNode<T>;//创建新的叶结点
		newBPTNode->father = father;
		newBPTNode->setorder(order);
		newKey->left = this;
		newKey->right = newBPTNode;
		newBPTNode->next = this->next;
		this->next = newBPTNode;
		for (int i = m; i < key.size(); i++) {//后一半的键值复制到新的叶节点中
			keyNode<T>* tmp = new keyNode<T>;
			tmp->data = key[i].data;
			tmp->left = key[i].left;
			tmp->right = key[i].right;
			newBPTNode->key.push_back(*tmp);
		}
		key.resize(m);//删除当前叶节点后半部分键值
		if (father) {//若父节点存在则把中间位置的键值插入父结点
			father->insert(newKey, Root);
			return true;
		}
		else {//若父节点不存在则创造一个新的内部节点作为父节点
			BPTNode<T>* tmp = new BPTNode<T>;//创建新的叶结点
			tmp->setorder(order);
			this->father = tmp;
			newBPTNode->father = tmp;
			tmp->key.push_back(*newKey);
			Root = tmp;
			return true;
		}
	}
	return true;
}

template<class T>
inline bool BPTNode<T>::myDelete(const T keyValue, BPTNode<T>*& Root)//结点中删除一个键值
{
	if (!find(keyValue))//找不到要删除的键值
		return false;
	for (int i = 0; i < key.size(); i++) {
		if (key[i].data == keyValue) {
			key.erase(key.begin() + i);
			break;
		}
	}
	int m = (order + 1) / 2 - 1;
	if (father == nullptr) return true;//如果是根结点，直接删除即可
	if (key.size() >= m) {//删除后结点满足定义，直接删除并调整父节点中对应关键字即可
		for (auto& p : father->key) {
			if (p.right == this) {
				p.data = key[0].data;
				break;
			}
		}
		return true;
	}
	else {//如果兄弟结点有富裕，则从兄弟结点借一个，同时把父节点中对应关键字更新为借的关键字
		BPTNode<T>* leftBro = nullptr;
		BPTNode<T>* rightBro = nullptr;
		int leftLoc = 0, rightLoc = 0;
		for (int i = 0; i < father->key.size(); i++) {
			if (father->key[i].left == this) {
				rightBro = father->key[i].right;
				rightLoc = i;
			}	
			if (father->key[i].right == this) {
				leftBro = father->key[i].left;
				leftLoc = i;
			}	
		}
		if (leftBro != nullptr && leftBro->key.size() > m) {//左兄弟结点够借
			keyNode<T> newKey = leftBro->key[leftBro->key.size() - 1];
			key.emplace(key.begin(), newKey);
			leftBro->key.erase(leftBro->key.end() - 1);
			father->key[leftLoc].data = key[0].data;//更新父节点中对应关键字
			return true;
		}
		if (rightBro != nullptr && rightBro->key.size() > m) {//右兄弟结点够借
			keyNode<T> newKey = rightBro->key[0];
			key.push_back(newKey);
			rightBro->key.erase(rightBro->key.begin());
			father->key[rightLoc].data = rightBro->key[0].data;//更新父节点中对应关键字
			return true;
		}
		
		if (leftBro != nullptr) {//如果左右兄弟都不够借且左兄弟存在，则和左兄弟合并，并删除父节点中对应关键字
			for (int i = 0; i < leftBro->key.size(); i++) {//左兄弟数据复制到当前结点前面
				key.emplace(key.begin() + i, leftBro->key[i]);
			}
			BPTNode<T>* cousin = getLeftBro(leftBro,Root);
			if (cousin != nullptr) cousin->next = this;
			if (leftLoc > 0)
				father->key[leftLoc - 1].right = this;
			father->key.erase(father->key.begin() + leftLoc);//删除父节点中对应关键字
			delete leftBro;
			return this->myDelete(father, Root);
		}
		else if (rightBro != nullptr) {//和右兄弟合并，并删除父节点中对应关键字
			for (int i = 0; i < rightBro->key.size(); i++) {//右兄弟数据复制到当前结点末尾
				key.push_back(rightBro->key[i]);
			}
			next = rightBro->next;
			if (rightLoc + 1 < father->key.size())
				father->key[rightLoc + 1].left = this;
			father->key.erase(father->key.begin() + rightLoc);//删除父节点中对应关键字
			delete rightBro;
			return this->myDelete(father, Root);
		}
	}
	return false;
}

template<class T>
inline bool BPTNode<T>::isEmpty()
{
	if (key.size() == 0) return true;
	return false;
}

template<class T>
inline bool BPTNode<T>::setorder(int order)
{
	if (key.size() >= order)
		return false;
	this->order = order;
	return true;
}

template<class T>
inline bool BPTNode<T>::insert(keyNode<T>* newKey, BPTNode<T>*& Root)//内部结点插入数据
{
	if (newKey->data > (*key.rbegin()).data)//插入数据比已有数据最大值还大，则直接插入最后位置
		key.push_back(*newKey);
	else {//找到应该插入位置并插入数据
		int len = key.size();
		int i = 0;
		for (i = 0; i < len; i++)
			if (newKey->data < key[i].data) break;
		if (i != 0)	key[i - 1].right = newKey->left;
		key[i].left = newKey->right;
		key.emplace(key.begin() + i, *newKey);
	}
	if (key.size() < order) {//空间充足,插入后仍符合定义
		return true;
	}
	else {//内部节点空间不够，插入后分裂成两个内部节点，并把中间的键值进位到父结点中
		int m = (order - 1) / 2;
		keyNode<T>* newKey = new keyNode<T>(key[m]);//复制中间位置的键值
		BPTNode<T>* newBPTNode = new BPTNode<T>;//创建新的内部结点
		newBPTNode->father = father;
		newBPTNode->setorder(order);
		newKey->left = this;
		newKey->right = newBPTNode;
		key[m].right->father = newBPTNode;
		for (int i = m + 1; i < key.size(); i++) {//后一半的键值复制到新的叶节点中
			if (key[i].right) key[i].right->father = newBPTNode;
			keyNode<T>* tmp = new keyNode<T>;
			tmp->data = key[i].data;
			tmp->left = key[i].left;
			tmp->right = key[i].right;
			newBPTNode->key.push_back(*tmp);
		}
		key.resize(m);//删除当前叶节点后半部分键值
		if (father) {//若父节点存在则把中间位置的键值插入父结点
			father->insert(newKey, Root);
			return true;
		}
		else {//若父节点不存在则创造一个新的内部节点作为父节点
			BPTNode<T>* tmp = new BPTNode<T>;//创建新的内部结点
			tmp->setorder(order);
			this->father = tmp;
			newBPTNode->father = tmp;
			tmp->key.push_back(*newKey);
			Root = tmp;
			return true;
		}
	}
	return false;
}

template<class T>
inline bool BPTNode<T>::myDelete(BPTNode<T>* now, BPTNode<T>*& Root)
{
	int m = (order + 1) / 2 - 1;
	if (Root == now) {//当前结点是根结点，若当前结点不为空直接返回，为空修改根节点后返回
		if (now->key.size() == 0) {
			this->father = nullptr;
			Root = this;
			delete now;
			return true;
		}
		return true;
	}
	if (now->key.size() >= m) return true;//当前结点不是根结点，删除后符合定义则直接返回
	BPTNode<T>* dad = now->father;
	BPTNode<T>* leftBro = nullptr;
	BPTNode<T>* rightBro = nullptr;
	int leftLoc = 0, rightLoc = 0;
	for (int i = 0; i < dad->key.size(); i++) {
		if (dad->key[i].right == now) {
			leftLoc = i;
			leftBro = dad->key[i].left;
		}	
		if (dad->key[i].left == now) {
			rightLoc = i;
			rightBro = dad->key[i].right;
		}		
	}
	if (leftBro != nullptr && leftBro->key.size() > m) {//如果左兄弟结点够借
		keyNode<T> newKey(dad->key[leftLoc].data);
		newKey.left = leftBro->key[leftBro->key.size() - 1].right;
		if (now->key.size() != 0)
			newKey.right = now->key[0].left;
		now->key.push_back(newKey);
		dad->key[leftLoc].data = leftBro->key[leftBro->key.size() - 1].data;
		leftBro->key.erase(leftBro->key.end() - 1);
		return true;
	}
	if (rightBro != nullptr && rightBro->key.size() > m) {//如果右兄弟结点够借
		keyNode<T> newKey(dad->key[rightLoc].data);
		newKey.right = rightBro->key[0].left;
		if (now->key.size() != 0)
			newKey.left = now->key[now->key.size() - 1].right;
		now->key.push_back(newKey);
		dad->key[rightLoc].data = rightBro->key[0].data;
		rightBro->key.erase(rightBro->key.begin());
		return true;
	}
	if (leftBro != nullptr) {//如果左右兄弟都不够借且左兄弟存在
		BPTNode<T>* dad = now->father;
		keyNode<T> newKey;
		newKey.data = dad->key[leftLoc].data;
		newKey.left = leftBro->key[leftBro->key.size() - 1].right;
		if (now->key.size() != 0)
			newKey.right = now->key[0].left;
		dad->key.erase(dad->key.begin() + leftLoc);//删除父节点中对应关键字
		now->key.emplace(now->key.begin(), newKey);//父结点中对应关键字下移到当前结点
		for (int i = 0; i < leftBro->key.size(); i++) {//左兄弟数据复制到当前结点前面
			now->key.emplace(now->key.begin() + i, leftBro->key[i]);
			leftBro->key[i].left->father = now;
			leftBro->key[i].right->father = now;
		}
		delete leftBro;
		return now->myDelete(dad, Root);
	}
	if(rightBro!=nullptr){//如果左右兄弟都不够借且右兄弟存在
		BPTNode<T>* dad = now->father;
		keyNode<T> newKey;
		newKey.data = dad->key[rightLoc].data;
		newKey.right = rightBro->key[0].left;
		if (now->key.size() != 0)
			newKey.left = now->key[now->key.size() - 1].right;
		dad->key.erase(dad->key.begin() + rightLoc);//删除父节点中对应关键字
		now->key.push_back(newKey);//父结点中对应关键字下移到当前结点
		for (int i = 0; i < rightBro->key.size(); i++) {//右兄弟数据复制到当前结点后面
			now->key.push_back(rightBro->key[i]);
			rightBro->key[i].left->father = now;
			rightBro->key[i].right->father = now;

		}
		delete rightBro;
		return now->myDelete(dad, Root);
	}
	return false;
}

template<class T>
inline BPTNode<T>* BPTNode<T>::getLeftBro(BPTNode<T>* now,BPTNode<T>* Root) {
	BPTNode<T>* tmp = getHead(Root);
	while (tmp->next) {
		if (tmp->next == now)
			return tmp;
		tmp = tmp->next;
	}
	return nullptr;
}

template<class T>
inline BPTNode<T>* BPTNode<T>::getHead(BPTNode<T>* Root) {
	BPTNode<T>* tmp = Root;
	while (tmp->key.size() > 0 && tmp->key[0].left)
		tmp = tmp->key[0].left;
	return tmp;
}

