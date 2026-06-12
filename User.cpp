#include "User.h"
#include <immintrin.h>
#include "itemLeafNode.h"
#include "itemNonLeafNode.h"


User::User(Eigen::VectorXf C, long KNN)
{
	center = C;

	dknn = std::numeric_limits<float>::max();
	k = KNN;
}
User::~User()
{
}

/*
void User::computeKNN(const std::vector<Item*>& L) { // v2

	auto cmp = []
	(const std::tuple<float, long>& a, const std::tuple<float, long >& b)
	{
		return std::get<0>(a) < std::get<0>(b);
	};

	std::set<std::tuple<float, long>, decltype(cmp)> V(cmp);

	float maxVal = std::numeric_limits<float>::min();
	float D;
	for (long i = 0; i < L.size(); i++) {
		dist(L[i], D);

		if (V.size() < k) {
			V.insert(std::tuple<float, long>(D, i));
			auto it = V.end();
			it--;
			maxVal = std::get<0>(*it);
		}

		if (D < maxVal) {
			V.insert(std::tuple<float, long>(D, i));

			auto it = V.end();
			it--;
			V.erase(it);

			it = V.end();
			it--;
			maxVal = std::get<0>(*it);
		}
	}

	dknn = sqrt(maxVal);

	for (auto e : V) {
		R.emplace_back(L[std::get<1>(e)]);
	}

}
*/
void User::computeKNN(const std::vector<Item*>& L) { // v1

	std::vector<std::tuple<float, long>> V;
	V.resize(L.size());
	float D;
	for (int i = 0; i < L.size(); i++) {
		dist(L[i], D);
		std::tuple<float, long> T(D,i);
		V[i] = T;
	}

	std::nth_element(V.begin(), V.begin() + k - 1, V.end(),
		[&](const std::tuple<float, long>& a, const std::tuple<float, long >& b) -> bool {
			return std::get<0>(a) < std::get<0>(b);
		});

	dknn = std::get<0>(V[k - 1]);

	R.clear();

	for (int i = 0; i < k; i++) {
		R.emplace_back(L[std::get<1>(V[i])]);
	}

	/*std::cout << "R size: " << R.size()<<std::endl;*/
}


void User::computeKNN_sort(const std::vector<Item*>& L) { // v1

    std::vector<std::tuple<float, Item*>> V;
    //V.resize(L.size());
    float D;
    for (auto item_l: L) {
        dist(item_l, D);
        V.emplace_back(std::make_tuple(D, item_l));
    }

    std::nth_element(V.begin(), V.begin() + k - 1, V.end(),
                     [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                         return std::get<0>(a) < std::get<0>(b);
                     });

    dknn = std::get<0>(V[k - 1]);

    std::sort(V.begin(), V.begin()+k-1, [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
        return std::get<0>(a) < std::get<0>(b);
    });

    R.clear();

    //R.assign(V.begin(), V.begin()+k-1);



    /*std::vector<std::tuple<float, Item*>> V_0;
    for (int i = 0; i < k; i++) {
        V_0.emplace_back(std::make_tuple(std::get<0>(V[i]),L[std::get<1>(V[i])]));
    }*/


    /*std::sort(R.begin(), R.end(), [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
        return std::get<0>(a) <= std::get<0>(b);
    });*/

    for (int i = 0; i < k; i++) {
        R.emplace_back(std::get<1>(V[i]));
    }

    /*std::cout << "R size: " << R.size()<<std::endl;*/
}





//void User::userComputeKNN(itemNode* node, User* users) { // v1
//
//	int clust;
//
//	if (node->type == itemNodeType::itemLeafNode) {
//		std::vector<std::tuple<float, Item*>> V;
//
//		auto LN = dynamic_cast<itemLeafNode*>(node);
//		long maxL = 0;
//
//		float D;
//		for (auto It : *LN) {
//			auto item = std::get<1>(It);
//
//			D = dist(D, item);
//
//			std::tuple<float, Item*> T(D, item);
//			V.emplace_back(T);
//
//		}
//
//		std::sort(V.begin(), V.end(),
//			[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
//				return std::get<0>(a) < std::get<0>(b);
//			});
//
//		for (int i = 0; i < k; i++) {
//			R.emplace_back(std::get<1>(V[i]));
//		}
//	}
//	else {
//		/*std::cout << "Non Leaf Node of Item Tree!!!" << std::endl;*/
//		auto NLN = dynamic_cast<itemNonLeafNode*>(node);
//		auto& Cp = NLN->clusters;
//
//		float D;
//		auto minDist = std::numeric_limits<float>::max();
//
//		for (long j = 0; j < Cp.size(); j++) {
//			D = dist(D, Cp[j]);
//
//			if (D < minDist) {
//				minDist = D;
//				clust = j;
//			}
//		}
//		userComputeKNN(NLN->children[clust]);
//	}
//}

