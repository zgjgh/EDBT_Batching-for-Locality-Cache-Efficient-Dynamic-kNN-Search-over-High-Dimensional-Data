#pragma once

#include <queue>

#include <map>

#include <fstream>

#include "DataBase.h"

#include "Clusters.h"

#include "NonLeafNode.h"

#include "LeafNode.h"

#include "Item.h"

#include "itemHDR_Tree.h"

#include <thread>

#include "Clustersi.h"

#include "BPlusTree.h"

#include "Clustersu.h"

#include "userLeafNode.h"

#include "userNonLeafNode.h"

//#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include "vtune_analysis.h"
#endif

typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;

class HDR_Tree : itemHDR_Tree
{
public:
	HDR_Tree();
	~HDR_Tree();

	void setData(DataBase* dataBase);

	void construct(long fanout, long threshold);
	void updateConstruct(Node* node);


	void stream();
	void userStream();

	void remove();

	void sync();

	long k;
	long windowSize;
	long numFeatures;
	bool verbose;
	long numUsers;
	bool checkDuplicates;
	float updateTime;

	std::vector<User*> Rp;
	void Search(VectorRef V);

	long numDistanceComputions;
	long numKnnComputions;
    DataBase* data;
    std::vector<User*> Users;

//item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature item_hdr_tree_feature

    long k_i;
    long windowSize_i;
    long numFeatures_i;
    bool verbose_i;
    long numItems_i;
    bool checkDuplicates_i;
    float updateTime_i;
    std::vector<User*> Rp_i;
    long numDistanceComputions_i;
    long numKnnComputions_i;
    itemNode* itemRoot_ptr_i;
    itemDataBase* data_i;
    std::vector<Eigen::VectorXf> iTrans_i;
    std::vector<Eigen::VectorXf> uTrans_i;
    long itemIndex_i;
    std::vector <Item*> slidingWindow_i;
    long  threshold_i;
    long  fix_threshold_i = 5;
    std::chrono::steady_clock::time_point begin_i;
    std::vector<Item*> Items_i;
    long L_i;
    itemNode* itemRoot_i;
    std::vector<Eigen::VectorXf> Trans_u_i;
    std::vector <Item*> slidingWindow;
    std::vector<Eigen::VectorXf> u_trans_i_clus_init;
    void project_u_iclust_pca_init(User* U);
    float dist_u_iclust_init(itemCluster* C);

    // single-query search probe used by the batch-size model sampling
    void probe_user_leaf_stats(User* user, long& nVisited, float& maxDist);



//item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function
    float dist_i(Item* I, itemCluster* C);
    float dist_i(User* U, itemCluster* C);
    float dist_i(Item* I, User* U);
    float dist_i(VectorRef A, VectorRef B);
    void projectItem_i(Item* I);
    void setData_i(itemDataBase* dataBase);
    void init_i();
    long construct_i(long fanout, long threshold);
    void projectUser_item(User* U);
    float dist_u_ic(User* U, itemCluster* C);
    float dist_u_ic_str(User* U, itemCluster* C);
    void update_user_hdr(User* U, Mode m, Node* node);
    float init_time;
    float computation_of_distance_init;

    void update_item_hdr(itemNode* root_item_tree, Item* I, Mode m);

    float dist_i_ic_update_itree(Item* I, itemCluster* IC);
    float dist_u_uc_update_utree(User* U, Cluster* UC);
    void update_user();
    void update_item();

    long add_user_num;
    long add_item_num;

    void update_user_add();

    void update_item_add();

    void update_user_user_tree();

    void update_user_add_user_tree();

    void update_item_user_tree();

    void update_item_add_user_tree();

    std::vector<float> dist_rate_bound_n2_knn;

    std::vector<float> dist_rate_bound_n2_rknn;









//item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree item_hdr_tree

//The features for the idistance and b+ tree indexing structure

    Clustersi* clusters_item;     //For storing the clusters of items on full dimension for confirmming the reference points and the space between the real value corresponding to the reference points on the axis

    BPlusTree* bptree_item;     //For storing the real value corresponding to the points on the axis.

    double max_r_item_cluster;   //For storing the max radius of the item clusters on full dimension to confirm the space between the real value corresponding to the reference points

