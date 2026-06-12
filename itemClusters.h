#pragma once

#include <vector>

#include <random>

#include <iostream>
#include <chrono>
#include <set>

#include "itemCluster.h"

#include "User.h"

typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;
typedef const Eigen::Ref<const Eigen::MatrixXf>& MatrixRef;

class itemClusters
{
public:
	itemClusters(const std::vector<Item*>& I, long f, MatrixRef T);

	itemClusters(itemCluster* Cj, long f, MatrixRef T);

	~itemClusters();

	auto begin() { return children.begin(); }
	auto end() { return children.end(); }
    std::vector<itemCluster*> children;

private:

	void process();



	std::vector<Item*> Items;
	std::vector<Eigen::VectorXf> transformedItems;
	std::vector<long> clusterItems;
	std::vector<float> clusterDistances;


	void init();
	long updateUsers();
	int getNearestCluster(long index);

	float dist(long item, long cluster);

	bool kMeansClustering();

	float updateClusters();

	void computeMaxDist();

	unsigned long numClusters;

	void clear();

	long maxIterations;
};



