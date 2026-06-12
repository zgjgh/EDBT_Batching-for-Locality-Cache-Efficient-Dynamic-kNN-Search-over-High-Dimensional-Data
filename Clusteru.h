
//#ifndef HDR_TREE_ITEM_4_CLUSTERU_H
//#define HDR_TREE_ITEM_4_CLUSTERU_H

//#endif //HDR_TREE_ITEM_4_CLUSTERU_H


#pragma once

#include "Eigen//Dense"

#include <vector>

#include "User.h"

#include "userNode.h"

#include "Littletree.h"

class Clusteru
{
public:
    Eigen::VectorXd center;
    double radius;
    unsigned long number;
    double maxdknn;
    unsigned long l;
    userNode* ptr;

    //unsigned long No;

    //double insection_r;

    std::vector<std::tuple<double, User*>> Users;

//The vector for holding the location of the vectors containing the users The vector for holding the location of the vectors containing the users The vector for holding the location of the vectors containing the users

    std::vector<std::vector<User*>*> section_user;
    std::vector<Littletree*> little_tree;

//The vector for holding the location of the vectors containing the users The vector for holding the location of the vectors containing the users The vector for holding the location of the vectors containing the users

    //bool out;
    //bool visited_flag;
    //bool out_visited_flag;
    //bool in_visited_flag;
    //double out_in;
    //double in_in;
    //double general_in;
    //double distance_user;




};
