#include "LeafNode.h"



LeafNode::LeafNode(Cluster* C, bool verbose)
{
	cluster = C;
	type = NodeType::LeafNode;

	if (verbose) {
		std::cout << "Leaf Node Level " << C->l << "\tCenter:" << C->center.transpose()
			<< "\tRadius: " << C->radius
			<< "\tDKNN: " << C->maxdknn
			<< "\tChildren: " << C->number << std::endl;
	}

    original_number=C->Users.size();
    if_update_user=false;
    if_influenced_user=false;

	if ((C->Users).size()==0){
	    size_available= false;

	}
	else{
        size_available= true;

        center=std::get<1>((C->Users)[0])->center;

        int i;

        for(i=1;i<(C->Users).size();i++){
            center=center+std::get<1>((C->Users)[i])->center;
        }

        center=center/((C->Users).size()*1.0f);



        std::vector<float> store_radius;
        float distance_center_user;

        for(auto user_cluster:C->Users){
            distance_center_user=(center-(std::get<1>(user_cluster))->center).norm();
            store_radius.emplace_back(distance_center_user);
        }

        std::nth_element(store_radius.begin(), store_radius.begin() + store_radius.size() - 1, store_radius.end(),
                         [&](const float & a, const float & b) -> bool {
                             return a < b;
                         });
        original_radius=store_radius[store_radius.size()-1];

        radius_updated_users=0;

        radius_influenced_users=0;


	}
}

void LeafNode::initial_center_low(std::vector<Eigen::MatrixXf> *pca_list) {
    for(long i=0; i<(*pca_list).size(); i++){
        C_low_d.emplace_back((*pca_list)[i] * center);
    }
}



LeafNode::LeafNode(){

}

LeafNode::~LeafNode()
{
}