    void form_item_clusters(long f);   //For formming the clusters of the items on full dimension

    void indexing_item();    //For indexing the items from high dimensional data poiints to real value

    void build_bp_tree();    //For building a bplus tree to store the keys and the item*

    void insert_key_point(double key, Item* it);   //For inserting the key-pointer pairs to the bplus tree

    void complete_tree();   //Complete the initial state of the bptree_item

    double distance_ci_u(User* u, Clusteri* ci);        //Compute the distance between the center of an clusteri and an user


    void user_computeknn_id(User* u, double r_rate, double delta_r_rate, bool print_if);   //Use idistance and B_plus tree to compute the knn list of an user
    int distance_time;       //The times of calculation of the distances



//The features for the idistance and b+ tree indexing structure

//The features for the sphere tree

    userNode* root_sphere;
    void construct_sphere(long fanout, long threshold);
    double dist_cu_i(Clusteru* cu, Item* i);
    void search_sphere(userNode* node_sphere, Item* i);
    double dist_ud_id(User* ud, Item* id);
    std::vector<User*> Rp_sphere;
    bool AdjustMaxdknnsphere(userNode* n);
    void update_sphere(User* U, Mode m, userNode* node);
    float dist_cu_u(Clusteru* cu, User* u);
    void update_user_spheretree();
    void update_user_add_spheretree();
    void update_item_spheretree();
    void update_item_add_spheretree();


//The features for the sphere tree


//The features for checking the power of the sphere tree

    int num_sphere_cal_clusters;
    int num_sphere_cal_users;


//The features for checking the power of the sphere tree

//The features for checking the power of the HDR tree

    int num_hdr_cal_clusters;
    int num_hdr_cal_users;


//The features for checking the power of the HDR tree

//The features for checking the projection time and the search time for the HDR tree

    float projection_time;
    float search_time_HDR;

//The features for checking the projection time and the search time for the HDR tree

//The features for checking the search time for the sphere tree

    float search_time_spheretree;

//The features for checking the search time for the sphere tree

//The features for update the knn lists and the indexing structure of the KNN+ method

    void update_user_knnjoinplus();
    void update_user_add_knnjoinplus();
    void update_item_knnjoinplus();
    void update_item_add_knnjoinplus();

    double distance_item_clusteri(Item* i_item, Clusteri* c_clusteri);



//The features for update the knn lists and the indexing structure of the KNN+ method


//The function for optimization of deletion


    void update_item_op_delet_usertree();
    std::vector<User*> Rp_0;
    void search_deletion_opt(Node* node, Item*I);


//The function for optimization of deletion


//The function for the deletion update when using only one HDR tree for the users for comparison with the optimization method, using computeKNN_sort.

    void update_item_user_tree_compute_sort();


//The function for the deletion update when using only one HDR tree for the users for comparison with the optimization method, using computeKNN_sort.

//The function for the distance between item and user cluster, combination of dimension reduction and distance calculation


    float distance_u_ucluster(Item* i, Cluster* c);



//The function for the distance between item and user cluster, combination of dimension reduction and distance calculation


//The function for constructing the little tree The function for constructing the little tree The function for constructing the little tree The function for constructing the little tree

    int construct_littletree(std::vector<User*>* tree_user, LeafNode* tree_node, NonLeafNode* tree_root, DataBase* tree_data, long fan, long thresh);

//The function for constructing the little tree The function for constructing the little tree The function for constructing the little tree The function for constructing the little tree

//The clusters for constructing the little trees The clusters for constructing the little trees The clusters for constructing the little trees The clusters for constructing the little trees

    Clustersu* clusters_user_tree;

//The clusters for constructing the little trees The clusters for constructing the little trees The clusters for constructing the little trees The clusters for constructing the little trees

//The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees

    void build_clustersu (long n_c);

//The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees

    void complete_little_trees(long n_t, long l_f, long l_thresh);

//The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees

//A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data

    void project_item_little(Item* i_p, DataBase* db_l, int height_in);

//A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data

//Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees.

    void update_item_multipletree();

//Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees.

//Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees.

    void update_item_add_multipletree();

//Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees.

//Storage of the roots of the influenced trees. Storage of the roots of the influenced trees. Storage of the roots of the influenced trees. Storage of the roots of the influenced trees.

