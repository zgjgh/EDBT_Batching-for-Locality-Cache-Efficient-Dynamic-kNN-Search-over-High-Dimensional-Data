
//#ifndef HDR_TREE_ITEM_4_CLUSTERSU_H
//#define HDR_TREE_ITEM_4_CLUSTERSU_H

//#endif //HDR_TREE_ITEM_4_CLUSTERSU_H


#pragma once
#include <vector>

#include <random>

#include <iostream>
#include <chrono>
#include <set>

#include "Clusteru.h"




typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;
typedef const Eigen::Ref<const Eigen::MatrixXf>& MatrixRef;



class Clustersu
{
public:
    Clustersu(const std::vector<User*>& U, long f);

    Clustersu(Clusteru* Cj, long f);

    ~Clustersu();

    auto begin() { return children.begin(); }
    auto end() { return children.end(); }

    std::vector<long> clusterUsers;      //Change Items to Users
    std::vector<double> clusterDistances;   //The iterator that contains the distance from the User u to its nearest clusteru
    std::vector<Clusteru *> children;   //Change Clusteri to Clusteru



private:

    void process();



    std::vector<User *> Users;     //Change Item* to User*, Change Items to Users
    std::vector<Eigen::VectorXd> transformedUsers;   // Change transformedItems to transformedUsers



    void init();
    long updateUsers();
    int getNearestClusteru(long index);

    double dist(long user, long clusteru);

    bool kMeansClustering();

    double updateClustersu();

    void computeMaxDist();

    unsigned long numClusters;

    void clear();

    long maxIterations;
};

