#pragma once

#include "Eigen//Dense"

#include <vector>

#include "Item.h"

//#include "User.h"

class itemNode;

struct itemCluster
{
	Eigen::VectorXf center;
	float radius;
	unsigned long number;
	/*float maxdknn;*/
	unsigned long l;

	std::vector<std::tuple<float, Item*>> Items;

	itemNode* ptr;

	long i_cluster_sequense;

	std::vector<float> dis_userleafnode_list;

	long sequence;

    std::vector<float> dis_usercluster;

    bool influenced;

    float low_d_radius;

    std::vector<Item*> updated_items;
};
