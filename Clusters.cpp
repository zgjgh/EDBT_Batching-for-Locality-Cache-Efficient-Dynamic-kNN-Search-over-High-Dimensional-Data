#include "Clusters.h"

Clusters::Clusters(const std::vector<User*>& U, long f, MatrixRef T)
{

	numClusters = f;
	maxIterations = 1000;

//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
//	std::cout << "U " << U.row(0).transpose().rows() << "x" << U.row(0).transpose().cols() << std::endl;

//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;

	for (int i = 0; i < U.size(); i++) {

		auto X = T * U[i]->center;
		transformedUsers.emplace_back(X);

		Users.emplace_back(U[i]);
		clusterUsers.emplace_back(-1);
		clusterDistances.emplace_back(0);
	}

	process();
}

Clusters::Clusters(Cluster* Cj, long f, MatrixRef T) {

	numClusters = f;
	maxIterations = 1500;

//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
//	std::cout << "U " << std::get<1>(Cj->Users[0]).rows() << "x" << std::get<1>(Cj->Users[0]).cols() << std::endl;

//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;
//	std::cout << std::endl << "P1: " << T.row(1) << std::endl << std::endl;

	for (int i = 0; i < Cj->Users.size(); i++) {

		auto X = T * std::get<1>(Cj->Users[i])->center;
		transformedUsers.emplace_back(X);
			
		Users.emplace_back(std::get<1>(Cj->Users[i]));

		clusterUsers.emplace_back(-1);
		clusterDistances.emplace_back(0);
	}



	process();



	//Cj->Users.clear();
	//Cj->Users.shrink_to_fit();
}


Clusters::~Clusters()
{
	clear();

	children.clear();
	children.shrink_to_fit();
}

void Clusters::process() {

//	std::cout << "Clustering " << Users.size()
//		<< " users in " << numClusters
//		<< " clusters. with dimension " << transformedUsers[0].rows() 
//		<< "x" << transformedUsers[0].cols() << std::endl;

    //std::cout<<"Start loops of assignment of users and update of the centers of the clusters"<<std::endl;

	bool res = kMeansClustering();

    //std::cout<<"Finish loops of assignment of users and update of the centers of the clusters"<<std::endl;


	if (!res) {
		return;
	}

    //std::cout<<"Start calculation of the radius of the clusters and insertion of the users into the clusters"<<std::endl;

	computeMaxDist();

    //std::cout<<"Finish calculation of the radius of the clusters and insertion of the users into the clusters"<<std::endl;

//	for (int i = 0; i < numClusters; i++) {
//		std::cout << "Cluster: " << i
//			<< " Radius: " << children[i].radius
//			<< " Nodes: " << children[i].number << std::endl;
//	}

	clear();
}

