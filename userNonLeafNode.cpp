
#include "userNonLeafNode.h"

userNonLeafNode::userNonLeafNode(Clustersu* C, long level, bool verbose)
{
    for (auto Cp : *C) {

        if (verbose) {
            std::cout << "Non Leaf Node Level " << level << "\tCenter: " << Cp->center.transpose()
                      << "\tRadius: " << Cp->radius
                      << "\tChildren: " << Cp->number << std::endl;
        }

        clusters.emplace_back(Cp);
    }

    l = level;


    type = userNodeType::userNonLeafNode;
}


userNonLeafNode::~userNonLeafNode()
{
}

