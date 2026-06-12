
//#ifndef HDR_TREE_ITEM_4_USERLEAFNODE_H
//#define HDR_TREE_ITEM_4_USERLEAFNODE_H

//#endif //HDR_TREE_ITEM_4_USERLEAFNODE_H


#pragma once

#include "Clusteru.h"

#include "userNode.h"

#include <iostream>
class userLeafNode :
        public userNode
{
public:
    userLeafNode(Clusteru* C, bool verbose);
    ~userLeafNode();

    auto begin() { return cluster->Users.begin(); }
    auto end() { return cluster->Users.end(); }

    Clusteru* cluster;
};