    std::vector<Node*> root_influenced;

//Storage of the roots of the influenced trees. Storage of the roots of the influenced trees. Storage of the roots of the influenced trees. Storage of the roots of the influenced trees.


//The features for cross optimization; The features for cross optimization; The features for cross optimization; The features for cross optimization;


    std::vector<vector<itemCluster*>*> dictionary_for_layer_itemcluster;
    std::vector<int*> record_sequence;
    long check_cluster_pruned_bycross;
    long check_cluster_visited;
    long check_cluster_pruned_traditional;
    long check_prune[5][7];

    void initialize_two_dimensional_array();  //Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune;


//For checking the time of steps in the crosscomputeknn method; For checking the time of steps in the crosscomputeknn method; For checking the time of steps in the crosscomputeknn method;

    float time_judgelevel;         //For calculation of the time for the judgement if the level of the item cluster exceeds the level of the highest cluster that contains the user

    float time_transformtype;   //For calculation of the time for transformation from cluster_origin to cluster;

    float time_calculate_crossprunedistance;   //For calculation of the time for calculation of the cross distance between itemclusters and userclusters;

    float time_traditional_prune;          //For calculation of the time for calculation of the traditional prunning distance;

    float time_knn_computation;    //For the time of knn computation;

    float time_other;  //For the time of rknn and projection;

    float time_compute_knn_cross; //For the time of the knn computation;

    float time_compute_knn_cross_head; //For compute the time for the head of the function compute_knn_cross;

    float time_compute_sort_list; //For compute the time for sort the candidate list of the item cluster;

//For checking the time of steps in the crosscomputeknn method; For checking the time of steps in the crosscomputeknn method; For checking the time of steps in the crosscomputeknn method;



//The features for cross optimization; The features for cross optimization; The features for cross optimization; The features for cross optimization;

//The features for storing the clusters on the HDR Tree. The features for storing the clusters on the HDR Tree. The features for storing the clusters on the HDR Tree.

    std::vector<vector<Cluster*>*> dictionary_for_layer_usercluster;
    std::vector<int*> record_sequence_user;


//The features for storing the clusters on the HDR Tree. The features for storing the clusters on the HDR Tree. The features for storing the clusters on the HDR Tree.

//The features for storing the leafnodes. The features for storing the leafnodes. The features for storing the leafnodes. The features for storing the leafnodes.


    std::vector<LeafNode*> leafnodelist;
    std::vector<itemLeafNode*> itemleafnodelist;

//The features for storing the leafnodes. The features for storing the leafnodes. The features for storing the leafnodes. The features for storing the leafnodes.

//The features for storing the clusters. The features for storing the clusters. The features for storing the clusters. The features for storing the clusters.


    std::vector<Cluster*> cluster_list;
    std::vector<itemCluster*> itemcluster_list;


//The features for storing the clusters. The features for storing the clusters. The features for storing the clusters. The features for storing the clusters.

//The features for calculating the distance between the leafnodes and the clusters. The features for calculating the distance between the leafnodes and the clusters.

    void calculate_distance_itemcluster_userleaf();
    void calculate_distance_usercluster_itemleaf();


//The features for calculating the distance between the leafnodes and the clusters. The features for calculating the distance between the leafnodes and the clusters.

//The function for updating of the items and updating the delta tree. The function for updating of the items and updating the delta tree. The function for updating of the items and updating the delta tree.

    void update_item_collectleaf(itemNode* root_item_tree, Item* I, Mode mode);



//The function for updating of the items and updating the delta tree. The function for updating of the items and updating the delta tree. The function for updating of the items and updating the delta tree.

    std::vector<Item*> list_of_noleaf_updateitem;   //For store the items that are given to the leafnodes without items originally
    std::vector<itemLeafNode*> updated_itemleafnode_list;   //To store the itemleafnodes that have items inserted or deleted

//The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree.

    void update_user_collectleaf(User* U, Mode m, Node* node);


//The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree.

    std::vector<User*> list_of_noleaf_updateuser;   //For store the users that are given to the leafnodes without users originally.
    std::vector<LeafNode*> update_userleafnode_list;     //To the store the userleafnodes that have users inserted or deleted.


//The function for searching the influenced users by the updated item leafnodes and updating the KNN of the influenced users and updating the HDR Tree.

