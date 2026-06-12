#pragma once

#include <queue>

#include "itemDataBase.h"

#include "itemClusters.h"

#include "itemNonLeafNode.h"

#include "itemLeafNode.h"

#include "Item.h"

typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;

class itemHDR_Tree
{
public:
	itemHDR_Tree();
	~itemHDR_Tree();

	void setData(itemDataBase* dataBase);

	void construct(long fanout, long threshold);
	/*void updateConstruct(Node* node);*/


	long k;
	long windowSize;
	long numFeatures;
	bool verbose;
	long numItems;
	bool checkDuplicates;
	float updateTime;

	std::vector<User*> Rp;

	long numDistanceComputions;
	long numKnnComputions;

	itemNode* itemRoot_ptr;

private:
	itemDataBase* data;

	std::vector<Eigen::VectorXf> iTrans;
	std::vector<Eigen::VectorXf> uTrans;

	long itemIndex;

	std::vector <Item*> slidingWindow;

	long  threshold;
	long  fix_threshold = 5;

	float dist(Item* I, itemCluster* C);
	float dist(User* U, itemCluster* C);
	float dist(Item* I, User* U);
	float dist(VectorRef A, VectorRef B);

	std::chrono::steady_clock::time_point begin;


	std::vector<Item*> Items;

	long L;

	itemNode* itemRoot;
	/*itemNonLeafNode* parent;*/

	void init();

	void streamLinear();
	void streamRandom();
	void userStreamRandom();
	void streamIndex(long ind);
	void userStreamIndex(long ind);

	void syncUsers(int o, int n);

	void projectItem(Item* I);
};



