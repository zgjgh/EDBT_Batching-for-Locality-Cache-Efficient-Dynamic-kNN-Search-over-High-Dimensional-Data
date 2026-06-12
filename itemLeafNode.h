#pragma once

#include "itemCluster.h"

#include "itemNode.h"

#include <iostream>
class itemLeafNode :
	public itemNode
{
public:
	itemLeafNode(itemCluster* C, bool verbose);
	~itemLeafNode();

	auto begin() { return cluster->Items.begin(); }
	auto end() { return cluster->Items.end(); }

	itemCluster* cluster;

    long sequence_ileafnode;

    Eigen::VectorXf center;   //For store the center of original itemleafnode full dimensions

    long original_size;   //To store the original size of the leafnode.

    float orginal_radius;   //To store the original radius of the leafnode.

    bool if_item_update;  //To record if there is item inserted or deleted

    bool size_available;

    std::vector<Item*> list_updated_item;   //To store the items in the leafnode being updated;

    float radius_updated_items;   //To store the radius of the users updated in the leafnode.




};


