//#define EIGEN_USE_MKL_ALL

#include <cstdlib>

#include "DataBase.h"
#include "HDR_Tree.h"
#include "itemDataBase.h"
#include "itemHDR_Tree.h"
#include "BPlusTree.h"
#ifdef _WIN32
#include <windows.h>
#endif
#define M 200
#define N 5000


#include <cmath>
#include <fstream>

DataBase dataBase;
HDR_Tree tree;
itemDataBase idataBase;
itemHDR_Tree itree;


void test_NUS_WID() {

	idataBase.load("Normalized_WT.dat");
	idataBase.verbose = false;

	itree.numFeatures = 112;
	itree.checkDuplicates = false;
	itree.numItems = 5000;
	itree.verbose = false;
	itree.setData(&idataBase);
	itree.construct(5, 7);

	std::cout << "\n - - - Finished Contructing Item Tree - - - \n\n" << std::endl;


	dataBase.load("Normalized_WT.dat");
	dataBase.verbose = false;

	tree.k = 10;
	tree.windowSize = 200000;
	tree.numFeatures = 128;
	tree.checkDuplicates = false;
	tree.numUsers = 5000;
	tree.verbose = false;
	tree.setData(&dataBase);
	tree.construct(5, 7);

	std::cout << "\n- - - Finished Contructing User Tree - - -\n\n" << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	tree.numDistanceComputions = 0;
	tree.numKnnComputions = 0;
	tree.updateTime = 0;

	for (long i = 1; i <= 600; i++) {

		tree.stream();
//		tree.sync();

		if (i % 100 == 0) {
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
			diff = diff / 1000.0f;

			printf("[%.2f] Update %i items \n", diff, 1 * i);
		}
	}

	std::cout << "Adding users to tree" << std::endl;
	for (int i = 0; i < 100; i++) {
		tree.userStream();
		std::cout << "\nUser " << i + 1 << " added successfully!" << std::endl;
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	diff = diff / 1000.0f;
	diff = diff - tree.updateTime;

	printf("Distance computation %i \n", tree.numDistanceComputions);
	printf("KNN computation %i \n", tree.numKnnComputions);

	printf("Distance computation (per user) = %i \n", (tree.numDistanceComputions)/2000);
	printf("KNN computation Time = %f \n", tree.updateTime);
	printf("Distance computation Time = %f \n", diff);
	printf("Distance computation Time (100 users) = %f \n", diff/20);
	printf("Distance computation Time (500 users) = %f \n", diff / 4);

}

//const std::string& dataset_path, const std::string& extra_arg



void testIRIS(const std::string& dataset_path, const std::string& extra_arg, long cluster_number=150) {

    //std::cout.precision(8);
    //"D:\\hdr_tree\\hdr_tree_0\\hdr_tree_0\\Normalized_WT.dat"

    long initial_user_set;
    long insertion_number;
    long initial_item_set;
    long general_size;
    long general_user_set;
    long general_item_set;
    long dimensions;
    long fanout_;
    long threshold_;
    long cluster_no;

    if(dataset_path=="..\\datasets\\Normalized_WT.dat"){
        initial_user_set=60000;
        insertion_number=20000;
        initial_item_set=125000;
        general_size=250000;
        general_user_set=125000;
        general_item_set=125000;
        dimensions=128;
        fanout_=10;
        threshold_=30;
        cluster_no =cluster_number;
    }else if(dataset_path=="..\\datasets\\cifar.txt"){
        initial_user_set=12000;
        insertion_number=12000;
        initial_item_set=24000;
        general_size=48000;
        general_user_set=24000;
        general_item_set=24000;
        dimensions=512;
        fanout_=20;
        threshold_=30;
        cluster_no = cluster_number;
    }else if(dataset_path=="..\\datasets\\audio.txt"){
        initial_user_set=12500;
        insertion_number=12500;
        initial_item_set=25000;
        general_size=50000;
        general_user_set=25000;
        general_item_set=25000;
        dimensions=192;
        fanout_=20;
        threshold_=30;
        cluster_no = cluster_number;
    }else if(dataset_path=="..\\datasets\\enron.txt"){
        initial_user_set=22500;
        insertion_number=22500;
        initial_item_set=45000;
        general_size=90000;
        general_user_set=45000;
        general_item_set=45000;
        dimensions=1369;
        fanout_=8;
        threshold_=30;
        cluster_no = cluster_number;
    }else if(dataset_path=="..\\datasets\\fashion-mnist-784-euclidean.txt"){
        initial_user_set=15000;
        insertion_number=15000;
        initial_item_set=30000;
        general_size=60000;
        general_user_set=30000;
        general_item_set=30000;
        dimensions=784;
        fanout_=15;
        threshold_=20;
        cluster_no = cluster_number;
    }else if(dataset_path=="..\\datasets\\millionSong.txt"){
        initial_user_set=225000;
        insertion_number=30000;
        initial_item_set=450000;
        general_size=900000;
        general_user_set=450000;
        general_item_set=450000;
        dimensions=420;
        fanout_=8;
        threshold_=30;
        cluster_no = cluster_number;
    }else if(dataset_path=="..\\datasets\\sun.txt"){
        initial_user_set=19000;
        insertion_number=19000;
        initial_item_set=38000;
        general_size=76000;
        general_user_set=38000;
        general_item_set=38000;
        dimensions=512;
        fanout_=10;
        threshold_=30;
        cluster_no =cluster_number;
    }else if(dataset_path=="..\\datasets\\trevi.txt"){
        initial_user_set=22500;
        insertion_number=22500;
        initial_item_set=45000;
        general_size=90000;
        general_user_set=45000;
        general_item_set=45000;
        dimensions=4096;
        fanout_=8;
        threshold_=30;
        cluster_no = cluster_number;
    }else{
        std::cout<<"Please enter the correct dataset name"<<std::endl;
        exit(1);
    }



    //tree.test_cache_effect();

    //tree.analyze_from_file("single_access.txt", "batch_access.txt");



    std::ifstream fin(dataset_path);
    if (!fin.is_open()) {
        std::cerr << "Error: cannot open dataset file: " << dataset_path << "\n";
        exit(1);
    }
    fin.close();

    dataset_path.c_str();
    //idataBase.load("D:\\hdr_tree\\hdr_tree_same_pca\\datasets\\Normalized_WT.dat");
    idataBase.load(dataset_path.c_str());

    //std::cout<<"The distance calculation time is "<<std::endl;

    //idataBase.verbose = false;
	//idataBase.load("D:\\hdr_tree\\hdr_tree_same_pca\\datasets\\Normalized_WT.dat");
	idataBase.verbose = false;

	/*itree.numFeatures = 4;
	itree.checkDuplicates = false;
	itree.numItems = 30;
	itree.verbose = true;
	itree.setData(&idataBase);
	itree.construct(3, 5);*/

//in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree
    tree.numFeatures_i=dimensions;
    tree.checkDuplicates_i=false;
    tree.numItems_i=initial_item_set;
    tree.verbose_i=true;
    tree.setData_i(&idataBase);

    tree.data_i->general_num=general_size;
    tree.data_i->general_num_item=general_item_set;
    tree.data_i->general_num_user=general_user_set;
    tree.data_i->num_user=initial_user_set;
    tree.data_i->num_item=initial_item_set;
    tree.data_i->random_bit=1;

    int delta_dim=tree.construct_i(fanout_,threshold_);   //orginal fanout 8    //original fanout 12


    /*for(auto i:tree.record_sequence){
        std::cout<<*i<<std::endl;
    }*/


    /*int layer_sequence=1;
    int check_dimension;
    float variance_proportion;
    float bias_proportion;
    for(auto cluster_layer_item:tree.dictionary_for_layer_itemcluster){
        std::cout<<"Now print the degree of bias between the distance from the points to the cluster center and the center radius of the "<<layer_sequence<<"th layer of the delta tree"<<std::endl;
        check_dimension=tree.data_i->computeDimension(layer_sequence, tree.L_i);
        variance_proportion=tree.vector_proportion_variance[check_dimension-1];
        layer_sequence=layer_sequence+1;
        for(auto cluster_item:*cluster_layer_item){
            bias_proportion=0;
            for(auto item_distance:cluster_item->Items){
                bias_proportion=bias_proportion+
            }
        }
    }*/














//Form the clusters of items on full dimensions

/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////


    /*tree.form_item_clusters(60);

    std::cout<<"max R="<<tree.max_r_item_cluster<<std::endl;

    //int check_num=0;
    //for(int i=0; i<tree.clusters_item->children.size(); i++){
        //std::cout<<tree.clusters_item->children[i]->radius<<std::endl;
    //}



    tree.complete_tree();*/

/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////

    //tree.bptree_item->PrintLeaves();
    //Sleep(10000);

    //tree.bptree_item->PrintLayerTree();
    //set<float> check;
    /*for(auto item_test:tree.slidingWindow){
        std::cout<<item_test->index_b_plus<<std::endl;
        //check.insert(item_test->index_b_plus);
    }*/
    //std::cout<<check.size()<<std::endl;

    //double a;



    /*int count_c=0;
    for(auto c_item:tree.clusters_item->children){
        std::cout<<" "<<std::endl;
        std::cout<<count_c<<std::endl;
        for(auto item_c: c_item->Items){
            std::cout<<std::get<1>(item_c)->index_b_plus<<" ";
        }


        std::cout<<" "<<std::endl;
        std::cout<<count_c<<std::endl;
        count_c=count_c+1;
    }*/

    /*std::vector<Item*> candidate_list_test;
    std::vector<Item*>* pointer_candidate_test=&(candidate_list_test);
    tree.bptree_item->get_range_item(pointer_candidate_test,0.846289, 2.08039, false, true, true);*/

    /*std::vector<Item*> test_0;
    std::vector<Item*>* pointer=&test_0;

    std::cout<<"Check get range start"<<std::endl;

    tree.bptree_item->get_range_item(pointer, 0,240,true, true);

    std::cout<<"Check get range end"<<std::endl;


    int check_num=0;
    for(auto item_1: test_0){
        check_num=check_num+1;
        std::cout<<item_1<<std::endl;
    }
    std::cout<<"check_num "<<check_num<<std::endl;*/
//Form the clusters of items on full dimensions


//in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree in_one_tree

	std::cout << "\n - - - Finished Contructing Item Tree - - - \n\n" << std::endl;

	//dataBase.load("D:\\hdr_tree\\hdr_tree_same_pca\\datasets\\Normalized_WT.dat");
    dataBase.load(dataset_path.c_str());
	dataBase.verbose = false;

	tree.k = 10;
	tree.windowSize = initial_item_set;
	tree.numFeatures = dimensions;
	tree.checkDuplicates = false;
	tree.numUsers = initial_user_set;
	tree.verbose = true;
	tree.setData(&dataBase);

	tree.data->general_num=general_size;
	tree.data->general_num_user=general_user_set;
	tree.data->num_user=initial_user_set;
	tree.data->num_item=initial_item_set;
	tree.data->random_bit=1;



	tree.construct(10, 50);

















































    //std::cout<<"This is the initialization time for the users "<<tree.init_time<<std::endl; //Check the initialization time
    //std::cout<<"distance_time= "<<tree.distance_time<<std::endl;




/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////

    //tree.construct_sphere(5,7);


/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////
/////////////////////KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, KNN JOIN+, /////////////////////////

    /*for(auto u_id:tree.Users){
        std::cout<<u_id->dknn<<std::endl;
    }*/

    //std::cout<<tree.Users[0]->center.size()<<std::endl;

    /*for(auto i_id:tree.Users[20]->R){
        std::cout<<i_id<<std::endl;
    }*/

    /*long layer_sequence=1;
    long check_dimension;
    float variance_proportion;
    float sum_radius;
    float average_radius;
    for(auto cluster_layer_item:tree.dictionary_for_layer_itemcluster){
        check_dimension=tree.data_i->computeDimension(layer_sequence, tree.L_i);
        variance_proportion=tree.vector_proportion_variance[check_dimension-1];
        sum_radius=0;
        for(auto cluster_item:*cluster_layer_item){

            sum_radius=sum_radius+cluster_item->radius;
            //std::cout<<cluster_item->radius<<std::endl;

        }
        average_radius=sum_radius/(*cluster_layer_item).size();
        std::cout<<"The average radius of the clusters on "<<layer_sequence<<"th layer of the delta tree is "<<average_radius<<std::endl;
        std::cout<<"The number of dimension of "<<layer_sequence<<"th layer of the delta tree is "<<check_dimension<<std::endl;
        std::cout<<"The proportion of variance of "<<layer_sequence<<"th layer of the delta tree is "<<variance_proportion<<std::endl;
        layer_sequence=layer_sequence+1;
    }



    float dknn_sum_0=0;                  //Print the sum of the dknn for the users in the initial state

    for(auto u: tree.Users) {             //Print the sum of the dknn for the users in the initial state
        dknn_sum_0 = dknn_sum_0 + u->dknn;   //Print the sum of the dknn for the users in the initial state
    }

    float average_dknn=dknn_sum_0/tree.Users.size();

    std::cout<<"Print the sum of the dknn for the users in the initial state "<<dknn_sum_0<<std::endl;

    std::cout<<"Print the average dknn for the users in the initial state "<<average_dknn<<std::endl;*/


    /*long layer_sequence=1;
    long check_dimension;
    float variance_proportion;
    float sum_level_cluster_average_distance_radius_proportion;
    float average_level_cluster_average_distance_radius_proportion;
    float sum_cluster_average_radius_distance_proportion;
    float average_cluster_average_radius_distance_proportion;
    float distance_radius_proportion;
    long sum_num_item_cluster_level;
    long average_num_item_cluster_level;
    float sum_radius;
    float average_radius;
    long sum_item_leafnode=0;
    float average_item_leafnode;
    std::vector<float> d_ratio_radius;
    std::vector<float> radius_vector;
    std::vector<float> num_item_average;



    for(auto leaf: tree.itemleafnodelist){
        sum_item_leafnode=sum_item_leafnode+leaf->cluster->Items.size();
    }
    average_item_leafnode=sum_item_leafnode*1.0f/(tree.itemleafnodelist.size()*1.0f);

    std::cout<<"The average number of the items in the leafnodes of delta tree is "<<average_item_leafnode<<std::endl;
    std::cout<<"The sum number of the items in the leafnodes of the delta tree is "<<sum_item_leafnode<<std::endl;
    std::cout<<"The number of leafnodes of the delta tree is "<<tree.itemleafnodelist.size()<<std::endl;


    for(auto cluster_layer_item:tree.dictionary_for_layer_itemcluster){

        check_dimension=tree.data_i->computeDimension(layer_sequence, tree.L_i);
        variance_proportion=tree.vector_proportion_variance[check_dimension-1];

        sum_level_cluster_average_distance_radius_proportion=0;
        sum_num_item_cluster_level=0;
        sum_radius=0;

        for(auto cluster_item:*cluster_layer_item){

            sum_cluster_average_radius_distance_proportion=0;

            if(cluster_item->Items.size()==1){
                average_cluster_average_radius_distance_proportion=1;
            }

            else{

                for(auto item_distance: cluster_item->Items){*/

                    /*if(layer_sequence==3){
                        std::cout<<"The distance to the center is "<<std::get<0>(item_distance)<<std::endl;
                        std::cout<<"The radius is "<<cluster_item->radius<<std::endl;
                        std::cout<<"The proportion is "<<std::get<0>(item_distance)/cluster_item->radius<<std::endl;
                    }*/

                    /*distance_radius_proportion=std::get<0>(item_distance)/cluster_item->radius;

                    sum_cluster_average_radius_distance_proportion=sum_cluster_average_radius_distance_proportion+distance_radius_proportion;

                }

                average_cluster_average_radius_distance_proportion=sum_cluster_average_radius_distance_proportion/cluster_item->Items.size();

            }


            //std::cout<<"The sum proportion is "<<sum_cluster_average_radius_distance_proportion<<std::endl;
            //std::cout<<"The size of the cluster is "<<cluster_item->Items.size()<<std::endl;



            //std::cout<<"The average proportion is "<<average_cluster_average_radius_distance_proportion<<std::endl;

            sum_level_cluster_average_distance_radius_proportion=sum_level_cluster_average_distance_radius_proportion+average_cluster_average_radius_distance_proportion;
            sum_num_item_cluster_level=sum_num_item_cluster_level+cluster_item->Items.size();
            sum_radius=sum_radius+cluster_item->radius;

        }

        average_level_cluster_average_distance_radius_proportion=sum_level_cluster_average_distance_radius_proportion/(*cluster_layer_item).size();
        average_num_item_cluster_level=sum_num_item_cluster_level/(*cluster_layer_item).size();
        average_radius=sum_radius/(*cluster_layer_item).size();

        std::cout<<"The average proportion of the distance between the items to the cluster center in the radius of the clusters on "<<layer_sequence<<"th layer of the delta tree is "<<average_level_cluster_average_distance_radius_proportion<<std::endl;
        d_ratio_radius.emplace_back(average_level_cluster_average_distance_radius_proportion);
        std::cout<<"The number of dimension of "<<layer_sequence<<"th layer of the delta tree is "<<check_dimension<<std::endl;
        std::cout<<"The proportion of variance of "<<layer_sequence<<"th layer of the delta tree is "<<variance_proportion<<std::endl;
        std::cout<<"The average number of the items of the clusters on the "<<layer_sequence<<"th layer of the delta tree is "<<average_num_item_cluster_level<<std::endl;
        num_item_average.emplace_back(average_num_item_cluster_level);
        std::cout<<"The average radius of the clusters on the "<<layer_sequence<<"th layer of the delta tree is "<<average_radius<<std::endl;
        radius_vector.emplace_back(average_radius);
        layer_sequence=layer_sequence+1;
    }

    std::cout<<"The average number of the items in the leafnodes of delta tree is "<<average_item_leafnode<<std::endl;
    std::cout<<"The sum number of the items in the leafnodes of the delta tree is "<<sum_item_leafnode<<std::endl;
    std::cout<<"The number of leafnodes of the delta tree is "<<tree.itemleafnodelist.size()<<std::endl;





    float dknn_sum_0=0;                  //Print the sum of the dknn for the users in the initial state

    for(auto u: tree.Users) {             //Print the sum of the dknn for the users in the initial state
        dknn_sum_0 = dknn_sum_0 + u->dknn;   //Print the sum of the dknn for the users in the initial state
    }

    float average_dknn=dknn_sum_0/tree.Users.size();

    std::cout<<"Print the sum of the dknn for the users in the initial state "<<dknn_sum_0<<std::endl;

    std::cout<<"Print the average dknn for the users in the initial state "<<average_dknn<<std::endl;*/






    /*for(auto i:tree.record_sequence){
        std::cout<<*i<<std::endl;
    }

    for(auto i:tree.record_sequence_user){
        std::cout<<*i<<std::endl;
    }*/

    //tree.calculate_distance_usercluster_itemleaf();
    //tree.calculate_distance_itemcluster_userleaf();






//Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune;

    //tree.initialize_two_dimensional_array();


//Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune; Initialize the check_prune;

//Initialize the time parameters for step time calculation; Initialize the time parameters for step time calculation; Initialize the time parameters for step time calculation;

    /*tree.time_judgelevel=0;
    tree.time_transformtype=0;
    tree.time_calculate_crossprunedistance=0;
    tree.time_traditional_prune=0;
    tree.time_knn_computation=0;
    tree.time_other=0;
    tree.time_compute_knn_cross=0;
    tree.time_compute_sort_list=0;*/

//Initialize the time parameters for step time calculation; Initialize the time parameters for step time calculation; Initialize the time parameters for step time calculation;



//The full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update

    //std::vector<Item*> check_item;
    //tree.bptree_item->get_range_item(&check_item, 120, 130, true, true, true);

















    /*std::cout<<"Check the index of 130.068493 Check the index of 130.068493 Check the index of 130.068493 "<<std::endl;

    //std::cout<<tree.slidingWindow[633]->index_b_plus<<std::endl;

    printf("The key of the 633th item: %f\n", tree.slidingWindow[633]->index_b_plus);*/

    /*for(auto i: tree.slidingWindow){
        tree.bptree_item->Remove(i->index_b_plus);
    }*/

    //tree.bptree_item->Insert(400, tree.slidingWindow[663]);

    /*tree.bptree_item->Remove(tree.slidingWindow[633]->index_b_plus);

    tree.bptree_item->PrintLeaves();*/



    /*std::vector<Item*> check_search;
    tree.bptree_item->get_range_item(&check_search, 130, 131, false, false, true);

    for(auto i:check_search){
        printf("The key of the item: %f\n", i->index_b_plus);
    }*/


//update users, update users, update users, update users, update users, update users, update users, update users, update users, update users, update users


    /*std::chrono::steady_clock::time_point begin_clock_remove_user = std::chrono::steady_clock::now();
    for(int i=0;i<1000;i++){
        tree.update_user();
    }
    std::chrono::steady_clock::time_point end_clock_remove_user = std::chrono::steady_clock::now();
    float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_remove_user - begin_clock_remove_user).count();
    diff = diff / 1000.0f;
    printf("Time for remove 1000 users = %f \n", diff);*/



    /*std::chrono::steady_clock::time_point begin_clock_user_add = std::chrono::steady_clock::now();
    for(int i=0;i<1000;i++){

        //std::cout<<"Start calculating the knn list of the "<<tree.numUsers+i+1<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;
        //std::cout<<"The "<<i+1<<"th user is inserted"<<std::endl;
        tree.update_user_add();

        //std::cout<<"Complete calculating the knn list of the "<<tree.numUsers+i+1<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;
    }
    std::chrono::steady_clock::time_point end_clock_user_add = std::chrono::steady_clock::now();
    float diff_0 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_user_add - begin_clock_user_add).count();
    diff_0 = diff_0 / 1000.0f;
    printf("Time for add 1000 users= %f \n", diff_0);*/



//update users, update users, update users, update users, update users, update users, update users, update users, update users, update users



//update items, update items, update items, update items, update items, update items, update items, update items, update items, update items


    /*tree.projection_time=0;      //For the projection time of HDR tree
    tree.search_time_HDR=0;      //For the search time of HDR tree

    tree.search_time_spheretree=0;    //For the search time of sphere tree*/


    /*tree.num_hdr_cal_users=0;       //For the number of times of calculation of users
    tree.num_hdr_cal_clusters=0;    //For the number of times of calculation of clusters*/



    long cluster_prune_d;
    cluster_prune_d=delta_dim;


///////$$$$$$$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$///////////
///////$$$$$$$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////
///////$$$$$$$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////
///////$$$$$$$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////

    //tree.form_clusters_item(cluster_prune_d, 10000);


    //std::cout<<"Check"<<std::endl;

    //tree.calculate_distance_utree_icluster();

    //std::cout<<"Check"<<std::endl;

    //tree.itemclusters_false();
    //tree.itemcluster_radius_zero();
    //tree.itemcluster_updated_item_clear();
    //tree.clear_updated_itemclusters();

/////////////////////////////////////////////////////////////////////////////////
    tree.form_clusters_user(cluster_prune_d, 150);


    //std::cout<<"Check"<<std::endl;

    tree.calculate_distance_itree_ucluster();

    //std::cout<<"Check"<<std::endl;

    tree.userclusters_false();
    tree.usercluster_radius_zero();
    tree.usercluster_updated_user_clear();
    tree.clear_clusters_haveuserknn();



//////$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$/////////
//////$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////
//////$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////
//////$$$$$$$$$$$$$$$For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster; For batch by cluster;  $$$$$$$$$$$$$$$$$////////

    /*std::cout<<"The number of leafnodes in the delta tree is "<<tree.itemleafnodelist.size()<<std::endl;


    std::cout<<"Start checking the performance of adding items by using different number of dimensions to prune"<<std::endl;

    tree.p1_additem.clear();

    std::vector<float> time_vector;
    std::vector<long> dimensions_vector;
    std::vector<float> variance_proportion;

    long num_dimensions;


    std::cout<<"Now, start show the p2"<<std::endl;
    for(num_dimensions=4;num_dimensions<=64;num_dimensions=num_dimensions+2){

        tree.data->generate_low_pcamatrix(num_dimensions);
        for(auto user:tree.Users){
            user->center_low=tree.data->Mat_low_d*user->center;
        }
        for(auto item:tree.slidingWindow){
            item->v_low=tree.data->Mat_low_d*item->v;
        }

        std::chrono::steady_clock::time_point begin_add_item = std::chrono::steady_clock::now();
        tree.update_item_add_batch_statistic(1500);
        std::chrono::steady_clock::time_point end_add_item = std::chrono::steady_clock::now();


        float diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_add_item - begin_add_item).count();
        diff_time = diff_time/1000000.0;
        time_vector.emplace_back(diff_time);
        dimensions_vector.emplace_back(num_dimensions);
        variance_proportion.emplace_back(tree.vector_proportion_variance[num_dimensions-1]);
    }



    std::cout<<" "<<std::endl;

    float p1_loop=0;
    for(auto p:tree.p1_additem){
        p1_loop=p+p1_loop;
    }
    p1_loop=float(p1_loop/(tree.p1_additem.size()*1.0));

    tree.p1_additem.clear();

    std::cout<<"Now, start show the time"<<std::endl;
    for(auto time:time_vector){
        std::cout<<time<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"Now, start show the dimensions"<<std::endl;
    for(auto d:dimensions_vector){
        std::cout<<d<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"Now, start show the variance proportions"<<std::endl;
    for(auto p:variance_proportion){
        std::cout<<p<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"The p1 is "<<p1_loop<<std::endl;*/




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    tree.data->generate_low_pcamatrix(cluster_prune_d);
    for(auto user:tree.Users){
        user->center_low=tree.data->Mat_low_d*user->center;
    }
    for(auto item:tree.slidingWindow){
        item->v_low=tree.data->Mat_low_d*item->v;
    }

    /*std::cout<<"start test knn by prune"<<std::endl;


    std::cout<<"The used number of pruning dimensions is "<<cluster_prune_d<<std::endl;

    std::cout<<"The proportion of variance is "<<tree.vector_proportion_variance[cluster_prune_d-1]<<std::endl;

    std::chrono::steady_clock::time_point start_clock_test_KNN_prune = std::chrono::steady_clock::now();

    //tree.update_user_add_batchstatistic(30000);
    tree.update_item_add_batch_statistic(30000);

    std::chrono::steady_clock::time_point end_clock_test_KNN_prune = std::chrono::steady_clock::now();


    float diff_test_KNN_prune = std::chrono::duration_cast<std::chrono::microseconds>( end_clock_test_KNN_prune-start_clock_test_KNN_prune ).count();
    diff_test_KNN_prune = diff_test_KNN_prune/1000000.0;

    printf("Time for insertion of 300000 users= %f \n", diff_test_KNN_prune);



    float dknn_sum_test=0;

    for(auto u: tree.Users){
        dknn_sum_test=dknn_sum_test+u->dknn;//std::cout<<u->dknn<<std::endl;
    }



    std::cout<<dknn_sum_test<<std::endl;*/





////////////////////////////////////////////test===================test===================test=================test////////////////////////



    /*std::chrono::steady_clock::time_point start_clock_test = std::chrono::steady_clock::now();

    for(int i=0;i<1500;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;

        //tree.update_item_add();

        //tree.update_item_add();

        tree.update_item_user_tree();

        //tree.update_user_add_deltatree();

        //tree.update_item_delete_doubletree_prune();

        //tree.update_item_knnjoinplus();

        //tree.update_item_user_tree();

    }
    tree.delete_item_batch_bycluster_prune(1500);

    std::chrono::steady_clock::time_point end_clock_test = std::chrono::steady_clock::now();


    float diff_test = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_test - start_clock_test).count();
    diff_test = diff_test/1000000.0;

    printf("Time for testing= %f \n", diff_test);

    float dknn_sum_test=0;

    for(auto u: tree.Users){
        dknn_sum_test=dknn_sum_test+u->dknn;//std::cout<<u->dknn<<std::endl;
    }



    std::cout<<dknn_sum_test<<std::endl;*/




////////////////////////////////////////////test===================test===================test=================test////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



     std::cout<<"Start update"<<std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*tree.distance_time=0;
    tree.distance_time_low_d_nonleaf=0;
    tree.distance_time_low_d_leaf=0;



    std::chrono::steady_clock::time_point start_clock_double_tree_prune = std::chrono::steady_clock::now();


    for(int i=0;i<1000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //std::cout<<"check"<<std::endl;
        //tree.update_user_add_doubletree_prune();
        //tree.update_item_delete_doubletree_prune();
        //tree.update_item_add_doubletree_prune();
    }



    for(int i=0;i<3000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;

        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //tree.update_user_add_doubletree_prune();
        //tree.update_item_delete_doubletree_prune();
        //tree.update_item_add_doubletree_prune();
    }

    for(int i=0;i<10000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //tree.update_user_add_doubletree_prune();
        //tree.update_item_delete_doubletree_prune();
        //tree.update_item_add_doubletree_prune();
    }




    std::chrono::steady_clock::time_point end_clock_double_tree_prune = std::chrono::steady_clock::now();


    float diff_double_tree_prune = std::chrono::duration_cast<std::chrono::microseconds>( end_clock_double_tree_prune- start_clock_double_tree_prune).count();
    diff_double_tree_prune = diff_double_tree_prune/1000000.0;

    printf("Time for using double trees and low-dimensional prune to update= %f \n", diff_double_tree_prune);

    std::cout<<"The distance calculation time of full-dimensional is "<<tree.distance_time<<std::endl;

    std::cout<<"The distance calculation time of low-dimensional on the non-leafnodes is "<<tree.distance_time_low_d_nonleaf<<std::endl;

    std::cout<<"The distance calculation time of low-dimensional on the leafnodes is "<<tree.distance_time_low_d_leaf<<std::endl;*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*tree.distance_time=0;
    tree.distance_time_low_d_nonleaf=0;
    tree.distance_time_low_d_leaf=0;



    std::chrono::steady_clock::time_point start_clock_double_tree_prune_addu = std::chrono::steady_clock::now();


    for(int i=0;i<3000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //std::cout<<"check"<<std::endl;
        //tree.update_user_add_doubletree_prune();
        //tree.update_item_delete_doubletree_prune();
        //tree.update_item_add_doubletree_prune();
        //tree.update_item_add_deltatree();
    }






    std::chrono::steady_clock::time_point end_clock_double_tree_prune_addu = std::chrono::steady_clock::now();


    float diff_double_tree_prune_addu = std::chrono::duration_cast<std::chrono::microseconds>( end_clock_double_tree_prune_addu- start_clock_double_tree_prune_addu).count();
    diff_double_tree_prune_addu = diff_double_tree_prune_addu/1000000.0;

    printf("Time for using double trees and low-dimensional prune to add user= %f \n", diff_double_tree_prune_addu);

    std::cout<<"The distance calculation time of full-dimensional is "<<tree.distance_time<<std::endl;

    std::cout<<"The distance calculation time of low-dimensional on the non-leafnodes is "<<tree.distance_time_low_d_nonleaf<<std::endl;

    std::cout<<"The distance calculation time of low-dimensional on the leafnodes is "<<tree.distance_time_low_d_leaf<<std::endl;*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*std::chrono::steady_clock::time_point start_clock_printempty = std::chrono::steady_clock::now();

    for(int p=0; p<=1000000000; p++){
        printf("");
    }

    std::chrono::steady_clock::time_point end_clock_printempty = std::chrono::steady_clock::now();

    float diff_printempty = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_printempty - start_clock_printempty).count();
    diff_printempty = diff_printempty/1000000.0;

    printf("Time for using single delta tree= %f \n", diff_printempty);*/

    if(extra_arg=="single"){
        std::chrono::steady_clock::time_point start_clock_delta_tree = std::chrono::steady_clock::now();


        for(int i=0;i<insertion_number;i++) {
            //std::cout<<"";
            //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;



            tree.update_user_add_deltatree();
            //tree.update_item_add_deltatree();
            //tree.update_item_deltatree();

        }

        std::chrono::steady_clock::time_point end_clock_delta_tree = std::chrono::steady_clock::now();


        float diff_delta_tree = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_delta_tree - start_clock_delta_tree).count();
        diff_delta_tree = diff_delta_tree/1000000.0;

        printf("Time in seconds for using single delta tree= %f \n", diff_delta_tree);
    }else if(extra_arg=="batch"){
        std::chrono::steady_clock::time_point start_clock_double_tree_clusterbatch = std::chrono::steady_clock::now();

        //tree.time_update_user_add_batch_except_knn=0;
        //tree.time_update_user_add_batch_knn=0;
        //tree.distance_time=0;
        //tree.time_full_dimensional_distance_add_batch_knn=0;
        //tree.time_statistic=0;

        //tree.reset_cache_analysis();
        //tree.real_time_distance_calculation=0;

        //tree.delete_item_batch_bycluster(5000);
        tree.update_user_add_batch_bycluster(insertion_number);
        //tree.add_item_batch_bycluster(8000);
        //tree.add_item_batch_bycluster_paralell(13000);

        //tree.save_access_pattern("batch_access.txt", true);

        std::chrono::steady_clock::time_point end_clock_double_tree_clusterbatch = std::chrono::steady_clock::now();



        float diff_double_tree_clusterbatch = std::chrono::duration_cast<std::chrono::microseconds>( end_clock_double_tree_clusterbatch- start_clock_double_tree_clusterbatch).count();
        diff_double_tree_clusterbatch = diff_double_tree_clusterbatch/1000000.0;

        printf("Time in seconds for using batch strategy to update = %f \n", diff_double_tree_clusterbatch);
    }else{
        std::cout<<"Please enter the correct mode"<<std::endl;
        exit(1);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






    /*std::chrono::steady_clock::time_point start_clock_hdr_tree = std::chrono::steady_clock::now();

    for(int i=0;i<1000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //tree.update_item_user_tree();
        //std::cout<<"check_1"<<std::endl;
        //tree.update_user_add_user_tree();
        //std::cout<<"check_2"<<std::endl;
        //tree.update_item_add_user_tree();
    }

    for(int i=0;i<8000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //tree.update_item_user_tree();

        //tree.update_user_add_user_tree();
        //tree.update_item_add_user_tree();
    }

    for(int i=0;i<10000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //tree.update_item_user_tree();

        //tree.update_user_add_user_tree();
        //tree.update_item_add_user_tree();
    }


    std::chrono::steady_clock::time_point end_clock_hdr_tree = std::chrono::steady_clock::now();


    float diff_hdr_tree = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_hdr_tree - start_clock_hdr_tree).count();
    diff_hdr_tree = diff_hdr_tree/1000000.0;

    printf("Time for using single hdr tree= %f \n", diff_hdr_tree);*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /*std::chrono::steady_clock::time_point start_clock_knnjoinplus = std::chrono::steady_clock::now();

    for(int i=0;i<10000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;



        //tree.update_user_add_knnjoinplus();
        //tree.update_item_knnjoinplus();
        //tree.update_item_add_knnjoinplus();
    }

     for(int i=0;i<10000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;



        //tree.update_user_add_knnjoinplus();
        //tree.update_item_knnjoinplus();
        //tree.update_item_add_knnjoinplus();
    }


    for(int i=0;i<10000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;



        //tree.update_user_add_knnjoinplus();
        //tree.update_item_knnjoinplus();
        //tree.update_item_add_knnjoinplus();
    }




    std::chrono::steady_clock::time_point end_clock_knnjoinplus = std::chrono::steady_clock::now();


    float diff_knnjoinplus = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_knnjoinplus - start_clock_knnjoinplus).count();
    diff_knnjoinplus = diff_knnjoinplus/1000000.0;

    printf("Time for using KNN Join plus= %f \n", diff_knnjoinplus);*/







    //std::cout<<"Thee time used for non-knn computation in batch add user is "<<tree.time_update_user_add_batch_except_knn/1000000000<<" seconds"<<std::endl;

    //std::cout<<"Thee time used for knn computation in batch add user is "<<tree.time_update_user_add_batch_knn/1000000000<<" seconds"<<std::endl;

    //std::cout<<"Thee times of full dimensional distance computation in batch add user is "<<tree.distance_time<<std::endl;

    //std::cout<<"Thee time used for full dimensional distance computation in batch add user is "<<tree.time_full_dimensional_distance_add_batch_knn/1000000000<<" seconds"<<std::endl;


    //std::cout<<"Average time of full dimensional distance computation is "<<(tree.time_full_dimensional_distance_add_batch_knn)/(tree.distance_time*1.0)<<"nanoseconds"<<std::endl;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*std::chrono::steady_clock::time_point start_clock_double_tree = std::chrono::steady_clock::now();

    tree.distance_time=0;
    //tree.distance_time_low_d_nonleaf=0;
    //tree.distance_time_low_d_leaf=0;
    //tree.time_statistic=0;
    //tree.time_leafnode=0;
    //tree.real_time_distance_calculation=0;
    //tree.leafnode_time_delta=0;
    tree.time_update_user_add_exceptknn=0;
    tree.time_update_user_add_knn=0;
    //tree.time_computeknn_project=0;
    tree.time_computeknn_full_distance=0;
    //tree.time_computeknn_sort_1=0;
    //tree.time_low_dimensional_distance=0;
    //tree.time_computeknn_sort_2=0;
    tree.knn_candidate_exceed_bound_proportion.clear();
    tree.rknn_candidate_exceed_bound_proportion.clear();

    tree.vector_unprun_proportion_knn.clear();
    tree.vector_unprun_proportion_rknn.clear();
    tree.insert_user_num=8000;

    float sum_unprun_knn_proportion=0;
    float sum_unprun_rknn_proportion=0;

    float average_unprun_knn_proportion;
    float average_unprun_rknn_proportion;

    tree.vector_dist_rate_bound_n2_knn.clear();
    tree.vector_dist_rate_bound_n2_rknn.clear();

    float sum_expectation_knn=0;
    float sum_variance_knn=0;
    float expectation_knn;
    float variance_knn;

    float sum_expectation_rknn=0;
    float sum_variance_rknn=0;
    float expectation_rknn;
    float variance_rknn;


    //for(auto i=0; i<=300; i++){
        //tree.items_exceed_knn_bound_proportion_statistic.emplace_back(0);
    //}

    //for(auto i=0; i<=800; i++){
        //tree.users_exceed_rknn_bound_proportion_statistic.emplace_back(0);
    //}

    //tree.reset_cache_analysis();

    for(int i=0;i<tree.insert_user_num;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;
        //std::cout<<"1"<<std::endl;
        //tree.update_user_add();
        //std::cout<<"2"<<std::endl;

        //tree.update_item();
        //std::cout<<i<<std::endl;
        tree.update_user_add();
        //std::cout<<"2"<<std::endl;

        //std::cout<<"";


    }


    //tree.save_access_pattern("single_access.txt", false);
    //std::cout<<endl;





    for(int i=0;i<4000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //std::cout<<i<<std::endl;
        //tree.update_item_add();


        //tree.update_item();

        //tree.update_user_add();

    }



    for(int i=0;i<1000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;

        //tree.update_item_add();

        //std::cout<<"\0";


        //tree.update_item();

        //tree.update_user_add();

        //std::cout<<"";

    }


    std::chrono::steady_clock::time_point end_clock_double_tree = std::chrono::steady_clock::now();



    float diff_double_tree = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_double_tree - start_clock_double_tree).count();
    diff_double_tree = diff_double_tree/1000000.0;

    printf("Time for using double trees= %f \n", diff_double_tree);



    std::cout<<"The distance calculation time of full-dimensional is "<<tree.distance_time<<std::endl;
    std::cout<<"The time costed for distance calculation time of full-dimensional is "<<tree.time_computeknn_full_distance/1000000000<<std::endl;
    std::cout<<"The average distance calculation time of full-dimensional is "<<tree.time_computeknn_full_distance/(tree.distance_time*1.0)<<"nanoseconds"<<std::endl;
    std::cout<<"The time used for non-knn computation of adding user is "<<tree.time_update_user_add_exceptknn/1000000000<<" seconds"<<std::endl;
    std::cout<<"Thee time used for knn computation in batch add user is "<<tree.time_update_user_add_knn/1000000000<<" seconds"<<std::endl;*/








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*std::chrono::steady_clock::time_point start_clock_double_tree = std::chrono::steady_clock::now();

    tree.distance_time=0;
    tree.distance_time_low_d_nonleaf=0;
    tree.distance_time_low_d_leaf=0;
    //tree.time_statistic=0;
    //tree.time_leafnode=0;
    //tree.real_time_distance_calculation=0;
    tree.leafnode_time_delta=0;
    tree.time_update_user_add_exceptknn=0;
    tree.time_update_user_add_knn=0;
    tree.time_computeknn_project=0;
    tree.time_computeknn_full_distance=0;
    tree.time_computeknn_sort_1=0;
    tree.time_low_dimensional_distance=0;
    tree.time_computeknn_sort_2=0;
    tree.knn_candidate_exceed_bound_proportion.clear();
    tree.rknn_candidate_exceed_bound_proportion.clear();

    tree.vector_unprun_proportion_knn.clear();
    tree.vector_unprun_proportion_rknn.clear();
    tree.insert_user_num=4000;

    float sum_unprun_knn_proportion=0;
    float sum_unprun_rknn_proportion=0;

    float average_unprun_knn_proportion;
    float average_unprun_rknn_proportion;

    tree.vector_dist_rate_bound_n2_knn.clear();
    tree.vector_dist_rate_bound_n2_rknn.clear();

    float sum_expectation_knn=0;
    float sum_variance_knn=0;
    float expectation_knn;
    float variance_knn;

    float sum_expectation_rknn=0;
    float sum_variance_rknn=0;
    float expectation_rknn;
    float variance_rknn;*/

    /*for(auto i=0; i<=300; i++){
        tree.items_exceed_knn_bound_proportion_statistic.emplace_back(0);
    }

    for(auto i=0; i<=800; i++){
        tree.users_exceed_rknn_bound_proportion_statistic.emplace_back(0);
    }*/

    /*for(int i=0;i<tree.insert_user_num;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;
        //std::cout<<"1"<<std::endl;
        //tree.update_user_add();
        //std::cout<<"2"<<std::endl;

        //tree.update_item();
        //std::cout<<i<<std::endl;
        //tree.update_user_add();
        //std::cout<<"2"<<std::endl;

        //std::cout<<"";


    }

    //std::cout<<endl;





    for(int i=0;i<4000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;


        //std::cout<<i<<std::endl;
        //tree.update_item_add();


        //tree.update_item();

        //tree.update_user_add();

    }



    for(int i=0;i<1000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;

        //tree.update_item_add();

        //std::cout<<"\0";


        //tree.update_item();

        //tree.update_user_add();

        //std::cout<<"";

    }


    std::chrono::steady_clock::time_point end_clock_double_tree = std::chrono::steady_clock::now();



    float diff_double_tree = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_double_tree - start_clock_double_tree).count();
    diff_double_tree = diff_double_tree/1000000.0;

    printf("Time for using double trees= %f \n", diff_double_tree);



    std::cout<<"The distance calculation time of full-dimensional is "<<tree.distance_time<<std::endl;*/

    /*auto smallest_beyond_part=*min_element(tree.knn_candidate_exceed_bound_proportion.begin(),tree.knn_candidate_exceed_bound_proportion.end());

    std::cout<<"The smallest square proportion of the temporary knn bound in the distance between user and item is "<<smallest_beyond_part<<std::endl;

    for(auto proportion: tree.items_exceed_knn_bound_proportion_statistic){
        std::cout<<proportion<<" ";
    }
    std::cout<<""<<std::endl;

    auto smallest_beyond_part_rknn=*min_element(tree.rknn_candidate_exceed_bound_proportion.begin(),tree.rknn_candidate_exceed_bound_proportion.end());

    std::cout<<"The smallest square proportion of the dknn in the distance between user and item is "<<smallest_beyond_part_rknn<<std::endl;

    for(auto proportion: tree.users_exceed_rknn_bound_proportion_statistic){
        std::cout<<proportion<<" ";
    }
    std::cout<<""<<std::endl;*/



    /*for (auto proportion: tree.vector_unprun_proportion_knn){
        sum_unprun_knn_proportion=sum_unprun_knn_proportion+proportion;
    }

    average_unprun_knn_proportion=sum_unprun_knn_proportion/tree.vector_unprun_proportion_knn.size();

    for(auto proportion: tree.vector_unprun_proportion_rknn){
        sum_unprun_rknn_proportion=sum_unprun_rknn_proportion+proportion;
    }

    average_unprun_rknn_proportion=sum_unprun_rknn_proportion/tree.vector_unprun_proportion_rknn.size();

    std::cout<<tree.vector_unprun_proportion_knn.size()<<" "<<sum_unprun_knn_proportion<<std::endl;

    std::cout<<"The average unprun proportion of the knn process is "<<average_unprun_knn_proportion<<std::endl;
    std::cout<<"The average unprun proportion of the rknn process is "<<average_unprun_rknn_proportion<<std::endl;*/

    /*for(auto rate_knn: tree.vector_dist_rate_bound_n2_knn){
        sum_expectation_knn=sum_expectation_knn+rate_knn;
    }

    expectation_knn=sum_expectation_knn/(tree.vector_dist_rate_bound_n2_knn.size()*1.0);

    for(auto rate_rknn: tree.vector_dist_rate_bound_n2_rknn){
        sum_expectation_rknn=sum_expectation_rknn+rate_rknn;
    }

    expectation_rknn=sum_expectation_rknn/(tree.vector_dist_rate_bound_n2_rknn.size()*1.0);

    for(auto rate_knn: tree.vector_dist_rate_bound_n2_knn){
        sum_variance_knn=sum_variance_knn+pow((rate_knn-expectation_knn),2);
    }

    variance_knn=sum_variance_knn/((tree.vector_dist_rate_bound_n2_knn.size()-1)*1.0);

    for(auto rate_rknn: tree.vector_dist_rate_bound_n2_rknn){
        sum_variance_rknn=sum_variance_rknn+pow((rate_rknn-expectation_rknn),2);
    }

    variance_rknn=sum_variance_rknn/((tree.vector_dist_rate_bound_n2_rknn.size()-1)*1.0);*/


    /*auto min_knn_d_rate_b_n2 = std::min_element(tree.vector_dist_rate_bound_n2_knn.begin(), tree.vector_dist_rate_bound_n2_knn.end());
    auto max_knn_d_rate_b_n2 = std::max_element(tree.vector_dist_rate_bound_n2_knn.begin(), tree.vector_dist_rate_bound_n2_knn.end());
    auto min_rknn_d_rate_b_n2 = std::min_element(tree.vector_dist_rate_bound_n2_rknn.begin(), tree.vector_dist_rate_bound_n2_rknn.end());
    auto max_rknn_d_rate_b_n2 = std::max_element(tree.vector_dist_rate_bound_n2_rknn.begin(), tree.vector_dist_rate_bound_n2_rknn.end());

    int num_interval_knn=100;
    int num_interval_rknn=100;

    float interval_knn=(*max_knn_d_rate_b_n2-*min_knn_d_rate_b_n2)/(num_interval_knn*1.0);
    float interval_rknn=(*max_rknn_d_rate_b_n2-*min_rknn_d_rate_b_n2)/(num_interval_rknn*1.0);

    int seq_knn;
    int seq_rknn;

    float sum_loss_knn=0;
    float sum_loss_rknn=0;

    float loss_knn;
    float loss_rknn;

    float middle_point_knn;
    float middle_point_rknn;

    float statistic_density_knn;
    float estimated_density_knn;
    float statistic_density_rknn;
    float estimated_density_rknn;

    std::vector<int> statistic_knn_rate;
    std::vector<int> statistic_rknn_rate;

    for(int k=0;k<num_interval_knn;k++){
        statistic_knn_rate.emplace_back(0);
    }
    for(int k=0;k<num_interval_rknn;k++){
        statistic_rknn_rate.emplace_back(0);
    }


    std::cout<<"interval "<<interval_knn<<std::endl;
    std::cout<<"interval "<<interval_rknn<<std::endl;


    for(auto rate_knn: tree.vector_dist_rate_bound_n2_knn){
        seq_knn=int((rate_knn-*min_knn_d_rate_b_n2)/interval_knn);
        if(seq_knn<=num_interval_knn-1){
            statistic_knn_rate[seq_knn]=statistic_knn_rate[seq_knn]+1;
        }else{
            statistic_knn_rate[statistic_knn_rate.size()-1]=statistic_knn_rate[statistic_knn_rate.size()-1]+1;
        }
    }

    for(auto rate_rknn: tree.vector_dist_rate_bound_n2_rknn){
        seq_rknn=int((rate_rknn-*min_rknn_d_rate_b_n2)/interval_rknn);
        if(seq_rknn<=num_interval_rknn-1){
            statistic_rknn_rate[seq_rknn]=statistic_rknn_rate[seq_rknn]+1;
        }else{
            statistic_rknn_rate[statistic_rknn_rate.size()-1]=statistic_rknn_rate[statistic_rknn_rate.size()-1]+1;
        };
    }*/

    //statistic_rknn_rate[statistic_rknn_rate.size()-1]=statistic_rknn_rate[statistic_rknn_rate.size()-1]+1;

    /*for(int k=0;k<10;k++){
        middle_point_knn=(*min_knn_d_rate_b_n2)+(k*1.0+1/2)*interval_knn;
        estimated_density_knn=(1/std::sqrt(2*M_PI*variance_knn))*(std::exp(-(pow(middle_point_knn-expectation_knn,2))/(2*variance_knn)));
        statistic_density_knn=(statistic_knn_rate[k]*1.0/(1.0*tree.vector_dist_rate_bound_n2_knn.size()))/(interval_knn);
        sum_loss_knn=sum_loss_knn+std::abs(estimated_density_knn-statistic_density_knn)/statistic_density_knn;
        //sum_loss_knn=sum_loss_knn+std::abs((1/std::sqrt(2*M_PI*variance_knn))*(std::exp(-pow((middle_point_knn-expectation_knn),2)/(2*variance_knn)))-statistic_rknn_rate[k]/(interval_knn*tree.vector_dist_rate_bound_n2_knn.size()))/(statistic_rknn_rate[k]/(interval_knn*tree.vector_dist_rate_bound_n2_knn.size()));

    }

    loss_knn=sum_loss_knn/(10*1.0);

    for(int k=0;k<10;k++){
        middle_point_rknn=*min_rknn_d_rate_b_n2+(k*1.0+1/2)*interval_rknn;
        //sum_loss_rknn=sum_loss_rknn+std::abs((1/std::sqrt(2*M_PI*variance_rknn))*(std::exp(-pow((middle_point_rknn-expectation_rknn),2)/(2*variance_rknn)))-statistic_rknn_rate[k]/(interval_rknn*tree.vector_dist_rate_bound_n2_rknn.size()))/(statistic_rknn_rate[k]/(interval_rknn*tree.vector_dist_rate_bound_n2_rknn.size()));
        estimated_density_rknn=(1/std::sqrt(2*M_PI*variance_rknn))*(std::exp(-(pow(middle_point_rknn-expectation_rknn,2))/(2*variance_rknn)));
        statistic_density_rknn=(statistic_rknn_rate[k]*1.0/(tree.vector_dist_rate_bound_n2_rknn.size()*1.0))/(interval_rknn);
        sum_loss_rknn=sum_loss_rknn+std::abs(estimated_density_rknn-statistic_density_rknn)/statistic_density_rknn;
    }

    loss_rknn=sum_loss_rknn/10;*/


    /*std::cout<<"The estimated expectation of the rate between distance square and bound square during knn process is "<<expectation_knn<<std::endl;
    std::cout<<"The estimated expectation of the rate between distance square and bound square during rknn process is "<<expectation_rknn<<std::endl;
    std::cout<<"The estimated variance of the rate between distance square and bound square during knn process is "<<variance_knn<<std::endl;
    std::cout<<"The estimated variance of the rate between distance square and bound square during rknn process is "<<variance_rknn<<std::endl;
    std::cout<<"The largest rate between distance square and bound square during knn process is "<<*max_knn_d_rate_b_n2<<std::endl;
    std::cout<<"The smallest rate between distance square and bound square during knn process is "<<*min_knn_d_rate_b_n2<<std::endl;
    std::cout<<"The largest rate between distance square and bound square during rknn process is "<<*max_rknn_d_rate_b_n2<<std::endl;
    std::cout<<"The smallest rate between distance square and bound square during rknn process is "<<*min_rknn_d_rate_b_n2<<std::endl;
    //std::cout<<"The average loss of knn rate is "<<loss_knn<<std::endl;
    //std::cout<<"The average loss of rknn rate is "<<loss_rknn<<std::endl;


    std::cout<<"Now, print the frequency of square of the rate between distance and bound when kNN search in each interval"<<std::endl;
    for(auto num:statistic_knn_rate){
        float percent=(num*1.0)/(tree.vector_dist_rate_bound_n2_knn.size()*1.0);
        std::cout<<percent<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"Now, print the accumulated frequency of square of the rate between distance and bound when kNN search in each interval"<<std::endl;
    float accumulated_frequency_knn=0;
    for(auto num:statistic_knn_rate){
        float percent=(num*1.0)/(tree.vector_dist_rate_bound_n2_knn.size()*1.0);
        accumulated_frequency_knn=accumulated_frequency_knn+percent;
        std::cout<<accumulated_frequency_knn<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"Now, print the frequency of square of the rate between distance and bound when rkNN search in each interval"<<std::endl;
    for(auto num:statistic_rknn_rate){
        float percent=(num*1.0)/(tree.vector_dist_rate_bound_n2_rknn.size()*1.0);
        std::cout<<percent<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"Now, print the accumulated frequency of square of the rate between distance and bound when rkNN search in each interval"<<std::endl;
    float accumulated_frequency_rknn=0;
    for(auto num:statistic_rknn_rate){
        float percent=(num*1.0)/(tree.vector_dist_rate_bound_n2_rknn.size()*1.0);
        accumulated_frequency_rknn=accumulated_frequency_rknn+percent;
        std::cout<<accumulated_frequency_rknn<<" ";
    }
    std::cout<<" "<<std::endl;


    /*std::ofstream file("D:\\gjz_py_tool\\data.csv");
    for (size_t i = 0; i < tree.vector_dist_rate_bound_n2_knn.size(); ++i) {
        if(tree.vector_dist_rate_bound_n2_knn[i]<=3.7&&tree.vector_dist_rate_bound_n2_knn[i]>=0.9){
            file << tree.vector_dist_rate_bound_n2_knn[i];
            if (i != tree.vector_dist_rate_bound_n2_knn.size() - 1) file << ",";
        }
    }
    file.close();*/

    /*std::ofstream fileknn("D:\\gjz_py_tool\\dataknn.csv");
    if (!fileknn.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    bool first_value_knn = true;
    for (size_t i = 0; i < tree.vector_dist_rate_bound_n2_knn.size(); ++i) {
        double value = tree.vector_dist_rate_bound_n2_knn[i];
        //if (value>1.011265&&value < 6) {           //&& value >= 0.9
            if (!first_value_knn) {
                fileknn << ",";
            }
            fileknn << value;
            first_value_knn = false;
        //}
    }

    fileknn << std::endl;

    fileknn.close();

    std::ofstream filerknn("D:\\gjz_py_tool\\datarknn.csv");
    if (!filerknn.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    bool first_value_rknn = true;
    for (size_t i = 0; i < tree.vector_dist_rate_bound_n2_rknn.size(); ++i) {
        double value = tree.vector_dist_rate_bound_n2_rknn[i];
        //if (value <4.57298) {           //&& value >= 0.9
            if (!first_value_rknn) {
                filerknn << ",";
            }
            filerknn << value;
            first_value_rknn = false;
        //}
    }

    filerknn << std::endl;

    filerknn.close();*/




    //std::cout<<"The distance calculation time of low-dimensional on the non-leafnodes is "<<tree.distance_time_low_d_nonleaf<<std::endl;

    //std::cout<<"The distance calculation time of low-dimensional on the leafnodes is "<<tree.distance_time_low_d_leaf<<std::endl;

    //std::cout<<"The time used for the calculation of the leafnodes is "<<tree.leafnode_time_delta/1000000000<<" seconds"<<std::endl;

    //std::cout<<"The time used for no-knn computation is "<<tree.time_update_user_add_exceptknn/1000000000<<" seconds"<<std::endl;

    //std::cout<<"The time used for knn computation is "<<tree.time_update_user_add_knn/1000000000<<" seconds"<<std::endl;

    //std::cout<<"The time used for projecting in compute knn is "<<tree.time_computeknn_project/1000000000<<" seconds"<<std::endl;

    //std::cout<<"The time used for full dimensional distance computation is "<<tree.time_computeknn_full_distance/1000000000<<" seconds"<<std::endl;

    //std::cout<<"Thee time used for sort 1 in compute knn is "<<tree.time_computeknn_sort_1/1000000000<<" seconds"<<std::endl;

    //std::cout<<"Thee time used for low dimensional distance computation is "<<tree.time_low_dimensional_distance/1000000000<<" seconds"<<std::endl;

    //std::cout<<"Thee time used for sort 2 in compute knn is "<<tree.time_computeknn_sort_2/1000000000<<" seconds"<<std::endl;

    //std::cout<<"The average time for full dimensional distance calcuation is "<<(tree.time_computeknn_full_distance)/(tree.distance_time*1.0)<<"nanoseconds"<<std::endl;


    //std::cout<<"The ratio between the distance and the radius of the clusters are ";

    /*for(auto i:d_ratio_radius){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;

    std::cout<<"The average number of the items in the clusters are ";

    for(auto i:num_item_average){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;

    std::cout<<"The radius of the clusters are ";

    for(auto i:radius_vector){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;

    std::cout<<"The number of calculated clusters on each level is ";

    for(auto i:tree.calculated_clusters_layer){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;

    std::cout<<"The number of pruned clusters on each level is ";

    for(auto i:tree.pruned_clusters_layer){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;

    std::cout<<"The number of pruned items on each level is ";

    for(auto i:tree.pruned_items_layer){
        std::cout<<i<<" ";
    }

    std::cout<<""<<std::endl;*/


    //tree.time_statistic=tree.time_statistic/1000000.0;

    //std::cout<<"The time cost for search is "<<tree.time_statistic<<std::endl;

    //tree.time_leafnode=tree.time_leafnode/1000000.0;

    //std::cout<<"The time cost for leafnodes is "<<tree.time_leafnode<<std::endl;

    //tree.real_time_distance_calculation=tree.real_time_distance_calculation/1000000000;

    //std::cout<<"The time cost for distance calculation is "<<tree.real_time_distance_calculation<<std::endl;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /*std::chrono::steady_clock::time_point start_clock_double_tree_addu = std::chrono::steady_clock::now();

    tree.distance_time=0;
    tree.distance_time_low_d_nonleaf=0;
    tree.distance_time_low_d_leaf=0;
    //tree.time_statistic=0;
    //tree.time_leafnode=0;
    //tree.real_time_distance_calculation=0;
    tree.leafnode_time_delta=0;
    tree.time_update_user_add_exceptknn=0;
    tree.time_update_user_add_knn=0;
    tree.time_computeknn_project=0;
    tree.time_computeknn_full_distance=0;
    tree.time_computeknn_sort_1=0;
    tree.time_low_dimensional_distance=0;
    tree.time_computeknn_sort_2=0;
    tree.knn_candidate_exceed_bound_proportion.clear();
    tree.rknn_candidate_exceed_bound_proportion.clear();

    //for(auto i=0; i<=300; i++){
        //tree.items_exceed_knn_bound_proportion_statistic.emplace_back(0);
    //}

    //for(auto i=0; i<=800; i++){
        //tree.users_exceed_rknn_bound_proportion_statistic.emplace_back(0);
    //}

    for(int i=0;i<1000;i++) {
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;
        //std::cout<<"1"<<std::endl;
        //tree.update_item_add();
        //std::cout<<"2"<<std::endl;

        //tree.update_item();
        //std::cout<<"1"<<std::endl;
        tree.update_user_add();
        //std::cout<<"2"<<std::endl;

        //std::cout<<"";


    }




    std::chrono::steady_clock::time_point end_clock_double_tree_addu = std::chrono::steady_clock::now();



    float diff_double_tree_addu = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_double_tree_addu - start_clock_double_tree_addu).count();
    diff_double_tree_addu = diff_double_tree_addu/1000000.0;

    printf("Time for using double trees to add user = %f \n", diff_double_tree_addu);*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    //std::cout<<"The distance calculation time is "<<tree.distance_time<<std::endl;

    //tree.time_statistic=tree.time_statistic/1000000.0;

    //std::cout<<"The time cost for rknn search by batch is "<<tree.time_statistic<<std::endl;

    //tree.real_time_distance_calculation=tree.real_time_distance_calculation/1000000000;

    //std::cout<<"The time cost for distance calculation is "<<tree.real_time_distance_calculation<<std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    /*std::chrono::steady_clock::time_point start_clock_double_tree_prune_batchcluster = std::chrono::steady_clock::now();

    //tree.delete_item_batch_bycluster_prune(10000);
    //tree.update_user_add_batch_bycluster_prune(20000);
    //tree.add_item_batch_bycluster_prune(20000);


    std::chrono::steady_clock::time_point end_clock_double_tree_prune_batchcluster = std::chrono::steady_clock::now();


    float diff_double_tree_prune_batchcluster = std::chrono::duration_cast<std::chrono::microseconds>( end_clock_double_tree_prune_batchcluster- start_clock_double_tree_prune_batchcluster).count();
    diff_double_tree_prune_batchcluster = diff_double_tree_prune_batchcluster/1000000.0;

    printf("Time for using double trees and low-dimensional prune and clusters to batch update= %f \n", diff_double_tree_prune_batchcluster);*/





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    float dknn_sum=0;

    for(auto u: tree.Users){
        dknn_sum=dknn_sum+u->dknn;//std::cout<<u->dknn<<std::endl;
    }



    std::cout<<"The sum of the distance from the k-nearest neighbor to the query points across the whole query set after inserton is "<<dknn_sum<<", this is for checking whether the correctness of the batch update, the results of the point-wise and the batch methods should be the same."<<std::endl;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /*std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();


    //tree.p1_additem.clear();

    //tree.add_item_batch_bycluster(1500);

    //tree.delete_item_batch_bycluster(3000);

    tree.update_user_add_batch_bycluster(3000);

    //tree.update_user_add_batchversion(1000);

    //tree.add_item_batch(3000);

    //tree.update_item_add_naive(5);

    //tree.update_item_add_assemble(3000);*/



    /*for(int i=0;i<3000;i++){
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        //std::cout<<"The "<<i+1<<" th item proccessed"<<std::endl;

        //tree.update_item_add();

        //tree.update_item_add();

        //tree.update_item();

        tree.update_user_add();
    }*/
    /*std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();

    //std::cout<<"totally meet "<<tree.meet_clusters_rknn<<" clusters, and prune "<<tree.prune_clusters_rknn<<" clusters, and calculate "<<tree.notprune_clusters_rknn<<" clusters."<<std::endl;

    //Check the time precision; Check the time precision; Check the time precision; Check the time precision; Check the time precision; Check the time precision;
    //int check_time=0;
    //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    //check_time=check_time+1;
    //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    //Check the time precision; Check the time precision; Check the time precision; Check the time precision; Check the time precision; Check the time precision;

    float p1=0;
    for(auto p:tree.p1_additem){
        p1=p+p1;
    }
    p1=float(p1/(tree.p1_additem.size()*1.0));
    std::cout<<"The p1 is "<<p1<<std::endl;

    float diff_1 = std::chrono::duration_cast<std::chrono::microseconds>(end_clock_item_remove - begin_clock_item_remove).count();
    diff_1 = diff_1/1000000.0;

    //std::cout<<"The number of itemclusters visited is "<<tree.check_cluster_visited<<std::endl;
    //std::cout<<"The number of itemclusters being pruned by cross is "<<tree.check_cluster_pruned_bycross<<std::endl;
    //std::cout<<"The number of itemclusters being pruned by traditional is "<<tree.check_cluster_pruned_traditional<<std::endl;

    printf("Time for remove 1000 items= %f \n", diff_1);*/







    /*std::cout<<"The time for level calculation is "<<tree.time_judgelevel/1000000000.0f<<std::endl;
    std::cout<<"The time for type transformation is "<<tree.time_transformtype/1000000000.0f<<std::endl;
    std::cout<<"The time for calculation of cross prune distance is "<<tree.time_calculate_crossprunedistance/1000000000.0f<<std::endl;
    std::cout<<"The time for calculation of traditional prune distance is "<<tree.time_traditional_prune/1000000000.0f<<std::endl;
    std::cout<<"The time for calculation of knn is "<<tree.time_knn_computation/1000000000.0f<<std::endl;
    std::cout<<"The time for other process is "<<tree.time_other/1000000000.0f<<std::endl;
    std::cout<<"The time for compute knn cross is "<<tree.time_compute_knn_cross/1000000000.0f<<std::endl;
    std::cout<<"The time for compute the head of the compute knn cross function ia "<<tree.time_compute_knn_cross_head/1000000000.0f<<std::endl;
    std::cout<<"The time for sort the candidate item cluster list is "<<tree.time_compute_sort_list/1000000000.0f<<std::endl;*/
    /*std::cout<<"Times of calculation of users "<<tree.num_sphere_cal_users<<std::endl;
    std::cout<<"Times of calculation of clusters "<<tree.num_sphere_cal_clusters<<std::endl;*/



    /*int count_user=1;
    for(auto user_test_0: tree.Users){
        std::cout<<"The size of the knn list of the "<<count_user<<"th user"<<std::endl;
        std::vector<double> transformed;
        for (auto item_index:user_test_0->R){
            transformed.emplace_back(item_index->index_b_plus);
        }
        std::set<double>S(transformed.begin(), transformed.end());
        std::cout<<S.size()<<std::endl;
        std::cout<<"End print the size of the knn list of the "<<count_user<<"th user"<<std::endl;
        std::cout<<" "<<std::endl;
        count_user=count_user+1;
    }*/






    /*std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();
    for(int i=0;i<1000;i++){

        //std::cout<<"The "<<i+1<<" time of update"<<std::endl;
        tree.update_item_add();
    }

    //tree.add_item_batch(1000);



    std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();
    float diff_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();
    diff_2 = diff_2 / 1000.0f;
    printf("Time for add 1000 items= %f \n", diff_2);*/


    /*printf("Time for projection= %f \n", tree.projection_time);
    printf("Time for search on HDR tree= %f \n", tree.search_time_HDR);
    printf("Time for search on sphere tree= %f \n", tree.search_time_spheretree);*/

    /*float dknn_sum=0;                                              // original;  original;  original;  original;  original;  original;  original;  original;
                                                                    // original;  original;  original;  original;  original;  original;  original;  original;
    for(auto u: tree.Users){                                       // original;  original;  original;  original;  original;  original;  original;  original;
        dknn_sum=dknn_sum+u->dknn;//std::cout<<u->dknn<<std::endl; // original;  original;  original;  original;  original;  original;  original;  original;
    }                                                              // original;  original;  original;  original;  original;  original;  original;  original;
                                                                   // original;  original;  original;  original;  original;  original;  original;  original;
    printf("Time for general process= %f \n", diff_1);             // original;  original;  original;  original;  original;  original;  original;  original;
                                                                   // original;  original;  original;  original;  original;  original;  original;  original;
    std::cout<<dknn_sum<<std::endl;*/                                // original;  original;  original;  original;  original;  original;  original;  original;

    //std::cout<<tree.check_prune<<std::endl;


