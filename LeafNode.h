#pragma once

#include "Cluster.h"

#include "Node.h"
class LeafNode :
	public Node
{
public:
	LeafNode(Cluster* C, bool verbose);
	LeafNode();
	~LeafNode();

	auto begin() { return cluster->Users.begin(); }
	auto end() { return cluster->Users.end(); }

	Cluster* cluster;



//For batch update; For batch update; For batch update; For batch update; For batch update; For batch update; For batch update;

    Eigen::VectorXf center;   //For store the center of original leafnode full dimensions

    std::vector<Eigen::VectorXf> C_low_d;

    bool on_off;            //Be use to check if the leafnode has users be updated;

    std::vector<User*> list_updated_user;   //To store the users in the leafnode being updated;

    float maxdknncell;    //To store the temporary dknn cell of the influenced users in the leafnode whose knn need to be calculated

    float maxdknncell_update;   //To store the temporary dknn cell of the updated users in the leafnode whose knn need to be calculated

    float radius_updated_users;   //To store the radius of the users updated in the leafnode.

    long original_number;    //To store the original number of users in the leafnode;

    float original_radius;   //To store the origianl radius of the leafnode;

    void initial_center_low(std::vector<Eigen::MatrixXf>* pca_list);   //To compute the low dimension form of the center;

    bool size_available;

    long sequence;

    bool if_update_user;

    bool if_influenced_user;

    float radius_influenced_users;

    std::vector<User*> list_influenced_user;


//For batch update; For batch update; For batch update; For batch update; For batch update; For batch update; For batch update;

};