void Clusters::init() {

    //std::cout<<"Start producing random user sequence and making them as the initial centers of the clusters"<<std::endl;

	/* Seed */
	std::random_device rd;

	/* Random number generator */
	std::default_random_engine generator(rd());

	/* Distribution on which to apply the generator */
	std::uniform_int_distribution<long unsigned> distribution(0, Users.size()-1);

	std::set<long unsigned> Lind;

    //std::cout<<"Start using the generator and the distribution to initialize the centers of the clusters"<<std::endl;

    //std::cout<<"The number of clusters is "<<numClusters<<std::endl;
    //std::cout<<"The number of users needed to be assigned is "<<Users.size()<<std::endl;

    long get_initial_center;

	while (Lind.size() < numClusters)

	{

	    get_initial_center=distribution(generator);

	    //std::cout<<"The produced initial center is the user of No."<<get_initial_center<<std::endl;

		Lind.insert(get_initial_center);
	}

    //std::cout<<"Finish producing random user sequence and making them as the initial centers of the clusters"<<std::endl;

    //std::cout<<"Start initializing the clusters"<<std::endl;

	auto i = 0;
	for (auto ind : Lind)
	{
		Cluster* child = new Cluster();
		child->center = transformedUsers[ind];
		child->maxdknn = 0;
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

    //std::cout<<"Finish initializing the clusters"<<std::endl;
}

long Clusters::updateUsers() {

	long numUpdates = 0;
	for (int i = 0; i < Users.size(); i++) {
		long C = getNearestCluster(i);

		if (C < 0) {
			return C;
		}

		if (C != clusterUsers[i]) {
			numUpdates++;
		}

		clusterUsers[i] = C;
	}

	return numUpdates;
}

int Clusters::getNearestCluster(long index) {
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

float Clusters::dist(long user, long cluster) {


	if ((transformedUsers[user].rows() != children[cluster]->center.rows()) ||
		(transformedUsers[user].cols() != children[cluster]->center.cols())) {

		std::cout << " User : " << user << " / " << Users.size() << std::endl;
		std::cout << " Cluster : " << cluster << " / " << numClusters << std::endl;

		std::cout << " User : " << transformedUsers[user].rows() << "x" << transformedUsers[user].cols() << std::endl;
		std::cout << " Cluster : " << children[cluster]->center.rows() << "x" << children[cluster]->center.cols() << std::endl;

		std::cout << " User : " << transformedUsers[user].transpose() << std::endl;
		std::cout << " Cluster : " << children[cluster]->center << std::endl;
	}


	float res = (transformedUsers[user] - children[cluster]->center).norm();

	return res;
}

float Clusters::updateClusters() {

	Eigen::MatrixXf Sum = Eigen::MatrixXf::Zero(numClusters, transformedUsers[0].size());
	Eigen::VectorXi Num = Eigen::VectorXi::Zero(numClusters);

	for (int i = 0; i < Users.size(); i++) 
	{
		Sum.row(clusterUsers[i]) += transformedUsers[i];
		Num(clusterUsers[i]) += 1;
	}

	float displacement = 0;
	for (int i = 0; i < numClusters; i++) {
		Eigen::VectorXf newCenter = Sum.row(i) / (1.0f * Num(i));
		displacement += (newCenter - children[i]->center).norm();
		children[i]->center = newCenter;
	}
	
	return displacement;
}

void Clusters::computeMaxDist() {

//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	long nClusters = std::min(1.0f * numClusters, 1.0f*clusterUsers.size());

	for (int i = 0; i < Users.size(); i++)
	{
		children[clusterUsers[i]]->radius = 0;
	}

	for (int i = 0; i < Users.size(); i++)
	{
		children[clusterUsers[i]]->radius = std::max(children[clusterUsers[i]]->radius, clusterDistances[i]);
	}


	for (int i = 0; i < Users.size(); i++) {
		std::tuple<float, User *> V(clusterDistances[i], Users[i]);
		children[clusterUsers[i]]->Users.emplace_back(V);
	}

	for (int i = 0; i < numClusters; i++)
	{		
		std::sort(children[i]->Users.begin(), children[i]->Users.end(),
			[&](const std::tuple<float, User*> & a, const std::tuple<float, User*> & b) -> bool {
				return std::get<0>(a) < std::get<0>(b);
			});

//		for (int j = 0; j < std::min(1.0f*numClusters, 1.0f*children[i].Users.size()); j++) {
//			std::cout << "Cluster:\t" << i
//				<< "\tNeighbour:\t" << j
//				<< "\tDistance:\t" << std::get<0>(children[i].Users[j]) << std::endl;
//		}

		long Siz = children[i]->Users.size();
		children[i]->number = Siz;
	}

//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//	diff = diff / 1000.0f;

//	printf("KNN in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));

}
bool Clusters::kMeansClustering() {


	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	init();

	long numUpdates;
	float displacement;

	for (int i = 0; i < maxIterations; i++) {


	    //std::cout<<"Start update users into clusters"<<std::endl;
		numUpdates = updateUsers();
        //std::cout<<"Finish update users into clusters"<<std::endl;

		if (numUpdates < 0) {
			std::cout << "Error occured During Clustering " << std::endl;
			return false;
		}

        //std::cout<<"Start update cluster centers"<<std::endl;
		displacement = updateClusters();
        //std::cout<<"Finish update cluster centers"<<std::endl;
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

void Clusters::clear() {

	Users.clear();
	Users.shrink_to_fit();

	transformedUsers.clear();
	transformedUsers.shrink_to_fit();

	clusterUsers.clear();
	clusterUsers.shrink_to_fit();

	clusterDistances.clear();
	clusterDistances.shrink_to_fit();
}