//update items, update items, update items, update items, update items, update items, update items, update items, update items, update items

//check clusters item radius, check clusters item radius, check clusters item radius, check clusters item radius, check clusters item radius,


    /*for(auto u_id:tree.Users){
        std::cout<<u_id->dknn<<std::endl;
    }*/

    /*std::vector<double> radius_clusters;
    double max_r;
    for(auto ic: *tree.clusters_item){
        radius_clusters.emplace_back(ic->radius);

    }
    auto mp=std::max_element(radius_clusters.begin(), radius_clusters.end());
    max_r=radius_clusters[mp-radius_clusters.begin()];
    std::cout<<"This is the max_r of the clusters for the item indexing structure "<<max_r<<std::endl;*/

//check clusters item radius, check clusters item radius, check clusters item radius, check clusters item radius, check clusters item radius,

//The full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update, the full dynamic update


//Check sliding window and Items_i//Check sliding window and Items_i//Check sliding window and Items_i//Check sliding window and Items_i



    /*if(tree.slidingWindow.size()==tree.Items_i.size()){
        std::cout<<"The slidingwindow correctly catch the size of the items"<<std::endl;
    }
    int match_number=0;
    for(int i=0;i<tree.slidingWindow.size();i++){
       if(tree.slidingWindow[i]==tree.Items_i[i]){
           match_number=match_number+1;
       }
    }
    if(match_number==tree.slidingWindow.size()){
        std::cout<<"Success"<<std::endl;
    }*/




