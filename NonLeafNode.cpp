#include "NonLeafNode.h"



NonLeafNode::NonLeafNode(Clusters* C, long level, bool verbose)
{
	/*for (auto Cp : *C) {

		if (verbose) {
			std::cout << "Non Leaf Node Level " << level << "\tCenter: " << Cp->center.transpose()
				<< "\tRadius: " << Cp->radius
				<< "\tDKNN: " << Cp->maxdknn
				<< "\tChildren: " << Cp->number << std::endl;
		}

		clusters.emplace_back(Cp);
	}

	l = level;


	type = NodeType::NonLeafNode;*/

	if(C->children.size()!=0){
        for (auto Cp : *C) {

            if (verbose) {
                std::cout << "Non Leaf Node Level " << level << "\tCenter: " << Cp->center.transpose()
                          << "\tRadius: " << Cp->radius
                          << "\tDKNN: " << Cp->maxdknn
                          << "\tChildren: " << Cp->number << std::endl;
            }

            clusters.emplace_back(Cp);
        }

        l = level;


        type = NodeType::NonLeafNode;
	}
}

NonLeafNode::NonLeafNode() {

}

NonLeafNode::~NonLeafNode()
{
}
