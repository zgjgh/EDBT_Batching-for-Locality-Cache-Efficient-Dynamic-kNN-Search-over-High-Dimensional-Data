
#ifndef HDR_TREE_ITEM_4_ROOT_PLUS_DATA_H
#define HDR_TREE_ITEM_4_ROOT_PLUS_DATA_H

#endif //HDR_TREE_ITEM_4_ROOT_PLUS_DATA_H

#pragma once

#include "NonLeafNode.h"
#include "DataBase.h"

class Littletree
{
public:
    Node* littleroot;
    DataBase *littledatabase;
    float whole_maxdknn;
    float whole_radius;
    int height;
    float whole_low_radius;
    std::vector<User*> littlerp;
    Littletree();
    ~Littletree();
    std::vector<User*>* little_section;
};