/*
void User::computeKNN(const std::vector<Item*>& L) { // v0
	
	std::vector<std::tuple<float, Item*>> V;
	V.resize(L.size());
	for (int i = 0; i < L.size(); i++) {
		float D = dist(L[i]);
		std::tuple<float, Item*> T(D,L[i]);
		V[i] = T;
	}

	std::nth_element(V.begin(), V.begin() + k - 1, V.end(),
		[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
			return std::get<0>(a) < std::get<0>(b);
		});

	dknn = std::get<0>(V[k - 1]);

	for (int i = 0; i < k; i++) {
		R.emplace_back(std::get<1>(V[i]));
	}
}
*/


//float User::dist(float& D, itemCluster* C) { // v1 for User and Cluster
//
////	D = (I->v - center).squaredNorm();
//	D = (center - C->center).norm();
//
//	return D;
//}
//
//float User::dist(float& D, Item* I)
//{
//	D = (center - I->v).norm();
//	return D;
//}

void User::dist(Item* I, float& D) { // v0 for any system

//	D = (I->v - center).squaredNorm();
	D = ((I->v).cast<float>() - center).norm();
	return;
}


void User::append(Item* I) {

	std::vector<std::tuple<float, Item*>> V;

	float D;
	for (int i = 0; i < R.size(); i++) {
		dist(R[i], D);
		std::tuple<float, Item*> T(D, R[i]);
		V.emplace_back(T);
	}

	dist(I, D);

	//std::cout<<"The distance between the new item and the affected user "<<D<<std::endl;

	std::tuple<float, Item*> T(D, I);
	V.emplace_back(T);



//	std::cout << "[" << D << "] I: " << I->v.segment(1, 5).transpose() << std::endl;

//	std::sort(V.begin(), V.end(),
//		[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
//			return std::get<0>(a) < std::get<0>(b);
//		});

	std::nth_element(V.begin(), V.begin() + k - 1, V.end(),
		[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
			return std::get<0>(a) < std::get<0>(b);
		});

	auto ind = std::min(1.0f * k, 1.0f * V.size());

	dknn = std::get<0>(V[ind-1]);

	//std::cout<<"The new dknn "<<dknn<<std::endl;

	R.clear();
	for (int i = 0; i < ind; i++) {
		R.emplace_back(std::get<1>(V[i]));
//		std::cout << "[" << std::get<0>(V[i]) << "] R: " << std::get<1>(V[i])->v.segment(1, 5).transpose() << std::endl;
	}

	/*std::cout<<"The new items in R"<<std::endl;

	/*for(auto i: R){
	    std::cout<<i->v<<std::endl;
	    //std::cout<<"The index of the item= "<<i->index_b_plus<<std::endl;
        printf("<index of item:%f>\n", i->index_b_plus);
	    std::cout<<" "<<std::endl;
	}*/

//	std::cout << "DKNN: " << dknn << " Size:" << R.size() << std::endl;
}

void User::remove(Item* I) {

	for (long i = 0; i < R.size(); i++) {
		if (*R[i] == *I) {
			remove(i);
		}
	}
}
void User::remove(long index) {
//	std::cout << "Removing " << index << std::endl;

//	for (int i = 0; i < R.size(); i++) {
//		std::cout << "RB: " << R[i]->v.segment(1, 5).transpose() << std::endl;
//	}

	R.erase(std::next(R.begin(), index));

//	for (int i = 0; i < R.size(); i++) {
//		std::cout << "RA: " << R[i]->v.segment(1, 5).transpose() << std::endl;
//	}
}

void User::update(Item* I, Mode m) {

	if (m == Mode::Add) {
		append(I);		
	}
	else {
		remove(I);
	}
}