//Check sliding window and Items_i//Check sliding window and Items_i//Check sliding window and Items_i//Check sliding window and Items_i


//Check if sliding window and the tree get the same knn//Check if sliding window and the tree get the same knn//Check if sliding window and the tree get the same knn
    /*for(int i=0; i<tree.Users.size(); i++){
        std::cout<<i+1<<" "<<tree.Users[i]->dknn<<" "<<tree.Users[i]->R.size()<<std::endl;
    }*/






//Check if sliding window and the tree get the same knn//Check if sliding window and the tree get the same knn//Check if sliding window and the tree get the same knn

/*	std::cout << "\n- - - Finished Contructing User Tree - - -\n\n" << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	tree.numDistanceComputions = 0;
	tree.numKnnComputions = 0;
	tree.updateTime = 0;

	std::cout << "Adding items to tree" << std::endl;
	for (int i = 0; i < 20; i++) {
		tree.stream();
	}

	std::cout << "Adding users to tree" << std::endl;
	for (int i = 0; i < 10; i++) {
		tree.userStream();
		std::cout << "User " << i+1 << " added successfully!" << std::endl;
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	diff = diff / 1000.0f;
	diff = diff - tree.updateTime;

	printf("Distance computation %i \n", tree.numDistanceComputions);
	//printf("KNN computation %i \n", tree.numKnnComputions);

	printf("Distance computation (per user) = %i \n", (tree.numDistanceComputions) / 2000);
	printf("KNN computation Time = %f \n", tree.updateTime);
	printf("Distance computation Time = %f \n", diff);*/