    void search_and_updateadd_by_itemleafnode(Node* node, itemLeafNode* itemleafnode);

//The function for searching the influenced users by the updated item leafnodes and updating the KNN of the influenced users and updating the HDR Tree.

//The function for add items for batch. The function for add items for batch. The function for add items for batch.

    void add_item_batch(long n);

//The function for add items for batch. The function for add items for batch. The function for add items for batch.

//The function for knn calculating of the users surrounding the leafnode; The function for knn calculating of the users surrounding the leafnode;

    void leafnodecomputeknn(LeafNode* hdrtreeleafnode, itemNode* node);


//The function for knn calculating of the users surrounding the leafnode; The function for knn calculating of the users surrounding the leafnode;


//The function for deleting items for batch. The function for deleting items for batch. The function for deleting items for batch.

    void delete_item_batch(long n);


//The function for deleting items for batch. The function for deleting items for batch. The function for deleting items for batch.


    std::vector<User*> influenced_user_noleaf;    //To store the Users that are influenced by the deleted items but are in leafnodes who originally have no items

    std::vector<LeafNode*> influenced_leafnode;   //To store the leafnodes that have items in them updated.

//When single deleted item, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When single deleted item, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

    void search_influenced_singleuser_leafnodes_single_delete_item(Node* node, Item* single_deleted_item);

//When single deleted item, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When single deleted item, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

//When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

    void search_influenced_singleuser_leafnodes_delete_itemleafnode(Node* node, itemLeafNode* include_deleted_itemleafnode);

//When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

//For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted.

    void leafnode_influenced_computeknn(LeafNode* hdrtreeleafnode, itemNode* node);

//For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted.

//RKNN search applying the middle dimensions between the full dimensions and the highest dimensions on the tree structure.

    void search_using_middle_dim(Node* node, Item* I);

//RKNN search applying the middle dimensions between the full dimensions and the highest dimensions on the tree structure.

//Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree.

    void add_user_batch(long n);

//Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree.


//To move some of the operations from the init function to the function below; To move some of the operations from the init function to the function below;



    void init_half_behind();



//To move some of the operations from the init function to the function below; To move some of the operations from the init function to the function below;


    void init_half_behind_2();

    void userComputeKNN_2(itemNode* node, User* user);

//For storage of the proportion of the sum of the variance of the front numbers of dimensions in the sum of the total variance; For storage of the proportion of the sum of the variance of the front numbers of dimensions in the sum of the total variance;

    std::vector<float> vector_proportion_variance;


//For storage of the proportion of the sum of the variance of the front numbers of dimensions in the sum of the total variance; For storage of the proportion of the sum of the variance of the front numbers of dimensions in the sum of the total variance;




//For storage of the average p1 of each user; For storage of the average p1 of each user; For storage of the average p1 of each user; For storage of the average p1 of each user;

    std::vector<float> vector_for_p1_eachuser;

//For storage of the average p1 of each user; For storage of the average p1 of each user; For storage of the average p1 of each user; For storage of the average p1 of each user;



//For storage of the average p2 of each user; For storage of the average p2 of each user; For storage of the average p2 of each user; For storage of the average p2 of each user;

    std::vector<float> vector_for_p2_eachuser;

//For storage of the average p2 of each user; For storage of the average p2 of each user; For storage of the average p2 of each user; For storage of the average p2 of each user;

//The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree;

    long height_hdr;

//The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree; The real height of the hdr tree;
//The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree;

    long dim_hdr;

//The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree;
//The real height of the delta tree; The real height of the delta tree; The real height of the delta tree; The real height of the delta tree;

    long height_delta;

//The real height of the delta tree; The real height of the delta tree; The real height of the delta tree; The real height of the delta tree;
//The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree;

    long dim_delta;

//The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree; The maxmal dimensions of the hdr tree;

    void userComputeKNN_statistic(itemNode* node, User* user, std::vector<float>* p1_v, std::vector<float>* p2_v);
    //



    void init_half_behind_loop(long divide_num);





    std::vector<long> v_d_prune;







    std::vector<float> partialsum_variance_proportion;


