#include "itemHDR_Tree.h"

itemHDR_Tree::itemHDR_Tree()
{
}

itemHDR_Tree::~itemHDR_Tree()
{
}

void itemHDR_Tree::setData(itemDataBase* dataBase) {
	data = dataBase;
}

void itemHDR_Tree::construct(long fanout, long threshold) {

	std::cout << "Constructing HDR Tree for Items" << std::endl;
	std::cout << "Items: " << numItems << std::endl;
	std::cout << "Features: " << numFeatures << std::endl;
	std::cout << "Fanout: " << fanout << std::endl << std::endl;

	init();

	L = log(data->I.size()) / log(fanout);
	std::cout << "Estimated Height: " << L << " levels." << std::endl;

	data->initTransform(L);
	long numClusters = 0;

	auto C = new itemClusters(Items, fanout, data->T[0]);
	for (auto Cj : *C) {

		numClusters += 1;
		Cj->l = 1;
	}

	itemRoot = new itemNonLeafNode(C, 1, verbose);

	itemRoot_ptr = itemRoot;

	std::queue<itemNode*> q;

	q.push(itemRoot);

	long maxL = 0;

	while (!q.empty()) {
		auto node = dynamic_cast<itemNonLeafNode*>(q.front());
		q.pop();

		auto Cp = node->clusters;

		for (auto Cj : Cp) {

			if (Cj->number < threshold) {
				auto LN = new itemLeafNode(Cj, verbose);

				node->children.emplace_back(LN);
				Cj->ptr = LN;
			}
			else {
				auto level = node->l + 1;
				maxL = std::max(maxL, level);

				auto Cpp = new itemClusters(Cj, fanout, data->T[level - 1]);
				for (auto Cj : *Cpp) {

					numClusters += 1;
					Cj->l = level;
				}

				auto NLN = new itemNonLeafNode(Cpp, level, verbose);

				node->children.emplace_back(NLN);
				Cj->ptr = NLN;
				q.push(NLN);
			}
		}
	}

	std::cout << "Actual Height: " << maxL << " levels." << std::endl;
	std::cout << "Clusters: " << numClusters << std::endl;

}

void itemHDR_Tree::projectItem(Item* I) {

	iTrans.clear();
	for (int i = 0; i < L; i++) {
		iTrans.emplace_back(data->T[i] * (I->v));
	}
}

float itemHDR_Tree::dist(Item* I, itemCluster* C) {

	numDistanceComputions++;
	float res = (iTrans[C->l - 1] - C->center).norm();
	return res - C->radius;
}
float itemHDR_Tree::dist(User* U, itemCluster* C) {

	numDistanceComputions++;
	float res = (uTrans[C->l - 1] - C->center).norm();
	return res - C->radius;
}
float itemHDR_Tree::dist(Item* I, User* U) {
    numDistanceComputions++;
    float res = (U->center - (I->v)).norm();
    return res;

}

float itemHDR_Tree::dist(VectorRef A, VectorRef B) {

    if ((A.rows() != B.rows()) ||
        (A.cols() != B.cols())) {
        std::cout << "Error : float itemHDR_Tree::dist(VectorRef A, VectorRef B) : Dimension Mismatch" << std::endl;
        std::cout << " A : " << A.rows() << "x" << A.cols() << std::endl;
        std::cout << " B : " << B.rows() << "x" << B.cols() << std::endl;
    }


    numDistanceComputions++;
    float res = (A - B).norm();
    return res;
}

void itemHDR_Tree::init() {

	data->transformData(numFeatures, checkDuplicates);
	data->generateItem(numItems);
	data->computePCA();

	for (long i = 0; i < data->I.rows(); i++) {
		auto itm = new Item(data->I.row(i));
		Items.emplace_back(itm);
	}

	/*itemIndex = windowSize;*/

	//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	//	diff = diff / 1000.0f;

	//	printf("Cluster Initialized in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));

}

