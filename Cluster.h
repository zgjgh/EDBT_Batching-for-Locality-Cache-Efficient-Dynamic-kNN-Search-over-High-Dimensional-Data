#pragma once

#include "Eigen//Dense"

#include <vector>

#include "User.h"

#include "Cluster_origin.h"

class Node;

class Cluster:
    public Cluster_origin
{
public:
	Eigen::VectorXf center;
	float radius;
	unsigned long number;
	float maxdknn;
	unsigned long l;

	std::vector<std::tuple<float, User*>> Users;

	std::vector<float> distance_cross_u_t;

	Node* ptr;

    long u_cluster_sequense;

    std::vector<float> dis_itemleafnode_list;

    long sequence;

    std::vector<User*>  user_need_knn;

    float maxdknnbound;

    float user_KNN_radius;

    std::vector<float> dis_itemcluster;

    Eigen::VectorXf low_d_center;

    std::vector<User* > user_need_knnlist;

    float radius_user_need_knn;

    bool have_user;

    float max_dknncell;


};