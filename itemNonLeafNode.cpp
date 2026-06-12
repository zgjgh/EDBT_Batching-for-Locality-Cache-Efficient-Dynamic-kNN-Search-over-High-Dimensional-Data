#include "itemNonLeafNode.h"

itemNonLeafNode::itemNonLeafNode(itemClusters* C, long level, bool verbose)
{
    long sum=0;
	for (auto Cp : *C) {

		if (verbose) {
			std::cout << "Non Leaf Node Level " << level << "\tCenter: " << Cp->center.transpose()
				<< "\tRadius: " << Cp->radius
				<< "\tChildren: " << Cp->number << std::endl;
		}

		clusters.emplace_back(Cp);
		sum=sum+Cp->Items.size();
	}

	l = level;

	size=sum;


	type = itemNodeType::itemNonLeafNode;


}


itemNonLeafNode::~itemNonLeafNode()
{
}