    std::vector<float> p1_for_dimensions;



    std::vector<float> p2_for_dimensions;



    std::vector<float> v_time;



    std::vector<itemCluster*> V_lowd_I_C;



    std::vector<Cluster*> V_lowd_U_C;



    void form_clusters_user(long dim, long f);
    void form_clusters_item(long dim, long f );
    void calculate_distance_utree_icluster();
    void calculate_distance_itree_ucluster();
    void distribute_updated_item(std::vector<Item*>* updated_item);
    std::vector<itemCluster* >influenced_item_clusters;
    void itemclusters_false();
    void itemcluster_radius_zero();
    void itemcluster_updated_item_clear();
    void clear_updated_itemclusters();

    void rknn_search_update_additem(Node* node, itemCluster* include_deleted_itemleafnode);
    void rknn_search_update_additem_prune(Node* node, itemCluster* include_deleted_itemleafnode);
    void rknn_search_update_additem_section(int o, int n);
    void rknn_search_update_additem_paralell();

    //void rknn_search_cluster(Node* node, itemCluster* include_itemcluster);

    void add_item_batch_bycluster_prune(long n);
    void add_item_batch_bycluster(long n);
    void add_item_batch_bycluster_paralell(long n);

    long meet_clusters_rknn;
    long prune_clusters_rknn;
    long notprune_clusters_rknn;
    void update_item_add_naive(long n);
    void update_item_add_assemble(long n);

    void knn_by_clusters(Cluster* influenced_cluster, itemNode* node);
    void knn_by_clusters_prune(Cluster* influenced_cluster, itemNode* node);

    std::vector<Cluster*> clusters_haveuserknn;
    void userclusters_false();
    void usercluster_radius_zero();
    void usercluster_updated_user_clear();
    void clear_clusters_haveuserknn();
    void distribute_user_needknn(std::vector<User*>* user_needknn);


    void update_user_add_batch_bycluster(long n);
    void update_user_add_batch_bycluster_paralell(long n);
    void update_user_add_batch_bycluster_prune(long n);

    float time_recorder;

    long item_add_update;

    float time_distance_calculation;
    float time_update_user_knn_additem;

    float distance_check(User* u, Item* i);

    void rknn_delete_item(Node* node, itemCluster* include_deleted_itemcluster);
    void rknn_delete_item_prune(Node* node, itemCluster* include_deleted_itemcluster);



    std::vector<User*> users_affected_deleteditems;

    void delete_item_batch_bycluster(long n);
    void delete_item_batch_bycluster_prune(long n);


    void computeknnuser_noroot(User* user);

    std::vector<float > p1_additem;

    std::vector<float > p2_additem;

    std::vector<float> p1_adduser;

    std::vector<float> p2_adduser;

    std::vector<float> p1_itemunit;

    std::vector<float> p2_itemunit;

    long num_total_meet;
    long num_cal_twice;

    void update_user_add_batchversion(long n);
    void update_user_add_fast();

    void update_user_add_batchstatistic(long n);
    void update_user_add_statistic();

    void Search_statistic(Node* node, Item* I);

    void Search_fast(Node* node, Item* I);


    void update_item_add_statistic();

    void update_item_add_batch_statistic(long n);

    long meet_num_additem;
    long update_num_additem;
    long twice_num_additem;

    void update_user_add_deltatree();

    void update_item_add_deltatree();

    void update_item_deltatree();

    void update_user_add_doubletree_prune();

    void update_item_add_doubletree_prune();

    void update_item_delete_doubletree_prune();


    float time_statistic;

    float time_leafnode;

    float time_leafnode_delta;





    float real_time_distance_calculation;

    long order_the_non_leafnode;

    long distance_time_low_d_leaf;

    long distance_time_low_d_nonleaf;

    std::vector<float> time_cluster_layer;

    std::vector<long> pruned_clusters_layer;

    std::vector<long> pruned_items_layer;

    std::vector<long> calculated_clusters_layer;

    float leafnode_time_delta;

    float time_update_user_add_exceptknn;

    float time_update_user_add_knn;

    float time_computeknn_project;

    float time_computeknn_full_distance;

    float time_computeknn_sort_1;

    float time_low_dimensional_distance;