//The content below is added later//The content below is added later//The content below is added later//The content below is added later//


    /*std::cout<<"This is the computation time of distance when initialization "<<tree.computation_of_distance_init<<std::endl;
    std::cout<<"This is the dimension of users "<<tree.Users[0]->center<<std::endl;
    //std::cout<<"This is the dimension of users "<<tree.Users[0]->center.size()<<std::endl;
    std::cout<<"This is the dimension of users "<<tree.data->U.row(0)<<std::endl;
    std::cout<<"This is the dimension of items "<<tree.Items_i[0]->v<<std::endl;
    std::cout<<"This is the dimension of items "<<tree.data_i->I.row(0)<<std::endl;
    if(tree.slidingWindow.size()==tree.Items_i.size()){
        std::cout<<"The slidingwindow correctly catch the size of the items"<<std::endl;
    }*/
    /*int match_number=0;
    for(int i=0;i<tree.slidingWindow.size();i++){
        std::cout<<tree.slidingWindow[i]->v<<std::endl;
        std::cout<<tree.Items_i[i]->v<<std::endl;
    }
    if(match_number==tree.slidingWindow.size()){
        std::cout<<"Success"<<std::endl;
    }*/
    /*std::cout<<"This is the 4998th item in slidingwindow "<<tree.slidingWindow[4997]->v<<std::endl<<"Saparate"<<std::endl;
    std::cout<<"This is the 4998th item in Items_i "<<tree.Items_i[4997]->v<<std::endl<<"Saparate"<<std::endl;
    std::cout<<"This is the 6003th item in slidingwindow "<<tree.slidingWindow[6002]->v<<std::endl<<"Saparate"<<std::endl;
    std::cout<<"This is the 6003th item in Items_i "<<tree.Items_i[6002]->v<<std::endl<<"Saparate"<<std::endl;*/






    /*tree.build_clustersu(5);

    tree.complete_little_trees(2, 4, 60);

    std::cout<<"Print the sum of the dknn for the users in the initial state "<<dknn_sum_0<<std::endl;

    std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();
    for(int i=0;i<1000;i++){
        //std::cout<<"The "<<i<<" time of update"<<std::endl;
        tree.update_item_multipletree();
    }
    std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();
    float diff_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
    diff_1 = diff_1 / 1000.0f;
    printf("Time for remove 1000 items= %f \n", diff_1);


    std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();
    for(int i=0;i<1000;i++){

        //std::cout<<"The "<<i+1<<" time of update"<<std::endl;
        tree.update_item_add_user_tree();
    }
    std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();
    float diff_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();
    diff_2 = diff_2 / 1000.0f;
    printf("Time for add 1000 items= %f \n", diff_2);

    float dknn_sum=0;

    for(auto u: tree.Users){
        dknn_sum=dknn_sum+u->dknn;//std::cout<<u->dknn<<std::endl;
    }

    printf("Time for general process= %f \n", diff_1+diff_2);

    std::cout<<dknn_sum<<std::endl;*/






}

//int argc, char** argv

/*if (argc < 2) {
std::cerr << "Usage: " << argv[0] << " <dataset_path>\n";
return 1;
}*/
//std::string dataset_path = argv[1];
//std::string mode = argv[2];


// The first function called when program starts executing
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <dataset_path>\n";
        return 1;
    }
    std::string dataset_path = argv[1];
    std::string mode         = argv[2];
    long cluster_number      = (argc >= 4) ? std::stol(argv[3]) : 150;

    testIRIS(dataset_path, mode, cluster_number);

    return 0;
}

