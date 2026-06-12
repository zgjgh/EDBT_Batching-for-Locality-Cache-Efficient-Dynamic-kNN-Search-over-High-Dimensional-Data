#include "itemClusters.h"

itemClusters::itemClusters(const std::vector<Item*>& I, long f, MatrixRef T)
{

	numClusters = f;
	maxIterations = 1000;


	//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
	//	std::cout << "U " << U.row(0).transpose().rows() << "x" << U.row(0).transpose().cols() << std::endl;

	//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;

	for (int i = 0; i < I.size(); i++) {

		auto X = T * (I[i]->v);
		transformedItems.emplace_back(X);

		Items.emplace_back(I[i]);
		clusterItems.emplace_back(-1);
		clusterDistances.emplace_back(0);
	}

	process();
}

itemClusters::itemClusters(itemCluster* Cj, long f, MatrixRef T) {

	numClusters = f;
	maxIterations = 1000;

	//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
	//	std::cout << "U " << std::get<1>(Cj->Users[0]).rows() << "x" << std::get<1>(Cj->Users[0]).cols() << std::endl;

	//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;
	//	std::cout << std::endl << "P1: " << T.row(1) << std::endl << std::endl;

	for (int i = 0; i < Cj->Items.size(); i++) {

		auto X = T * (std::get<1>(Cj->Items[i])->v);
		transformedItems.emplace_back(X);

		Items.emplace_back(std::get<1>(Cj->Items[i]));

		clusterItems.emplace_back(-1);
		clusterDistances.emplace_back(0);
	}
	process();

	//Cj->Items.clear();
	//Cj->Items.shrink_to_fit();
}


itemClusters::~itemClusters()
{
	clear();

	children.clear();
	children.shrink_to_fit();
}

void itemClusters::process() {

	//	std::cout << "Clustering " << Users.size()
	//		<< " users in " << numClusters
	//		<< " clusters. with dimension " << transformedUsers[0].rows() 
	//		<< "x" << transformedUsers[0].cols() << std::endl;

	bool res = kMeansClustering();

	if (!res) {
		return;
	}

	computeMaxDist();

	//	for (int i = 0; i < numClusters; i++) {
	//		std::cout << "Cluster: " << i
	//			<< " Radius: " << children[i].radius
	//			<< " Nodes: " << children[i].number << std::endl;
	//	}

	clear();
}

void itemClusters::init() {

	/* Seed */
	std::random_device rd;

	/* Random number generator */
	std::default_random_engine generator(rd());

	/* Distribution on which to apply the generator */
	std::uniform_int_distribution<long unsigned> distribution(0, Items.size() - 1);

	std::set<long unsigned> Lind;

	while (Lind.size() < numClusters)
	{
		Lind.insert(distribution(generator));
	}

	auto i = 0;
	for (auto ind : Lind)
	{
		itemCluster* child = new itemCluster();
		child->center = transformedItems[ind];
		/*child->maxdknn = 0;*/
		child->l = 1;
		child->number = 0;
		child->radius = 0;
		child->ptr = nullptr;
		children.emplace_back(child);

		//		std::cout << "Initialized Cluster " << i
		//			<< " from index " << ind
		//			<< " having value " << Users[ind]->center.transpose() << std::endl;

				//		std::cout << "User " << Users[ind] << std::endl;
		i++;
	}
}

long itemClusters::updateUsers() {

	long numUpdates = 0;
	for (int i = 0; i < Items.size(); i++) {
		long C = getNearestCluster(i);

		if (C < 0) {
			return C;
		}

		if (C != clusterItems[i]) {
			numUpdates++;
		}

		clusterItems[i] = C;
	}

	return numUpdates;
}

int itemClusters::getNearestCluster(long index) {
	int C = -1;
	float minDist = std::numeric_limits<float>::max();
	float d;

	for (int k = 0; k < numClusters; k++) {
		d = dist(index, k);
		if (d < minDist) {
			minDist = d;
			C = k;
		}
	}

	clusterDistances[index] = minDist;

	//	if (C < 0) {
	//		for (int k = 0; k < numClusters; k++) {
	//			d = dist(index, k);
	//			std::cout << "Error: Cluster " << k
	//				<< " Distance " << d << std::endl;
	//			std::cout << "User: " << transformedUsers[index] << std::endl;
	//			std::cout << "Center: " << children[k].center << std::endl;
	//
	//		}
	//	}

	return C;
}

