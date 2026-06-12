#include "itemLeafNode.h"



itemLeafNode::itemLeafNode(itemCluster* C, bool verbose)
{
	cluster = C;
	type = itemNodeType::itemLeafNode;

	if (verbose) {
		std::cout << "Leaf Node Level " << C->l << "\tCenter:" << C->center.transpose()
			<< "\tRadius: " << C->radius
			<< "\tChildren: " << C->number << std::endl;
	}

    if_item_update= false;
	radius_updated_items=0;
    original_size=C->Items.size();

    if ((C->Items).size()==0){
        size_available= false;

    }
    else{
        size_available= true;

        center=std::get<1>((C->Items)[0])->v;

        int i;

        for(i=1;i<(C->Items).size();i++){
            center=center+std::get<1>((C->Items)[i])->v;
        }

        center=center/((C->Items).size()*1.0f);



        std::vector<float> store_radius;
        float distance_center_item;

        for(auto item_cluster:C->Items){
            distance_center_item=(center-(std::get<1>(item_cluster))->v).norm();
            store_radius.emplace_back(distance_center_item);
        }

        std::nth_element(store_radius.begin(), store_radius.begin() + store_radius.size() - 1, store_radius.end(),
                         [&](const float & a, const float & b) -> bool {
                             return a < b;
                         });
        orginal_radius=store_radius[store_radius.size()-1];
    }

}


itemLeafNode::~itemLeafNode()
{
}
