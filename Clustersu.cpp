
#include "Clustersu.h"





Clustersu::Clustersu(const std::vector<User*>& U, long f)
{

    numClusters = f;
    maxIterations = 1000;

//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
//	std::cout << "U " << U.row(0).transpose().rows() << "x" << U.row(0).transpose().cols() << std::endl;

//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;

    for (int i = 0; i < U.size(); i++) {

        auto X = U[i]->center;
        transformedUsers.emplace_back(X.cast<double>());

        Users.emplace_back(U[i]);
        clusterUsers.emplace_back(-1);
        clusterDistances.emplace_back(0);
    }

    process();
}

Clustersu::Clustersu(Clusteru* Cj, long f) {

    numClusters = f;
    maxIterations = 1000;

//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;
//	std::cout << "U " << std::get<1>(Cj->Users[0]).rows() << "x" << std::get<1>(Cj->Users[0]).cols() << std::endl;

//	std::cout << std::endl << "P0: " << T.row(0) << std::endl << std::endl;
//	std::cout << std::endl << "P1: " << T.row(1) << std::endl << std::endl;

    for (int i = 0; i < Cj->Users.size(); i++) {

        auto X = std::get<1>(Cj->Users[i])->center;
        transformedUsers.emplace_back(X.cast<double>());

        Users.emplace_back(std::get<1>(Cj->Users[i]));

        clusterUsers.emplace_back(-1);
        clusterDistances.emplace_back(0);
    }
    process();

    //Cj->Users.clear();
    //Cj->Users.shrink_to_fit();
}


Clustersu::~Clustersu()
{
    clear();

    children.clear();
    children.shrink_to_fit();
}

void Clustersu::process() {

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

void Clustersu::init() {

    /* Seed */
    std::random_device rd;

    /* Random number generator */
    std::default_random_engine generator(rd());

    /* Distribution on which to apply the generator */
    std::uniform_int_distribution<long unsigned> distribution(0, Users.size()-1);

    std::set<long unsigned> Lind;

    while (Lind.size() < numClusters)
    {
        Lind.insert(distribution(generator));
    }

    auto i = 0;
    for (auto ind : Lind)
    {
        Clusteru* child = new Clusteru();
        child->center = transformedUsers[ind];
        child->maxdknn = 0;
        //child->l = 1;
        child->number = 0;
        child->radius = 0;
        //child->ptr = nullptr;
        children.emplace_back(child);

//		std::cout << "Initialized Cluster " << i
//			<< " from index " << ind
//			<< " having value " << Users[ind]->center.transpose() << std::endl;

        //		std::cout << "User " << Users[ind] << std::endl;
        i++;
    }
}

long Clustersu::updateUsers() {

    long numUpdates = 0;
    for (int i = 0; i < Users.size(); i++) {
        long C = getNearestClusteru(i);

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

int Clustersu::getNearestClusteru(long index) {
    int C = -1;
    double minDist = std::numeric_limits<double>::max();
    double d;

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

double Clustersu::dist(long item, long clusteru) {


    if ((transformedUsers[item].rows() != children[clusteru]->center.rows()) ||
        (transformedUsers[item].cols() != children[clusteru]->center.cols())) {

        std::cout << " Item : " << item << " / " << Users.size() << std::endl;
        std::cout << " Clusteri : " << clusteru << " / " << numClusters << std::endl;

        std::cout << " Item : " << transformedUsers[item].rows() << "x" << transformedUsers[item].cols() << std::endl;
        std::cout << " Clusteri : " << children[clusteru]->center.rows() << "x" << children[clusteru]->center.cols() << std::endl;

        std::cout << " Item : " << transformedUsers[item].transpose() << std::endl;
        std::cout << " Clusteri : " << children[clusteru]->center << std::endl;
    }


    double res = (transformedUsers[item] - children[clusteru]->center).norm();

    return res;
}

double Clustersu::updateClustersu() {

    Eigen::MatrixXd Sum = Eigen::MatrixXd::Zero(numClusters, transformedUsers[0].size());
    Eigen::VectorXi Num = Eigen::VectorXi::Zero(numClusters);

    for (int i = 0; i < Users.size(); i++)
    {
        Sum.row(clusterUsers[i]) += transformedUsers[i];
        Num(clusterUsers[i]) += 1;
    }

    double displacement = 0;
    for (int i = 0; i < numClusters; i++) {
        Eigen::VectorXd newCenter = Sum.row(i) / (1.0f * Num(i));
        displacement += (newCenter - children[i]->center).norm();
        children[i]->center = newCenter;
    }

    return displacement;
}

void Clustersu::computeMaxDist() {

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
        std::tuple<double, User *> V(clusterDistances[i], Users[i]);
        children[clusterUsers[i]]->Users.emplace_back(V);
    }

    for (int i = 0; i < numClusters; i++)
    {
        std::sort(children[i]->Users.begin(), children[i]->Users.end(),
                  [&](const std::tuple<double, User*> & a, const std::tuple<double, User*> & b) -> bool {
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
bool Clustersu::kMeansClustering() {


    //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    init();

    long numUpdates;
    double displacement;

    for (int i = 0; i < maxIterations; i++) {
        numUpdates = updateUsers();

        if (numUpdates < 0) {
            std::cout << "Error occured During Clustering " << std::endl;
            return false;
        }

        displacement = updateClustersu();

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

void Clustersu::clear() {

    Users.clear();
    Users.shrink_to_fit();

    transformedUsers.clear();
    transformedUsers.shrink_to_fit();

    clusterUsers.clear();
    clusterUsers.shrink_to_fit();

    clusterDistances.clear();
    clusterDistances.shrink_to_fit();
}
