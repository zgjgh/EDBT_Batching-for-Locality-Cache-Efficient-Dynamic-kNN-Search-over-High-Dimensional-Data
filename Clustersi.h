
//#ifndef HDR_TREE_ITEM_4_CLUSTERSI_H
//#define HDR_TREE_ITEM_4_CLUSTERSI_H

//#endif //HDR_TREE_ITEM_4_CLUSTERSI_H

#pragma once
#include <vector>

#include <random>

#include <iostream>
#include <chrono>
#include <set>

#include "Clusteri.h"


typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;
typedef const Eigen::Ref<const Eigen::MatrixXf>& MatrixRef;



class Clustersi
{
public:
    Clustersi(const std::vector<Item*>& I, long f);

    Clustersi(Clusteri* Cj, long f);

    ~Clustersi();

    auto begin() { return children.begin(); }
    auto end() { return children.end(); }

    std::vector<long> clusterItems;      //Change Users to Items
    std::vector<double> clusterDistances;   //The iterator that contains the distance from the item i to its nearest clusteri
    std::vector<Clusteri *> children;   //Change Cluster to Clusteri



private:

    void process();



    std::vector<Item *> Items;     //Change User* to Item*, Change Users to Items
    std::vector<Eigen::VectorXd> transformedItems;   // Change transformedUsers to transformedItems



    void init();
    long updateItems();
    int getNearestClusteri(long index);

    double dist(long item, long clusteri);

    bool kMeansClustering();

    double updateClustersi();

    void computeMaxDist();

    unsigned long numClusters;

    void clear();

    long maxIterations;
};
