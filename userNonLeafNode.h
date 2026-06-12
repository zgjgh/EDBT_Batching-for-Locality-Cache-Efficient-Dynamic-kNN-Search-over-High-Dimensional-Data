
//#ifndef HDR_TREE_ITEM_4_USERNONLEAFNODE_H
//#define HDR_TREE_ITEM_4_USERNONLEAFNODE_H

//#endif //HDR_TREE_ITEM_4_USERNONLEAFNODE_H


#pragma once

#include <vector>
#include <iostream>

#include "Clustersu.h"

#include "userNode.h"

class userNonLeafNode :
        public userNode
{
public:
    userNonLeafNode(Clustersu* C, long level, bool verbose);
    ~userNonLeafNode();

    std::vector<Clusteru*> clusters;

    std::vector<userNode*> children;

    long l;
};