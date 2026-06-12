#pragma once

#include <vector>
#include <iostream>

#include "itemClusters.h"

#include "itemNode.h"

class itemNonLeafNode :
	public itemNode
{
public:
	itemNonLeafNode(itemClusters* C, long level, bool verbose);
	~itemNonLeafNode();

	std::vector<itemCluster*> clusters;

	std::vector<itemNode*> children;

	long l;

	long size;
};


