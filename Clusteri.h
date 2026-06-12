
//#ifndef HDR_TREE_ITEM_4_CLUSTERI_H
//#define HDR_TREE_ITEM_4_CLUSTERI_H

//#endif //HDR_TREE_ITEM_4_CLUSTERI_H

#pragma once

#include "Eigen//Dense"

#include <vector>

#include "Item.h"




class Clusteri
{
public:
    Eigen::VectorXd center;
    double radius;
    unsigned long number;
    //float maxdknn;
    unsigned long No;

    double insection_r;

    std::vector<std::tuple<double, Item*>> Items;


    bool out;
    bool visited_flag;
    bool out_visited_flag;
    bool in_visited_flag;
    double out_in;
    double in_in;
    double general_in;
    double distance_user;

    long sequece;



};