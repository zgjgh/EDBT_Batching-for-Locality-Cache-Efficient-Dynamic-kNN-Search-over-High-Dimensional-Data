#pragma once

#include <vector>

#include <random>

#include <iostream>
#include <chrono>
#include <set>

#include "Cluster.h"

#include "User.h"

typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;
typedef const Eigen::Ref<const Eigen::MatrixXf>& MatrixRef;

class Clusters
{
public:
	Clusters(const std::vector<User*>& U, long f, MatrixRef T);

	Clusters(Cluster* Cj, long f, MatrixRef T);

	~Clusters();

	auto begin() { return children.begin(); }
	auto end() { return children.end(); }

    std::vector<long> clusterUsers;
    std::vector<float> clusterDistances;
    std::vector<Cluster *> children;

private:

	void process();


	std::vector<User *> Users;
	std::vector<Eigen::VectorXf> transformedUsers;



	void init();
	long updateUsers();
	int getNearestCluster(long index);

	float dist(long user, long cluster);

	bool kMeansClustering();

	float updateClusters();

	void computeMaxDist();

	unsigned long numClusters;

	void clear();

	long maxIterations;
};

