
#include "userLeafNode.h"



userLeafNode::userLeafNode(Clusteru* C, bool verbose)
{
    cluster = C;
    type = userNodeType::userLeafNode;

    if (verbose) {
        std::cout << "Leaf Node Level " << C->l << "\tCenter:" << C->center.transpose()
                  << "\tRadius: " << C->radius
                  << "\tChildren: " << C->number << std::endl;
    }

}


userLeafNode::~userLeafNode()
{
}