float itemClusters::dist(long item, long cluster) {


	if ((transformedItems[item].rows() != children[cluster]->center.rows()) ||
		(transformedItems[item].cols() != children[cluster]->center.cols())) {

		std::cout << " Item : " << item << " / " << Items.size() << std::endl;
		std::cout << " Cluster : " << cluster << " / " << numClusters << std::endl;

		std::cout << " Item : " << transformedItems[item].rows() << "x" << transformedItems[item].cols() << std::endl;
		std::cout << " Cluster : " << children[cluster]->center.rows() << "x" << children[cluster]->center.cols() << std::endl;

		std::cout << " Item : " << transformedItems[item].transpose() << std::endl;
		std::cout << " Cluster : " << children[cluster]->center << std::endl;
	}


	float res = (transformedItems[item] - children[cluster]->center).norm();

	return res;
}

float itemClusters::updateClusters() {

	Eigen::MatrixXf Sum = Eigen::MatrixXf::Zero(numClusters, transformedItems[0].size());
	Eigen::VectorXi Num = Eigen::VectorXi::Zero(numClusters);

	for (int i = 0; i < Items.size(); i++)
	{
		Sum.row(clusterItems[i]) += transformedItems[i];
		Num(clusterItems[i]) += 1;
	}

	float displacement = 0;
	for (int i = 0; i < numClusters; i++) {
		Eigen::VectorXf newCenter = Sum.row(i) / (1.0f * Num(i));
		displacement += (newCenter - children[i]->center).norm();
		children[i]->center = newCenter;
	}

	return displacement;
}

void itemClusters::computeMaxDist() {

	//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	long nClusters = std::min(1.0f * numClusters, 1.0f * clusterItems.size());

	for (int i = 0; i < Items.size(); i++)
	{
		children[clusterItems[i]]->radius = 0;
	}

	for (int i = 0; i < Items.size(); i++)
	{
		children[clusterItems[i]]->radius = std::max(children[clusterItems[i]]->radius, clusterDistances[i]);
	}


	for (int i = 0; i < Items.size(); i++) {
		std::tuple<float, Item*> V(clusterDistances[i], Items[i]);
		children[clusterItems[i]]->Items.emplace_back(V);
	}

	for (int i = 0; i < numClusters; i++)
	{
		std::sort(children[i]->Items.begin(), children[i]->Items.end(),
			[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
				return std::get<0>(a) < std::get<0>(b);
			});

		//		for (int j = 0; j < std::min(1.0f*numClusters, 1.0f*children[i].Users.size()); j++) {
		//			std::cout << "Cluster:\t" << i
		//				<< "\tNeighbour:\t" << j
		//				<< "\tDistance:\t" << std::get<0>(children[i].Users[j]) << std::endl;
		//		}

		long Siz = children[i]->Items.size();
		children[i]->number = Siz;
	}

	//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	//	diff = diff / 1000.0f;

	//	printf("KNN in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));

}
bool itemClusters::kMeansClustering() {


	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	init();

	long numUpdates;
	float displacement;

	for (int i = 0; i < maxIterations; i++) {
		numUpdates = updateUsers();

		if (numUpdates < 0) {
			std::cout << "Error occured During Clustering " << std::endl;
			return false;
		}

		displacement = updateClusters();

		//		std::cout << "Iteration\t" << i
		//			<< "\tUpdates:\t" << numUpdates
		//			<< "\tDisplacement\t" << displacement << std::endl;

		if ((numUpdates == 0) || (displacement < 0.000001f)) {
			updateUsers();
			break;
		}

		if ((maxIterations - i) < 2) {
			std::cout << "Warning: K-Means failed to converge" << std::endl;
		}
	}

	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	//diff = diff / 1000.0f;

	//printf("K-Means %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));

	return true;
}

void itemClusters::clear() {

	Items.clear();
	Items.shrink_to_fit();

	transformedItems.clear();
	transformedItems.shrink_to_fit();

	clusterItems.clear();
	clusterItems.shrink_to_fit();

	clusterDistances.clear();
	clusterDistances.shrink_to_fit();
}