    float time_computeknn_sort_2;

    float time_update_user_add_batch_except_knn;

    float time_update_user_add_batch_knn;

    float time_full_dimensional_distance_add_batch_knn;

    float time_leafnode_batch_add_items;

    float full_distance_time_batch_add_items;

    long time_for_full_distance_computation_batch_add_items;

    long time_for_full_distance_computation_dualtree;

    std::vector<float> knn_candidate_exceed_bound_proportion;

    std::vector<int> items_exceed_knn_bound_proportion_statistic;

    std::vector<float> rknn_candidate_exceed_bound_proportion;

    std::vector<int> users_exceed_rknn_bound_proportion_statistic;



    std::vector<float> vector_unprun_proportion_knn;

    std::vector<float> vector_unprun_proportion_rknn;

    long unprun_num_knn;

    long unprun_num_rknn;

    std::vector<float> vector_dist_rate_bound_n2_knn;
    std::vector<float> vector_dist_rate_bound_n2_rknn;

    long insert_user_num;

    void test_cache_effect();

    std::map<void*, int> item_access_count_single;
    std::map<void*, int> item_access_count_batch;

    void reset_cache_analysis() {
        item_access_count_single.clear();
        item_access_count_batch.clear();
    }

    void analyze_cache_pattern();

    void save_access_pattern(const std::string& filename, bool is_batch);

    void analyze_from_file(const std::string& single_file, const std::string& batch_file);


    struct AccessRecord {
        void* item_ptr;
        int item_id;
        int sequence_number;
        int user_id;
    };

    std::vector<AccessRecord> single_access_sequence;
    std::vector<AccessRecord> batch_access_sequence;
    int single_seq_counter = 0;
    int batch_seq_counter = 0;


    void clear_algorithm_data(bool is_batch);
    void save_access_sequence(const std::string& filename, bool is_batch);
    void load_access_sequence(const std::string& filename, bool is_batch);

    float distance_test_chache(User* user, Item* item);





private:


	std::vector<Eigen::VectorXf> iTrans;
	std::vector<Eigen::VectorXf> uTrans;

	long itemIndex;



	long  threshold;
	long  fix_threshold=5;

	float dist(Item* I, Cluster* C);
	float dist(User* U, Cluster* C);
	float dist(Item* I, User* U);
	float dist(VectorRef A, VectorRef B);

	void Search(Node* node, Item* I);

	//For searching on the blocks, for searching on the blocks, for searching on the blocks, for searching on the blocks

    void Search_block(Node* node, Item* I, Littletree* lt_pointer);

    //For searching on the blocks, for searching on the blocks, for searching on the blocks, for searching on the blocks

	void updateUser(Node* node, User* U);

	std::chrono::steady_clock::time_point begin;

	void initUsersSerial();
	void UsersParalell(int o, int n);

	void initUsersParalell();

	void insert(Item* I);
	void userInsert(User* U);
	void fillTree();
	void update(Item* I, Mode m);
	void userUpdate(User* U, Mode m);
	void UpdateNaiveRKNN(Item* I, Mode m);
	void UpdateHdrKNN(Item* I, Mode m);
	void userUpdateHdrKNN(User* U, Mode m);

	float AdjustMaxdknnPrev(Node* n);
	bool AdjustMaxdknnPaper(Node* n);

//For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees

    bool AdjustMaxdknnPaper_littletree(Node* n, Littletree* lpt_pointer);


//For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees





	void updateUsers(Item* I, Mode m, int o, int n);
	void updateUsersParalell(Item* I, Mode m);
	void updateUsersSerial(Item* I, Mode m);

	void userComputeKNN(itemNode* node, User* user);

	void userComputeKNNcross(itemNode* node, User* user);

	//float idist(float& D, itemCluster* C);

	/*void dist(Item* I, float& D);*/
	float dist(User* U, struct itemCluster* C);
	float dist(User* U, Item* I);

	long L;

	Node* root;
	NonLeafNode* parent;

	void init();

	void streamLinear();
	void streamRandom();
	void userStreamRandom();
	void streamIndex(long ind);
	void userStreamIndex(long ind);

	void syncUsers(int o, int n);

	void projectItem(Item* I);
	void projectUser(User* U);
};

