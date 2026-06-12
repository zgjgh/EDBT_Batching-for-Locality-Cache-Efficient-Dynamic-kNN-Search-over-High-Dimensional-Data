#pragma once

#include <vector>
#include <iostream>

#include "Clusters.h"

#include "Node.h"

class NonLeafNode :
	public Node
{
public:
	NonLeafNode(Clusters* C, long level, bool verbose);
	NonLeafNode();
	~NonLeafNode();

	std::vector<Cluster*> clusters;

	std::vector<Node*> children;

	long l;
};

