#include "HDR_Tree.h"
#include <atomic>
//#include "User.cpp"

HDR_Tree::HDR_Tree()
{
}

HDR_Tree::~HDR_Tree()
{
}

void HDR_Tree::setData(DataBase* dataBase) {
	data = dataBase;
}

void HDR_Tree::construct(long fanout, long threshold) {

	std::cout << "Constructing HDR Tree" << std::endl;
	std::cout << "Users: " << numUsers << std::endl;
	std::cout << "Features: " << numFeatures << std::endl;
	std::cout << "Window: " << windowSize << std::endl;
	std::cout << "Fanout: " << fanout << std::endl << std::endl;



	init();

	L = log(data->U.size()) / log(fanout);
	std::cout << "Estimated Height: " << L << " levels." << std::endl;

	data->initTransform(L);

	//int row_0;

	/*for(row_0=0; row_0<=data->PrincipalComponents.size()-1; row_0++){
	    std::cout<<std::get<1>(data->PrincipalComponents[row_0])<<std::endl;
	}*/


	//data->calculate_transform_all();

	/*int row_1;

    for(row_1=0; row_1<=data->T_all.size()-1; row_1++){
        std::cout<<"The "<<row_1+1<<"d partial PCA matrix"<<std::endl;
        std::cout<<data->T_all[row_1].size()<<std::endl;
    }*/



	for (auto proportion: data->propartion_variance){
	    vector_proportion_variance.emplace_back(proportion);
	}

	std::cout<<"Show the proportion of the sum of the variance of the dimensions below, there are "<<vector_proportion_variance.size()<<" dimensions"<<std::endl;
	for(auto proportion:vector_proportion_variance){
	    std::cout<<proportion<<" ";
	}
	std::cout<<" "<<std::endl;


	long sequence=0;  //For recording the sequence of the leafnodes. For recording the sequence of the leafnodes. For recording the sequence of the leafnodes.


    /*for(auto u_ini: Users){
        projectUser(u_ini);
        for(int count_d=0; count_d<L; count_d=count_d+1){
            u_ini->u_low_d.emplace_back(uTrans[count_d]);
        }

        //u_ini->center_low=data->Mat_low_d*u_ini->center;

        projectUser_item(u_ini);
        for(auto u_lowd_item:Trans_u_i){
            u_ini->u_low_d_i.emplace_back(u_lowd_item);
        }

    }*/


    /*for(auto i_ini: slidingWindow){
        projectItem(i_ini);
        for(int count_d_0=0; count_d_0<L_i; count_d_0=count_d_0+1){
            i_ini->i_low_d.emplace_back(iTrans[count_d_0]);
        }

        //i_ini->v_low=data->Mat_low_d*i_ini->v;

    }*/

    std::cout<<"check"<<std::endl;

    init_half_behind();



    //init_half_behind_2();


    //init_half_behind_loop(13);




	long numClusters = 0;


    auto first_layer_userclusters=new std::vector<Cluster*>;


	auto C = new Clusters(Users, fanout, data->T[0]);  //0->5

    long sequense_user_cluster_i_layer=0;



	for (auto Cj : *C) {

		if (Cj->Users.size() > 0) {
			Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
			for (auto t : Cj->Users) {
				Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
			}
		}

		numClusters += 1;
		Cj->l = 1;

		/*for(auto c_item_0:*(dictionary_for_layer_itemcluster[Cj->l-1])){
            (Cj->distance_cross_u_t).emplace_back((Cj->center-c_item_0->center).norm());
		}*/

        Cluster_origin* belong_cluster_root=dynamic_cast<Cluster_origin*>(Cj);

        for(auto user_root: Cj->Users){

            std::get<1>(user_root)->list_cluster_belong.emplace_back(belong_cluster_root);
        }

        Cj=dynamic_cast<Cluster*>(belong_cluster_root);


        Cj->u_cluster_sequense=sequense_user_cluster_i_layer;

        sequense_user_cluster_i_layer=sequense_user_cluster_i_layer+1;

        first_layer_userclusters->emplace_back(Cj);
	}

    dictionary_for_layer_usercluster.emplace_back(first_layer_userclusters);

    auto b_sequense=new int;

    *b_sequense=(*first_layer_userclusters).size();

    record_sequence_user.emplace_back(b_sequense);


    //order_the_non_leafnode=0;


	root = new NonLeafNode(C, 1, false);


	//order_the_non_leafnode=order_the_non_leafnode+1;

	std::queue<Node *> q;

	q.push(root);

	long maxL = 1;              //Here the maxL should be 1; Here the maxL should be 1; Here the maxL should be 1; Here the maxL should be 1; Here the maxL should be 1;

    //std::cout<<"check_1"<<std::endl;

	while (!q.empty()) {
		auto node = dynamic_cast<NonLeafNode *>(q.front());
		q.pop();

		auto Cp = node->clusters;

		for (auto Cj : Cp) {

			if (Cj->number < threshold) {

			    //std::cout<<"Leafnode start"<<std::endl;

				auto LN = new LeafNode(Cj, false);
				node->children.emplace_back(LN);
				Cj->ptr = LN;

                

				
				for(auto user_leaf: LN->cluster->Users){
				    std::get<1>(user_leaf)->belong_leaf=LN;

				}

				//Recording the sequence of the leafnodes, and put it into the list. Recording the sequence of the leafnodes, and put it into the list.
				if(LN->cluster->Users.size()!=0){
                    LN->sequence=sequence;
                    sequence=sequence+1;
                    leafnodelist.emplace_back(LN);
				}

				//Recording the sequence of the leafnodes, and put it into the list. Recording the sequence of the leafnodes, and put it into the list.



                //std::cout<<"Leafnode end"<<std::endl;
			}
			else {

                //std::cout<<"nonLeafnode start"<<std::endl;

				auto level = node->l + 1;

				//std::cout<<"level is "<<level<<std::endl;

                if(level>maxL){

                    //std::cout<<"Start make a new layer of nonleafnode"<<std::endl;

                    auto b_sequense=new int;
                    *b_sequense=0;
                    record_sequence_user.emplace_back(b_sequense);
                    auto middle_layer_userclusters=new std::vector<Cluster*>;
                    dictionary_for_layer_usercluster.emplace_back(middle_layer_userclusters);

                    //std::cout<<"Finish building a new layer of nonleafnode"<<std::endl;

                }



				maxL = std::max(maxL, level);

                //std::cout<<"Now the max height is "<<maxL<<std::endl;


				if(level>L){
				    std::cout<<"Excceed the estimated level"<<std::endl;
				}

				//std::cout<<"Start making a new group of clusters"<<std::endl;

				Clusters* Cpp;

				/*if(data->T[level-1].rows()>=19){
				    if(Cj->number>=fanout){
                        Cpp = new Clusters(Cj, fanout, data->T_all[18]);
				    }else{
                        Cpp = new Clusters(Cj, Cj->number, data->T_all[18]);
				    }
				}*/
				//else{
                    if(Cj->number>=fanout){
                        Cpp = new Clusters(Cj, fanout, data->T[level-1]);
                    }else{
                        Cpp = new Clusters(Cj, Cj->number, data->T[level-1]);
                    }
                    //Cpp = new Clusters(Cj, fanout, data->T[level-1]);   //level-1->5
				//}

                //std::cout<<"Finish making a new group of clusters"<<std::endl;

				for (auto Cj : *Cpp) {

					if (Cj->Users.size() > 0) {
						Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
						for (auto t : Cj->Users) {
							Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
						}
					}

					numClusters += 1;
					Cj->l = level;

//This is for recording the sequence of the newly produced clusters, and storeing the newly produced clusters. This is for recording the sequence of the newly produced clusters, and storeing the newly produced clusters.

                    //std::cout<<"Start adding the traversed cluster into the layer and recording its sequence"<<std::endl;

                    dictionary_for_layer_usercluster[level-1]->emplace_back(Cj);
                    Cj->u_cluster_sequense=(*record_sequence_user[level-1]);
                    (*record_sequence_user[level-1])=(*record_sequence_user[level-1])+1;

//This is for recording the sequence of the newly produced clusters, and storeing the newly produced clusters. This is for recording the sequence of the newly produced clusters, and storeing the newly produced clusters.

                    //std::cout<<"Finish adding the traversed cluster into the layer and recording its sequence"<<std::endl;

                    /*for(auto c_item_1:*(dictionary_for_layer_itemcluster[Cj->l-1])){
                        (Cj->distance_cross_u_t).emplace_back((Cj->center-c_item_1->center).norm());
                    }*/

                    Cluster_origin* belong_cluster_inter=dynamic_cast<Cluster_origin*>(Cj);

                    for(auto user_inter: Cj->Users){

                        std::get<1>(user_inter)->list_cluster_belong.emplace_back(belong_cluster_inter);
                    }

                    Cj=dynamic_cast<Cluster*>(belong_cluster_inter);
				}

				auto NLN = new NonLeafNode(Cpp, level, false);

                //order_the_non_leafnode=order_the_non_leafnode+1;

				node->children.emplace_back(NLN);
				Cj->ptr = NLN;
				q.push(NLN);

                //std::cout<<"nonLeafnode end"<<std::endl;

                //std::cout<<"There are "<<order_the_non_leafnode<<" nonleafnodes"<<std::endl;

			}
		}
	}

	std::cout << "Actual Height: " << maxL << " levels." << std::endl;

	height_hdr=maxL;
	dim_hdr=data->computeDimensionForward(maxL, L);

    std::cout<<"The maxmal number of dimensions of the HDR tree is "<<dim_hdr<<std::endl;

	std::cout << "Clusters: " << numClusters << std::endl;

//	for (int i = 0; i < 100; i++) {
//		Users[0]->update(slidingWindow[i], Mode::Add);
//	}

//	for (int i = 0; i < 100; i++) {
//		if (Users[0]->dist(slidingWindow[i]) < Users[0]->dknn) {
//			Users[0]->update(slidingWindow[i], Mode::Remove);
//			break;
//		}
//	}


//	fillTree();

}


void HDR_Tree::init() {

    data->transformData(numFeatures, checkDuplicates);


//Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA;

    //data->generateUser(100000);

//Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA;

//Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA;

    data->generate_general_set();



    data->generate_user_set();



    data->generate_pca_matrix();

//Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA;


    data->computePCA();


    data->generate_low_pcamatrix(20);


    /*for (long i = 0; i < windowSize; i++) {
//		std::cout << i << "/" << data->D.size() << std::endl;
        auto dat = data->D[i];
        Item* I = new Item(dat);
        slidingWindow.emplace_back(I);
    }*/

//switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow





    /*for (long i = 0; i < windowSize; i++) {
//		std::cout << i << "/" << data->D.size() << std::endl;
        auto dat = data_i->I.row(i);
        Item* I = new Item(dat);
        slidingWindow.emplace_back(I);
    }*/






//switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow switch_slidingwindow

    for (long i = 0; i < numUsers; i++) {
        auto usr = new User(data->U.row(i), k);
        Users.emplace_back(usr);
    }












//The content below is moved to the init_half_behind function; The content below is moved to the init_half_behind function; The content below is moved to the init_half_behind function;









/*	add_user_num=0;

//	initUsersSerial();

    std::chrono::steady_clock::time_point begin_clock = std::chrono::steady_clock::now();

	computation_of_distance_init=0;
//---------------------------------------do not use thread--------------------------------do not use thread-----------------------------




//---------------------------------------do not use thread--------------------------------do not use thread------------------------------
    distance_time=0;
    //int trace_no_user=0;

    //int count_user=1;

    for(int i=0; i<Users.size(); i++){
        //Users[i]->computeKNN(slidingWindow);
        //

        userComputeKNN(itemRoot_i, Users[i]);

        //std::cout<<trace_no_user<<std::endl;

        //std::cout<<"Start calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //user_computeknn_id(Users[i], 0.02,0.02, false);

        //std::cout<<"Complete calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //count_user=count_user+1;
        //trace_no_user=trace_no_user+1;
        //Users[i]->computeKNN_sort(slidingWindow);

        //Users[i]->computeKNN(slidingWindow);
    }*/

    /*int count_u=1;

    for(auto u: Users){
        std::cout<<"The knn list of the "<<count_u<<"th user"<<std::endl;
        for(auto i:u->R){
            std::cout<<i->v<<std::endl;
            std::cout<<" "<<std::endl;
        }
        count_u=count_u+1;
    }*/





/*//---------------------------------------use thread---------------------------------------use thread--------------------------------------
	//initUsersParalell();
//---------------------------------------use thread---------------------------------------use thread--------------------------------------
    std::chrono::steady_clock::time_point end_clock = std::chrono::steady_clock::now();
    float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - begin_clock).count();
    diff = diff / 1000.0f;
    printf("Initialization Time = %f \n", diff);

    init_time=diff;

	itemIndex = windowSize;

//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//	diff = diff / 1000.0f;

//	printf("Cluster Initialized in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));*/

}



void HDR_Tree::init_half_behind(){

    add_user_num=0;

//	initUsersSerial();

    std::chrono::steady_clock::time_point begin_clock = std::chrono::steady_clock::now();

    computation_of_distance_init=0;
//---------------------------------------do not use thread--------------------------------do not use thread-----------------------------

    //std::cout<<"check"<<std::endl;


//---------------------------------------do not use thread--------------------------------do not use thread------------------------------
    distance_time=0;
    //int trace_no_user=0;

    //int count_user=1;

    //int i=0;


    /*for(int i=0; i<Users.size(); i++){
        //Users[i]->computeKNN(slidingWindow);
        //

        userComputeKNN_2(itemRoot_i, Users[i]);

        //std::cout<<" "<<std::endl;

        //std::cout<<i+1<<std::endl;
        //i=i+1;

        //std::cout<<trace_no_user<<std::endl;

        //std::cout<<"Start calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //user_computeknn_id(Users[i], 0.02,0.02, false);

        //std::cout<<"Complete calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //count_user=count_user+1;
        //trace_no_user=trace_no_user+1;
        //Users[i]->computeKNN_sort(slidingWindow);

        //Users[i]->computeKNN(slidingWindow);
    }*/

    /*int count_u=1;

    for(auto u: Users){
        std::cout<<"The knn list of the "<<count_u<<"th user"<<std::endl;
        for(auto i:u->R){
            std::cout<<i->v<<std::endl;
            std::cout<<" "<<std::endl;
        }
        count_u=count_u+1;
    }*/



    //std::cout<<"check"<<std::endl;

//---------------------------------------use thread---------------------------------------use thread--------------------------------------
    initUsersParalell();
//---------------------------------------use thread---------------------------------------use thread--------------------------------------
    std::chrono::steady_clock::time_point end_clock = std::chrono::steady_clock::now();
    float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - begin_clock).count();
    diff = diff / 1000.0f;
    printf("Initialization Time = %f \n", diff);

    init_time=diff;

    itemIndex = windowSize;

//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//	diff = diff / 1000.0f;

//	printf("Cluster Initialized in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));
};


void HDR_Tree::init_half_behind_2(){

    add_user_num=0;

//	initUsersSerial();

    std::chrono::steady_clock::time_point begin_clock = std::chrono::steady_clock::now();

    computation_of_distance_init=0;
//---------------------------------------do not use thread--------------------------------do not use thread-----------------------------

    //std::cout<<"check"<<std::endl;


//---------------------------------------do not use thread--------------------------------do not use thread------------------------------
    distance_time=0;
    //int trace_no_user=0;

    //int count_user=1;

    //int i=0;


    for(int i=0; i<Users.size(); i++){
        //Users[i]->computeKNN(slidingWindow);
        //

        userComputeKNN_2(itemRoot_i, Users[i]);

        //std::cout<<"The dknn value of the "<<i+1<<" user is "<<Users[i]->dknn<<std::endl;

        //std::cout<<i+1<<std::endl;
        //i=i+1;

        //std::cout<<trace_no_user<<std::endl;

        //std::cout<<"Start calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //user_computeknn_id(Users[i], 0.02,0.02, false);

        //std::cout<<"Complete calculating the knn list of the "<<count_user<<"th user"<<std::endl;

        //std::cout<<" "<<std::endl;

        //count_user=count_user+1;
        //trace_no_user=trace_no_user+1;
        //Users[i]->computeKNN_sort(slidingWindow);

        //Users[i]->computeKNN(slidingWindow);
    }

    /*int count_u=1;

    for(auto u: Users){
        std::cout<<"The knn list of the "<<count_u<<"th user"<<std::endl;
        for(auto i:u->R){
            std::cout<<i->v<<std::endl;
            std::cout<<" "<<std::endl;
        }
        count_u=count_u+1;
    }*/



    //std::cout<<"check"<<std::endl;

//---------------------------------------use thread---------------------------------------use thread--------------------------------------
    //initUsersParalell();
//---------------------------------------use thread---------------------------------------use thread--------------------------------------
    std::chrono::steady_clock::time_point end_clock = std::chrono::steady_clock::now();
    float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - begin_clock).count();
    diff = diff / 1000.0f;
    printf("Initialization Time = %f \n", diff);

    init_time=diff;

    itemIndex = windowSize;

//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//	diff = diff / 1000.0f;

//	printf("Cluster Initialized in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));
};





void HDR_Tree::init_half_behind_loop(long divide_num){
    long step_long=int((numFeatures-dim_delta)/divide_num);
    std::vector<float> v_p1_user;
    std::vector<float> v_p2_user;
    int step;
    long dim=dim_delta;

    for(int step_ini=1; step_ini<=10; step_ini=step_ini+1){



        std::cout<<"Start by using "<<step_ini<<" dimensions and "<<vector_proportion_variance[step_ini-1]<<" proportion of variance"<<std::endl;

        v_p1_user.clear();
        v_p2_user.clear();
        data->generate_low_pcamatrix(step_ini);
        for(auto u_ini: Users){
            u_ini->center_low=data->Mat_low_d*u_ini->center;
        }

        for(auto i_ini: slidingWindow){
            i_ini->v_low=data->Mat_low_d*i_ini->v;
        }

        std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();

        for(int i=0; i<Users.size(); i++){
            //Users[i]->computeKNN(slidingWindow);
            //

            userComputeKNN_statistic(itemRoot_i, Users[i], &(v_p1_user),&(v_p2_user) );


        }



        std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();
        float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
        diff = diff / 1000.0f;

        std::cout<<"The loop for dimension "<<step_ini<<"has been completed, and use time of "<<diff<<" seconds"<<std::endl;

        v_time.emplace_back(diff);
        float sum_p1=0;
        for(auto p1:v_p1_user){
            sum_p1=sum_p1+p1;
        }
        auto average_p1=float((1.0*sum_p1)/(1.0*v_p1_user.size()));
        p1_for_dimensions.emplace_back(average_p1);

        float sum_p2=0;
        for(auto p2:v_p2_user){
            sum_p2=sum_p2+p2;
        }
        float average_p2=float((1.0*sum_p2)/(1.0*v_p2_user.size()));
        p2_for_dimensions.emplace_back(average_p2);

        v_d_prune.emplace_back(step_ini);
        partialsum_variance_proportion.emplace_back(vector_proportion_variance[step_ini-1]);

    }



    for(step=1; step<=divide_num-1; step++){



        dim=dim_delta+step*step_long;

        std::cout<<"Start by using "<<dim<<" dimensions and "<<vector_proportion_variance[dim-1]<<" proportion of variance"<<std::endl;

        v_p1_user.clear();
        v_p2_user.clear();
        data->generate_low_pcamatrix(dim);
        for(auto u_ini: Users){
            u_ini->center_low=data->Mat_low_d*u_ini->center;
        }

        for(auto i_ini: slidingWindow){
            i_ini->v_low=data->Mat_low_d*i_ini->v;
        }

        std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();

        for(int i=0; i<Users.size(); i++){
            //Users[i]->computeKNN(slidingWindow);
            //

            userComputeKNN_statistic(itemRoot_i, Users[i], &(v_p1_user),&(v_p2_user) );


        }



        std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();
        float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
        diff = diff / 1000.0f;

        std::cout<<"The "<<step<<"th loop has been completed, and use time of "<<diff<<" seconds"<<std::endl;

        v_time.emplace_back(diff);
        float sum_p1=0;
        for(auto p1:v_p1_user){
            sum_p1=sum_p1+p1;
        }
        auto average_p1=float((1.0*sum_p1)/(1.0*v_p1_user.size()));
        p1_for_dimensions.emplace_back(average_p1);

        float sum_p2=0;
        for(auto p2:v_p2_user){
            sum_p2=sum_p2+p2;
        }
        float average_p2=float((1.0*sum_p2)/(1.0*v_p2_user.size()));
        p2_for_dimensions.emplace_back(average_p2);

        v_d_prune.emplace_back(dim);
        partialsum_variance_proportion.emplace_back(vector_proportion_variance[dim-1]);

    }
    std::cout<<"The dimensions used for pruning on the leafnode of the delta tree are showed below"<<std::endl;
    for(auto dimension: v_d_prune){
        std::cout<<dimension<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"The sum of variance used for pruning on the leafnode of the delta tree are showed below"<<std::endl;
    for(auto variance: partialsum_variance_proportion){
        std::cout<<variance<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"The p1 when using of each dimensions for pruning are showed below"<<std::endl;
    for(auto p1: p1_for_dimensions){
        std::cout<<p1<<" ";
    }
    std::cout<<" "<<std::endl;

    std::cout<<"The p2 when using of each dimensions for pruning are showed below"<<std::endl;
    for(auto p2: p2_for_dimensions){
        std::cout<<p2<<" ";
    }
    std::cout<<" "<<std::endl;

};



void HDR_Tree::setData_i(itemDataBase* dataBase) {
    data_i = dataBase;
}

void HDR_Tree::init_i() {

    data_i->transformData(numFeatures_i, checkDuplicates_i);


//Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA;


//Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA; Different PCA;

//Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA;

    data_i->generate_general_set();

    //std::cout<<"check_0"<<std::endl;

    data_i->generate_item_set();

    //std::cout<<"check_1"<<std::endl;

    data_i->generate_pca_matrix();

//Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA; Same PCA;

    data_i->computePCA();

    for (long i = 0; i < numItems_i; i++) {
        auto itm = new Item(data_i->I.row(i));
        Items_i.emplace_back(itm);
    }

    for (long i = 0; i < Items_i.size(); i++) {
//		std::cout << i << "/" << data->D.size() << std::endl;
        slidingWindow.emplace_back(Items_i[i]);
    }

    add_item_num=0;





    /*itemIndex = windowSize;*/

    //	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    //	diff = diff / 1000.0f;

    //	printf("Cluster Initialized in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));

}


long HDR_Tree::construct_i(long fanout, long threshold) {



    std::cout << "Constructing HDR Tree for Items" << std::endl;
    std::cout << "Items: " << numItems_i << std::endl;
    std::cout << "Features: " << numFeatures_i << std::endl;
    std::cout << "Fanout: " << fanout << std::endl << std::endl;

    init_i();

    L_i = log(data_i->I.size()) / log(fanout);
    std::cout << "Estimated Height: " << L_i << " levels." << std::endl;

    data_i->initTransform(L_i);

    long sequence_ileafnode=0;

    //data_i->calculate_transform_all();

    //data_i->T[2]=data_i->T_all[19];


    //fanout=6000;





    long numClusters_i = 0;

    auto first_layer_itemclusters=new std::vector<itemCluster*>;

    auto C = new itemClusters(Items_i, fanout, data_i->T[0]);   //Directly use the 6th level; directly use the 6-th level

    long sequense_item_cluster_i_layer=0;

    for (auto Cj : *C) {

        numClusters_i += 1;
        Cj->l = 1;

        Cj->i_cluster_sequense=sequense_item_cluster_i_layer;
        sequense_item_cluster_i_layer=sequense_item_cluster_i_layer+1;
        (*first_layer_itemclusters).emplace_back(Cj);
    }

    //std::cout<<"check_0"<<std::endl;

    itemRoot_i = new itemNonLeafNode(C, 1, false);

    itemRoot_ptr_i = itemRoot_i;

    dictionary_for_layer_itemcluster.emplace_back(first_layer_itemclusters);

    auto a_sequense=new int;

    *a_sequense=(*first_layer_itemclusters).size();

    record_sequence.emplace_back(a_sequense);

    std::queue<itemNode*> q_i;

    q_i.push(itemRoot_i);

    long maxL_i = 1;

    while (!q_i.empty()) {
        auto node_i = dynamic_cast<itemNonLeafNode*>(q_i.front());
        q_i.pop();

        auto Cp_i = node_i->clusters;

        for (auto Cj_i : Cp_i) {

            if (Cj_i->number < threshold) {
                auto LN_i = new itemLeafNode(Cj_i, false);

                node_i->children.emplace_back(LN_i);
                Cj_i->ptr = LN_i;


                if(LN_i->cluster->Items.size()!=0){
                    LN_i->sequence_ileafnode=sequence_ileafnode;
                    sequence_ileafnode=sequence_ileafnode+1;
                    itemleafnodelist.emplace_back(LN_i);
                }
            }
            else {
                auto level_i = node_i->l + 1;

                if(level_i>maxL_i){
                    auto b_sequense=new int;
                    *b_sequense=0;
                    record_sequence.emplace_back(b_sequense);
                    auto middle_layer_itemclusters=new std::vector<itemCluster*>;
                    dictionary_for_layer_itemcluster.emplace_back(middle_layer_itemclusters);
                }

                maxL_i = std::max(maxL_i, level_i);

                if(level_i>L_i){
                    std::cout<<"Excceed the estimated level"<<std::endl;
                }

                itemClusters* Cpp_i;

                if(Cj_i->number<fanout){         //*std::pow(2,level_i-1)

                    Cpp_i = new itemClusters(Cj_i, Cj_i->number, data_i->T[level_i - 1]);
                }else{

                    Cpp_i = new itemClusters(Cj_i, fanout, data_i->T[level_i - 1]);
                }
                //Cpp_i = new itemClusters(Cj_i, fanout, data_i->T[level_i - 1]);
                for (auto Cj_i : *Cpp_i) {

                    numClusters_i += 1;
                    Cj_i->l = level_i;
                    dictionary_for_layer_itemcluster[level_i-1]->emplace_back(Cj_i);
                    Cj_i->i_cluster_sequense=(*record_sequence[level_i-1]);
                    (*record_sequence[level_i-1])=(*record_sequence[level_i-1])+1;
                }

                auto NLN_i = new itemNonLeafNode(Cpp_i, level_i, false);


                node_i->children.emplace_back(NLN_i);
                Cj_i->ptr = NLN_i;
                q_i.push(NLN_i);
            }
        }
    }

    std::cout << "Actual Height: " << maxL_i << " levels." << std::endl;
    std::cout << "Clusters: " << numClusters_i << std::endl;

    height_delta= maxL_i;



    dim_delta=data_i->computeDimensionForward(maxL_i, L_i);
    std::cout<<"The maxmal number of dimensions of the delta tree is "<<dim_delta<<std::endl;

    int i;
    for(i=0; i<maxL_i; i++){
        time_cluster_layer.emplace_back(0);
        pruned_clusters_layer.emplace_back(0);
        pruned_items_layer.emplace_back(0);
        calculated_clusters_layer.emplace_back(0);
    }

    return dim_delta;
    //leafnode_time_delta=0;

//Print the number of clusters in each level; Print the number of clusters in each level; Print the number of clusters in each level; Print the number of clusters in each level;

    /*for(auto v:dictionary_for_layer_itemcluster){
        std::cout<<v->size()<<std::endl;
        for(auto c:*v){
            std::cout<<c->i_cluster_sequense<<std::endl;
        }
    }*/

//Print the number of clusters in each level; Print the number of clusters in each level; Print the number of clusters in each level; Print the number of clusters in each level;
}


void HDR_Tree::userComputeKNN(itemNode* node, User* user) { // v1

	/*int clust;

	if (node->type == itemNodeType::itemLeafNode) {
		std::vector<std::tuple<float, Item*>> V;

		auto LN = dynamic_cast<itemLeafNode*>(node);
		long maxL = 0;

		float D;
		for (auto It : *LN) {
			auto item = std::get<1>(It);

			D = dist(user, item);

			std::tuple<float, Item*> T(D, item);
			V.emplace_back(T);

		}

		std::sort(V.begin(), V.end(),
			[&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
				return std::get<0>(a) < std::get<0>(b);
			});

		for (int i = 0; i < k; i++) {
			user->R.emplace_back(std::get<1>(V[i]));
		}
	}
	else {
		auto NLN = dynamic_cast<itemNonLeafNode*>(node);
		auto& Cp = NLN->clusters;

		float D;
		auto minDist = std::numeric_limits<float>::max();

		for (long j = 0; j < Cp.size(); j++) {
			D = dist(user, Cp[j]);

			if (D < minDist) {
				minDist = D;
				clust = j;
			}
		}
		userComputeKNN(NLN->children[clust], user);
	}*/

	//test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
    //projectUser_item(user);


    //std::chrono::steady_clock::time_point begin_clock;
    //std::chrono::steady_clock::time_point end_clock;


    //begin_clock=std::chrono::steady_clock::now();

    project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    //end_clock=std::chrono::steady_clock::now();

    //time_computeknn_project=time_computeknn_project+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

    //u_trans_i_clus_init.clear();

    //u_trans_i_clus_init.emplace_back((data_i->T[5])*(user->center));




    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    user->R.clear();
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound=std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;
    //std::chrono::steady_clock::time_point begin_clock_leafnode;
    //std::chrono::steady_clock::time_point end_clock_leafnode;
    //int exceed_statistic;




    float item_add;

    float distance_user_item;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{

            //begin_clock=std::chrono::steady_clock::now();

            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();

            //end_clock=std::chrono::steady_clock::now();

            //time_computeknn_sort_1=time_computeknn_sort_1+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

        }
        if((std::get<0>(pop_node))>upbound){
            //for(auto remain_node:V){
                //pruned_clusters_layer[std::get<1>(remain_node)->l-2]=pruned_clusters_layer[std::get<1>(remain_node)->l-2]+1;
                //pruned_items_layer[std::get<1>(remain_node)->l-2]=pruned_items_layer[std::get<1>(remain_node)->l-2]+std::get<1>(remain_node)->size;
            //}
            //pruned_clusters_layer[std::get<1>(pop_node)->l-2]=pruned_clusters_layer[std::get<1>(pop_node)->l-2]+1;
            //pruned_items_layer[std::get<1>(pop_node)->l-2]=pruned_items_layer[std::get<1>(pop_node)->l-2]+std::get<1>(pop_node)->size;
            break;

        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation

                //begin_clock=std::chrono::steady_clock::now();

                float dist_r=dist_u_iclust_init(C)-(C->radius);

                //end_clock=std::chrono::steady_clock::now();

                //time_low_dimensional_distance=time_low_dimensional_distance+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

                //calculated_clusters_layer[C->l-1]=calculated_clusters_layer[C->l-1]+1;

                //distance_time_low_d_nonleaf=distance_time_low_d_nonleaf+1;

// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(dist_r<=upbound){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){


                        //std::cout<<"start leafnode"<<std::endl;


                        //item_add=0;
                        if(C->Items.size()!=0){
                            for(auto I: C->Items){
                                //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);

                                //if((user->center_low-std::get<1>(I)->v_low).norm()<upbound) {



                                //User* u=user;

                                //item_access_count_single[std::get<1>(I)]++;

                                Item* item_cache=(std::get<1>(I));

                                //begin_clock=std::chrono::steady_clock::now();

                                //distance_user_item = dist(std::get<1>(I), user);
                                //VTUNE_START_SINGLE();

                                //distance_user_item=(user->center-(std::get<1>(I))->v).norm();

                                distance_user_item=distance_test_chache(user, item_cache);

                                //VTUNE_END_SINGLE();

                                //unprun_num_knn=unprun_num_knn+1;

                                //vector_dist_rate_bound_n2_knn.emplace_back(pow((distance_user_item/upbound),2));

                                //distance_user_item =(u->center-(std::get<1>(I))->v).norm();


                                //end_clock=std::chrono::steady_clock::now();

                                //time_computeknn_full_distance=time_computeknn_full_distance+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();


                                //distance_time = distance_time + 1;

                                if (distance_user_item < upbound) {
                                    d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                    item_add = item_add + 1;
                                }
                                //else{
                                  //knn_candidate_exceed_bound_proportion.emplace_back(pow((upbound/distance_user_item),2));

                                //knn_candidate_exceed_bound_proportion.emplace_back(pow((distance_user_item/upbound),2));

                                //exceed_statistic=int(((distance_user_item-upbound)/upbound)/0.4);

                                //exceed_statistic=int((pow((upbound/distance_user_item),2))/0.3);
                                  //items_exceed_knn_bound_proportion_statistic[int(pow((upbound/distance_user_item),2)/0.1)]=items_exceed_knn_bound_proportion_statistic[int(pow((upbound/distance_user_item),2)/0.1)]+1;

                                //}
                                //}
                                //distance_user_item=dist(std::get<1>(I), user);
                                //if(distance_user_item<user->dknn){
                                //d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                //}
                            }



                            //begin_clock=std::chrono::steady_clock::now();

                            if((d_i.size()>=k)&&(item_add>0)){

                                std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                d_i.assign(d_i.begin(), d_i.begin()+k);
                                upbound=std::get<0>(d_i[k-1]);

                            }

                            //end_clock=std::chrono::steady_clock::now();

                            //time_computeknn_sort_2=time_computeknn_sort_2+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

                            //end_clock_leafnode = std::chrono::steady_clock::now();
                            //leafnode_time_delta=leafnode_time_delta+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_leafnode - begin_clock_leafnode).count();
                        }

                        //std::cout<<"end leafnode"<<std::endl;

                    }else{
                        //std::cout<<"start nonleafnode"<<std::endl;
                        V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                        //std::cout<<"end nonleafnode"<<std::endl;
                    }
                }
                //else{
                    //pruned_clusters_layer[C->l-1]=pruned_clusters_layer[C->l-1]+1;
                    //pruned_items_layer[C->l-1]=pruned_items_layer[C->l-1]+C->Items.size();
                //}
            }
        }
    }
    for(auto di: d_i){
        user->R.emplace_back(std::get<1>(di));

    }
    user->dknn=std::get<0>(d_i[k-1]);

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
}


void HDR_Tree::userComputeKNN_2(itemNode* node, User* user) { // v1

    /*int clust;

    if (node->type == itemNodeType::itemLeafNode) {
        std::vector<std::tuple<float, Item*>> V;

        auto LN = dynamic_cast<itemLeafNode*>(node);
        long maxL = 0;

        float D;
        for (auto It : *LN) {
            auto item = std::get<1>(It);

            D = dist(user, item);

            std::tuple<float, Item*> T(D, item);
            V.emplace_back(T);

        }

        std::sort(V.begin(), V.end(),
            [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                return std::get<0>(a) < std::get<0>(b);
            });

        for (int i = 0; i < k; i++) {
            user->R.emplace_back(std::get<1>(V[i]));
        }
    }
    else {
        auto NLN = dynamic_cast<itemNonLeafNode*>(node);
        auto& Cp = NLN->clusters;

        float D;
        auto minDist = std::numeric_limits<float>::max();

        for (long j = 0; j < Cp.size(); j++) {
            D = dist(user, Cp[j]);

            if (D < minDist) {
                minDist = D;
                clust = j;
            }
        }
        userComputeKNN(NLN->children[clust], user);
    }*/

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
    //projectUser_item(user);

    project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    user->R.clear();
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound=std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    float item_add;

    float distance_user_item;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>upbound){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation

                float dist_r=dist_u_iclust_init(C)-(C->radius);

                distance_time_low_d_nonleaf=distance_time_low_d_nonleaf+1;

// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(dist_r<=upbound){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){
                        if(C->Items.size()!=0){
                            item_add=0;
                            for(auto I: C->Items){
                                //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);

                                num_total_meet=num_total_meet+1;

                                distance_time_low_d_leaf=distance_time_low_d_leaf+1;


                                if((user->center_low-std::get<1>(I)->v_low).norm()<upbound) {
                                    distance_user_item = dist(std::get<1>(I), user);

                                    distance_time = distance_time + 1;

                                    num_cal_twice=num_cal_twice+1;

                                    if (distance_user_item < upbound) {
                                        d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                        item_add = item_add + 1;
                                    }
                                }
                                /*distance_user_item=dist(std::get<1>(I), user);
                                if(distance_user_item<user->dknn){
                                    d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                }*/
                            }
                            if((d_i.size()>=k)&&(item_add>0)){
                                std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                d_i.assign(d_i.begin(), d_i.begin()+k);
                                upbound=std::get<0>(d_i[k-1]);
                            }
                        }
                    }else{
                        V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto di: d_i){
        user->R.emplace_back(std::get<1>(di));

    }
    user->dknn=std::get<0>(d_i[k-1]);

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
}









void HDR_Tree::userComputeKNN_statistic(itemNode* node, User* user, std::vector<float>* p1_v, std::vector<float>* p2_v) {


    //
    /*int clust;

    if (node->type == itemNodeType::itemLeafNode) {
        std::vector<std::tuple<float, Item*>> V;

        auto LN = dynamic_cast<itemLeafNode*>(node);
        long maxL = 0;

        float D;
        for (auto It : *LN) {
            auto item = std::get<1>(It);

            D = dist(user, item);

            std::tuple<float, Item*> T(D, item);
            V.emplace_back(T);

        }

        std::sort(V.begin(), V.end(),
            [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                return std::get<0>(a) < std::get<0>(b);
            });

        for (int i = 0; i < k; i++) {
            user->R.emplace_back(std::get<1>(V[i]));
        }
    }
    else {
        auto NLN = dynamic_cast<itemNonLeafNode*>(node);
        auto& Cp = NLN->clusters;

        float D;
        auto minDist = std::numeric_limits<float>::max();

        for (long j = 0; j < Cp.size(); j++) {
            D = dist(user, Cp[j]);

            if (D < minDist) {
                minDist = D;
                clust = j;
            }
        }
        userComputeKNN(NLN->children[clust], user);
    }*/

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
    //projectUser_item(user);

    std::vector<float> storage_p1;
    std::vector<float> storage_p2;

    long sum_item_leafnode;
    long sum_in;
    long sum_out_once;
    long sum_out_twice;
    long twice;
    long sum_out;
    float p1;
    float p2;

    float sum_p1;
    float average_p1;
    float sum_p2;
    float average_p2;


    project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    user->R.clear();
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound=std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    float item_add;

    float distance_user_item;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>upbound){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation

                float dist_r=dist_u_iclust_init(C)-(C->radius);

// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(dist_r<=upbound){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){

                        item_add=0;

                        sum_item_leafnode=0;
                        sum_in=0;
                        twice=0;


                        for(auto I: C->Items){
                            //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                            //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);

                            sum_item_leafnode=sum_item_leafnode+1;

                            if((user->center_low-std::get<1>(I)->v_low).norm()<upbound) {
                                distance_user_item = dist(std::get<1>(I), user);

                                twice=twice+1;

                                distance_time = distance_time + 1;

                                if (distance_user_item < upbound) {
                                    d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                    item_add = item_add + 1;

                                    sum_in=sum_in+1;

                                }
                            }
                            /*distance_user_item=dist(std::get<1>(I), user);
                            if(distance_user_item<user->dknn){
                                d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                            }*/
                        }

                        sum_out=sum_item_leafnode-sum_in;
                        sum_out_twice=twice-sum_in;

                        if(sum_item_leafnode!=0){
                            p1=float((1.0f*sum_in)/(1.0f*sum_item_leafnode));
                            storage_p1.emplace_back(p1);
                        }

                        if(sum_out!=0){
                            p2=float((1.0f*sum_out_twice)/(1.0f*sum_out));
                            storage_p2.emplace_back(p2);
                        }



                        if((d_i.size()>=k)&&(item_add>0)){
                            std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                             [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                 return std::get<0>(a) <= std::get<0>(b);
                                             });
                            d_i.assign(d_i.begin(), d_i.begin()+k);
                            upbound=std::get<0>(d_i[k-1]);
                        }
                    }else{
                        V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto di: d_i){
        user->R.emplace_back(std::get<1>(di));

    }
    user->dknn=std::get<0>(d_i[k-1]);

    sum_p1=0;
    sum_p2=0;

    for(auto p1_gained:storage_p1){
        sum_p1=sum_p1+p1_gained;
    }
    average_p1=float(sum_p1/(1.0*storage_p1.size()));
    p1_v->emplace_back(average_p1);

    for(auto p2_gained:storage_p2){
        sum_p2=sum_p2+p2_gained;
    }
    average_p2=float(sum_p2/(1.0*storage_p2.size()));
    p2_v->emplace_back(average_p2);

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_

    //std::cout<<"p1="<<average_p1<<std::endl;
    //std::cout<<"p2="<<average_p2<<std::endl;
};






void HDR_Tree::userComputeKNNcross(itemNode* node, User* user){

    std::chrono::steady_clock::time_point check_time_start;

    std::chrono::steady_clock::time_point check_time_end;

    /*int clust;

    if (node->type == itemNodeType::itemLeafNode) {
        std::vector<std::tuple<float, Item*>> V;

        auto LN = dynamic_cast<itemLeafNode*>(node);
        long maxL = 0;

        float D;
        for (auto It : *LN) {
            auto item = std::get<1>(It);

            D = dist(user, item);

            std::tuple<float, Item*> T(D, item);
            V.emplace_back(T);

        }

        std::sort(V.begin(), V.end(),
            [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                return std::get<0>(a) < std::get<0>(b);
            });

        for (int i = 0; i < k; i++) {
            user->R.emplace_back(std::get<1>(V[i]));
        }
    }
    else {
        auto NLN = dynamic_cast<itemNonLeafNode*>(node);
        auto& Cp = NLN->clusters;

        float D;
        auto minDist = std::numeric_limits<float>::max();

        for (long j = 0; j < Cp.size(); j++) {
            D = dist(user, Cp[j]);

            if (D < minDist) {
                minDist = D;
                clust = j;
            }
        }
        userComputeKNN(NLN->children[clust], user);
    }*/

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
    //projectUser_item(user);

    //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;
    check_time_start=std::chrono::steady_clock::now();
    //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;

    project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    user->R.clear();
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound=std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;
    long level;
    Cluster* C_transformed;
    int i;   //Use to calculate the number of loops; useless formally
    float dist_r;  //Use to store the low-dimensional distance

    float distance_cross;

    //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;
    check_time_end=std::chrono::steady_clock::now();
    time_compute_knn_cross_head=time_compute_knn_cross_head+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
    //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;

    //Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters;


    //Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters; Temporary parameters;

    //temporary action; temporary action; temporary action; temporary action; temporary action; temporary action; temporary action; temporary action;

    //temporary action; temporary action; temporary action; temporary action; temporary action; temporary action; temporary action; temporary action;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{

            //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;
            check_time_start=std::chrono::steady_clock::now();
            //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;



            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();

            //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;
            check_time_end=std::chrono::steady_clock::now();
            time_compute_sort_list=time_compute_sort_list+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
            //check the time for the head of this function; check the time for the head of this function; check the time for the head of this function; check the time for the head of this function;



        }
        if((std::get<0>(pop_node))>upbound){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){

                //Add 1 for the sum of the number of clusters on one layer; Add 1 for the sum of the number of clusters on one layer; Add 1 for the sum of the number of clusters on one layer;
                //check_prune[C->l-1][0]=check_prune[C->l-1][0]+1;
                //Add 1 for the sum of the number of clusters on one layer; Add 1 for the sum of the number of clusters on one layer; Add 1 for the sum of the number of clusters on one layer;


                //check how many clusters are vistied summury; check how many clusters are vistied summury; check how many clusters are vistied summury;
                //check_cluster_visited=check_cluster_visited+1;
                //check how many clusters are vistied summury; check how many clusters are vistied summury; check how many clusters are vistied summury;

                //the number of clusters prunned by traditional method; the number of clusters prunned by traditional method; the number of clusters prunned by traditional method;
                //check_cluster_pruned_traditional=check_cluster_pruned_traditional+1;
                //the number of clusters prunned by traditional method; the number of clusters prunned by traditional method; the number of clusters prunned by traditional method;




                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation


                //std::cout<<"The level of the highest cluster that holds the user is "<<user->list_cluster_belong.size()<<std::endl;
                //std::cout<<"The level of the item cluster that is visited is "<<C->l<<std::endl;


                //Check the time for calculating the level calculation time;  Check the time for calculating the level calculation time;
                check_time_start=std::chrono::steady_clock::now();
                //Check the time for calculating the level calculation time;  Check the time for calculating the level calculation time;
                level=user->list_cluster_belong.size();
                //Check the time for calculating the level calculation time;  Check the time for calculating the level calculation time;
                check_time_end=std::chrono::steady_clock::now();
                time_judgelevel=time_judgelevel+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                //Check the time for calculating the level calculation time;  Check the time for calculating the level calculation time;

                if(level<C->l){
                    //std::cout<<"Error, no cluster of level "<<C->l<<" that holds the user"<<std::endl;
                    //std::cout<<"check_1"<<std::endl;

                    //exceeding height; exceeding height; exceeding height; exceeding height;
                    //check_prune[C->l-1][1]=check_prune[C->l-1][1]+1;
                    //exceeding height; exceeding height; exceeding height; exceeding height;

                    //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;
                    check_time_start=std::chrono::steady_clock::now();
                    //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;
                    /*for(i=0;i<=500000;i++){
                        dist_r=dist_u_iclust_init(C)-(C->radius);
                    }*/

                    dist_r=dist_u_iclust_init(C)-(C->radius);
                    //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;
                    check_time_end=std::chrono::steady_clock::now();
                    time_traditional_prune=time_traditional_prune+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                    //std::cout<<"The time_traditional_prune is "<<time_traditional_prune<<std::endl;
                    //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;

// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                    if(dist_r<=upbound){

                        //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;
                        check_time_start=std::chrono::steady_clock::now();
                        //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;


                        if((C->ptr)->type==itemNodeType::itemLeafNode){
                            for(auto I: C->Items){
                                //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);
                                d_i.emplace_back(std::make_tuple(dist(std::get<1>(I), user), std::get<1>(I)));
                            }
                            if(d_i.size()>=k){
                                std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                d_i.assign(d_i.begin(), d_i.begin()+k);
                                upbound=std::get<0>(d_i[k-1]);
                            }
                        }else{
                            V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                        }

                        //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;
                        check_time_end=std::chrono::steady_clock::now();
                        time_knn_computation=time_knn_computation+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                        //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;
                    }

                }
                else{


                    //no exceeding height; no exceeding height; no exceeding height; no exceeding height;
                    //check_prune[C->l-1][2]=check_prune[C->l-1][2]+1;
                    //no exceeding height; no exceeding height; no exceeding height; no exceeding height;


                    //Check the time for transformation of cluster type; Check the time for transformation of cluster type; Check the time for transformation of cluster type;
                    check_time_start=std::chrono::steady_clock::now();
                    //Check the time for transformation of cluster type; Check the time for transformation of cluster type; Check the time for transformation of cluster type;
                    C_transformed=dynamic_cast<Cluster*>((user->list_cluster_belong)[C->l-1]);
                    //Check the time for transformation of cluster type; Check the time for transformation of cluster type; Check the time for transformation of cluster type;
                    check_time_end=std::chrono::steady_clock::now();
                    time_transformtype=time_transformtype+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                    //Check the time for transformation of cluster type; Check the time for transformation of cluster type; Check the time for transformation of cluster type;




                    //std::cout<<"check_0"<<std::endl;




                    //check the calculation for cross distance; check the calculation for cross distance; check the calculation for cross distance;
                    check_time_start=std::chrono::steady_clock::now();
                    //check the calculation for cross distance; check the calculation for cross distance; check the calculation for cross distance;
                    distance_cross=(C_transformed->distance_cross_u_t)[C->i_cluster_sequense]-C_transformed->radius-C->radius;
                    //check the calculation for cross distance; check the calculation for cross distance; check the calculation for cross distance;
                    check_time_end=std::chrono::steady_clock::now();
                    time_calculate_crossprunedistance=time_calculate_crossprunedistance+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                    //check the calculation for cross distance; check the calculation for cross distance; check the calculation for cross distance;
                    if(distance_cross<=upbound){



                        //no cross prune; no cross prune; no cross prune; no cross prune;
                        //check_prune[C->l-1][4]=check_prune[C->l-1][4]+1;
                        //no cross prune; no cross prune; no cross prune no cross prune;


                        //check number of clusters pruned by cross; check number of clusters pruned by cross; check number of clusters pruned by cross;
                        //check_cluster_pruned_bycross=check_cluster_pruned_bycross+1;
                        //check number of clusters pruned by cross; check number of clusters pruned by cross; check number of clusters pruned by cross;


                        //std::cout<<"check_1"<<std::endl;

                        //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;
                        check_time_start=std::chrono::steady_clock::now();
                        //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;

                        dist_r=dist_u_iclust_init(C)-(C->radius);
                        //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;
                        check_time_end=std::chrono::steady_clock::now();
                        time_traditional_prune=time_traditional_prune+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                        //Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance; Check the time for calculating traditional prunning distance;



// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                        if(dist_r<=upbound){

                            //No traditional prune; No traditional prune; No traditional prune; No traditional prune;

                            //check_prune[C->l-1][6]=check_prune[C->l-1][6]+1;

                            //No traditional prune; No traditional prune; No traditional prune; No traditional prune;




                            //the number of clusters prunned by traditional method; the number of clusters prunned by traditional method; the number of clusters prunned by traditional method;
                            //check_cluster_pruned_traditional=check_cluster_pruned_traditional+1;
                            //the number of clusters prunned by traditional method; the number of clusters prunned by traditional method; the number of clusters prunned by traditional method;

                            //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;
                            check_time_start=std::chrono::steady_clock::now();
                            //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;

                            if((C->ptr)->type==itemNodeType::itemLeafNode){
                                for(auto I: C->Items){
                                    //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                    //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);
                                    d_i.emplace_back(std::make_tuple(dist(std::get<1>(I), user), std::get<1>(I)));
                                }
                                if(d_i.size()>=k){
                                    std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                                     [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                         return std::get<0>(a) <= std::get<0>(b);
                                                     });
                                    d_i.assign(d_i.begin(), d_i.begin()+k);
                                    upbound=std::get<0>(d_i[k-1]);
                                }
                            }else{
                                V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                            }

                            //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;
                            check_time_end=std::chrono::steady_clock::now();
                            time_knn_computation=time_knn_computation+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                            //check time for knn computation; check time for knn computation; check time for knn computation; check time for knn computation;



                        }
                        /*else{
                            //Traditional prune; Traditional prune; Traditional prune; Traditional prune;

                            //check_prune[C->l-1][5]=check_prune[C->l-1][5]+1;

                            //Traditional prune; Traditional prune; Traditional prune; Traditional prune;
                        }*/




                    }

                    /*else{
                        //cross prune; cross prune; cross prune; cross prune;
                        //check_prune[C->l-1][3]=check_prune[C->l-1][3]+1;
                        //cross prune; cross prune; cross prune cross prune;
                    }*/

                }


                //std::cout<<"check_1"<<std::endl;
            }
        }
    }
    for(auto di: d_i){
        user->R.emplace_back(std::get<1>(di));
        user->dknn=std::get<0>(d_i[k-1]);
    }


    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
}


void HDR_Tree::computeknnuser_noroot(User* user){

    /*int clust;

    if (node->type == itemNodeType::itemLeafNode) {
        std::vector<std::tuple<float, Item*>> V;

        auto LN = dynamic_cast<itemLeafNode*>(node);
        long maxL = 0;

        float D;
        for (auto It : *LN) {
            auto item = std::get<1>(It);

            D = dist(user, item);

            std::tuple<float, Item*> T(D, item);
            V.emplace_back(T);

        }

        std::sort(V.begin(), V.end(),
            [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                return std::get<0>(a) < std::get<0>(b);
            });

        for (int i = 0; i < k; i++) {
            user->R.emplace_back(std::get<1>(V[i]));
        }
    }
    else {
        auto NLN = dynamic_cast<itemNonLeafNode*>(node);
        auto& Cp = NLN->clusters;

        float D;
        auto minDist = std::numeric_limits<float>::max();

        for (long j = 0; j < Cp.size(); j++) {
            D = dist(user, Cp[j]);

            if (D < minDist) {
                minDist = D;
                clust = j;
            }
        }
        userComputeKNN(NLN->children[clust], user);
    }*/

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
    //projectUser_item(user);

    project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(itemRoot_i);
    user->R.clear();
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound=std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    float item_add;

    float distance_user_item;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>upbound){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation

                float dist_r=dist_u_iclust_init(C)-(C->radius);

// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(dist_r<=upbound){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){

                        item_add=0;

                        for(auto I: C->Items){
                            //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                            //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);

                            if((user->center_low-std::get<1>(I)->v_low).norm()<upbound) {
                                distance_user_item = dist(std::get<1>(I), user);

                                distance_time = distance_time + 1;

                                if (distance_user_item < upbound) {
                                    d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                    item_add = item_add + 1;
                                }
                            }
                            /*distance_user_item=dist(std::get<1>(I), user);
                            if(distance_user_item<user->dknn){
                                d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                            }*/
                        }
                        if((d_i.size()>=k)&&(item_add>0)){
                            std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                             [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                 return std::get<0>(a) <= std::get<0>(b);
                                             });
                            d_i.assign(d_i.begin(), d_i.begin()+k);
                            upbound=std::get<0>(d_i[k-1]);
                        }
                    }else{
                        V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto di: d_i){
        user->R.emplace_back(std::get<1>(di));

    }
    user->dknn=std::get<0>(d_i[k-1]);

    //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
};





void HDR_Tree::Search(Node* node, Item* I) {

    if (node->type == NodeType::LeafNode) {

        //std::cout<<"Start leafnode"<<std::endl;

        //std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

        //std::cout<<"Start transformation"<<std::endl;

        auto LN = dynamic_cast<LeafNode*>(node);

        //std::cout<<"End transformation and start loop of users"<<std::endl;
        if(LN->cluster->Users.size()!=0){
            for (auto Ut : *LN) {

                //std::cout<<"Start add times 1"<<std::endl;

                num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

                unprun_num_rknn=unprun_num_rknn+1;

                //std::cout<<"End add times 1 and start add times 2"<<std::endl;

                distance_time=distance_time+1;

                //std::cout<<"End add times 2"<<std::endl;

                auto user = std::get<1>(Ut);


                //if((user->center_low-I->v_low).norm()<user->dknn) {

                //std::chrono::steady_clock::time_point time_start_cal = std::chrono::steady_clock::now();

                //std::cout<<"Start distance computation"<<std::endl;

                float distance=(user->center-I->v).norm();

                vector_dist_rate_bound_n2_rknn.emplace_back(pow((distance/(user->dknn)),2));

                //std::cout<<"End distance computation"<<std::endl;

                //std::chrono::steady_clock::time_point time_end_cal = std::chrono::steady_clock::now();

                //real_time_distance_calculation=real_time_distance_calculation+std::chrono::duration_cast<std::chrono::nanoseconds>(time_end_cal - time_start_cal).count();

                //std::cout<<"Start judgement of being affected or not"<<std::endl;

                if (distance < user->dknn) {
                    Rp.emplace_back(user);
                }

                //std::cout<<"End judgement of being affected or not"<<std::endl;

                //else{
                  //float rate=pow((user->dknn/distance),2);
                  //rknn_candidate_exceed_bound_proportion.emplace_back(rate);
                  //users_exceed_rknn_bound_proportion_statistic[int(pow((user->dknn/distance),2)/0.1)]=users_exceed_rknn_bound_proportion_statistic[int(pow((user->dknn/distance),2)/0.1)]+1;
                //}

                //'}
            }

        }



        //std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();


        //time_leafnode=time_leafnode+std::chrono::duration_cast<std::chrono::microseconds>( time_end- time_start).count();

        //std::cout<<"End leafnode"<<std::endl;
    }
    else {

        //std::cout<<"Start Nonleafnode"<<std::endl;

        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            distance_time_low_d_nonleaf=distance_time_low_d_nonleaf+1;

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                Search(NLN->children[j], I);
            }
        }

        //std::cout<<"End Nonleafnode"<<std::endl;

    }


    /*if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {

            num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);
            if (dist(I, user) < user->dknn) {
                Rp.emplace_back(user);
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                Search(NLN->children[j], I);
            }
        }
    }*/
}

void HDR_Tree::Search_fast(Node* node, Item* I){

    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);

        if(LN->cluster->Users.size()!=0){
            for (auto Ut : *LN) {

                num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

                auto user = std::get<1>(Ut);

                distance_time_low_d_leaf=distance_time_low_d_leaf+1;


                if((user->center_low-I->v_low).norm()<user->dknn) {

                    distance_time=distance_time+1;

                    if (dist(I, user) < user->dknn) {
                        Rp.emplace_back(user);
                    }

                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            distance_time_low_d_nonleaf=distance_time_low_d_nonleaf+1;

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                Search_fast(NLN->children[j], I);
            }
        }
    }
}







float HDR_Tree::AdjustMaxdknnPrev(Node* n) {

    float Dist = 0;

    if (n->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(n);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);
            auto D = user->dknn;
            Dist = std::max(Dist, D);
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(n);
        auto Cp = NLN->clusters;

        for (auto Cj : Cp) {
            auto D = AdjustMaxdknnPrev(Cj->ptr);
            Dist = std::max(Dist, D);
        }
    }

    return Dist;
}
bool HDR_Tree::AdjustMaxdknnPaper(Node* n) {

    bool result = false;

    if (n->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(n);

        long Len = Rp.size();
        for (auto Ut : *LN) {
            auto& user = std::get<1>(Ut);
            for (long i = 0; i < Len; i++) {
                if (Rp[i] == user) {
                    return true;
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(n);
        auto& Cp = NLN->clusters;

        for (auto Cj : Cp) {
            bool res = AdjustMaxdknnPaper(Cj->ptr);

            if (res) {
                result = true;

                float Dist = 0;

                if (Cj->ptr->type == NodeType::LeafNode) {

                    auto LN = dynamic_cast<LeafNode*>(Cj->ptr);
                    for (auto Ut : *LN) {

                        auto user = std::get<1>(Ut);
                        Dist = std::max(Dist, user->dknn);
                    }
                }
                else {

                    auto NLN = dynamic_cast<NonLeafNode*>(Cj->ptr);
                    auto Cp = NLN->clusters;

                    for (auto Cj : Cp) {
                        Dist = std::max(Dist, Cj->maxdknn);
                    }
                }

                Cj->maxdknn = Dist;
            }
        }
    }

    return result;
}




//void User::dist(Item* I, float& D) { // v0 for any system
//
////	D = (I->v - center).squaredNorm();
//	D = (I->v - center).norm();
//
//	return;
//}



//item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function









void HDR_Tree::update_user_hdr(User* U, Mode m, Node* node){
    if(m==Mode::Add){

        if(node->type==NodeType::NonLeafNode){
            auto NLN=dynamic_cast<NonLeafNode*>(node);

            //std::cout<<"Calculating the "<<NLN->l<<" level node"<<std::endl;

            std::vector<std::tuple<float, Cluster*>>V;
            auto v_transformed=data->T[NLN->l-1]*U->center;
            for(auto C_0: NLN->clusters){
                V.emplace_back(std::make_tuple((v_transformed-C_0->center).norm(), C_0));
            }

            //std::cout<<"Complete computing the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, Cluster*>& a, const std::tuple<float, Cluster* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });

            //std::cout<<"Complete sorting the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

            auto C_add=std::get<1>(V[V.size()-1]);

            //std::cout<<"Catch the cluster that the new user should be added to"<<std::endl;

            C_add->Users.emplace_back(std::make_tuple(std::get<0>(V[V.size()-1]), U));

            //std::cout<<"Complete adding the user into the proper node"<<std::endl;

            C_add->number=C_add->number+1;

            //std::cout<<"The current number of users "<<C_add->number<<" in the catched cluster after adding the new user"<<std::endl;

            std::sort(C_add->Users.begin(),  C_add->Users.end(),
                             [&](const std::tuple<float, User*>& a, const std::tuple<float, User*>& b) -> bool {
                                 return std::get<0>(a) <= std::get<0>(b);
                             });
            C_add->radius=std::get<0>(C_add->Users[(C_add->Users).size()-1]);
            if (U->dknn>C_add->maxdknn){
                C_add->maxdknn=U->dknn;
            }
            update_user_hdr( U, m, C_add->ptr);
        }
    }else{
        if(node->type==NodeType::NonLeafNode){
            auto NLN=dynamic_cast<NonLeafNode*>(node);
            //std::cout<<"1"<<std::endl;   //show show show show show show show show
            Cluster* C_catch;

            //std::cout<<"The original state of the C_catch "<<(C_catch->Users).size()<<std::endl;   //show show show show show show show show

            long i_catch;
            for(auto C:NLN->clusters){
                if(C->number!=0){
                    //std::cout<<"Not zero"<<std::endl;         //show show show show show show show
                    bool indicator= false;
                    for(long i=0; i<(C->Users).size(); i++){
                        if(std::get<1>(C->Users[i])==U){


                            C_catch=C;

                            //std::cout<<"catch success"<<std::endl;   //show show show show show show show

                            i_catch=i;
                            indicator= true;
                            break;
                        }
                    }
                    if(indicator==true){
                        break;
                    }
                }
            }

            //std::cout<<"2"<<std::endl;//show show show show show show show show
            //std::cout<<(C_catch->Users).size()<<" 3"<<std::endl;  //show show show show show show show show
            C_catch->number=C_catch->number-1;
            if(C_catch->number==0){
                C_catch->radius=0;
                C_catch->maxdknn=0;
                C_catch->Users.erase(C_catch->Users.begin()+i_catch);
            }else{
                //std::cout<<"4"<<std::endl;//show show show show show show show show
                //std::cout<<(C_catch->Users).size()<<" 5"<<std::endl;  //show show show show show show show show
                C_catch->Users.erase(C_catch->Users.begin()+i_catch);
                //std::cout<<"6"<<std::endl;//show show show show show show show show
                //std::cout<<(C_catch->Users).size()<<" 7"<<std::endl;   //show show show show show show show show
                C_catch->radius=std::get<0>(C_catch->Users[C_catch->Users.size()-1]);
                //std::cout<<"8"<<std::endl;//show show show show show show show show
                if(U->dknn==C_catch->maxdknn){
                    std::vector<float> V;
                    for(auto tp: C_catch->Users){
                        V.emplace_back(std::get<1>(tp)->dknn);
                    }
                    std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                                     [&](const float& a, const float& b) -> bool {
                                         return a<=b;
                                     });
                    C_catch->maxdknn=V[V.size()-1];
                }
            }

            update_user_hdr( U, m, C_catch->ptr);
        }
    }
}






void HDR_Tree::update_item_hdr(itemNode* root_item_tree, Item* I, Mode mode){
    if(mode==Mode::Add){
        if(root_item_tree->type==itemNodeType::itemNonLeafNode){
            auto root_i=dynamic_cast<itemNonLeafNode*>(root_item_tree);
            itemCluster* catch_c;
            std::vector<std::tuple<float, itemCluster*>> V_d_c;
            auto v_i_transformed=data_i->T[root_i->l-1]*I->v;
            for(auto C:root_i->clusters){
                float distance_i_c=(v_i_transformed-C->center).norm();
                V_d_c.emplace_back(std::make_tuple(distance_i_c, C));
            }
            std::nth_element(V_d_c.begin(), V_d_c.begin() + V_d_c.size() - 1, V_d_c.end(),
                             [&](const std::tuple<float, itemCluster*>& a, const std::tuple<float, itemCluster*>& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            catch_c=std::get<1>(V_d_c[V_d_c.size()-1]);
            catch_c->number=catch_c->number+1;
            catch_c->Items.emplace_back(std::make_tuple(std::get<0>(V_d_c[V_d_c.size()-1]), I));
            std::nth_element(catch_c->Items.begin(), catch_c->Items.begin() + catch_c->Items.size() - 1, catch_c->Items.end(),
                             [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                 return std::get<0>(a) <= std::get<0>(b);
                             });
            catch_c->radius=std::get<0>(catch_c->Items[catch_c->Items.size()-1]);
            update_item_hdr(catch_c->ptr,  I, mode);
        }
    }else{
        if(root_item_tree->type==itemNodeType::itemNonLeafNode){
            auto root_i_reduce=dynamic_cast<itemNonLeafNode*>(root_item_tree);
            itemCluster* catch_c_reduce;
            int catch_no_i;
            for(auto C_c_r: root_i_reduce->clusters){
                if(C_c_r->Items.size()!=0){
                    bool indicator_c_if= false;
                    for(int i=0; i<C_c_r->Items.size();i++){
                        if(std::get<1>(C_c_r->Items[i])==I){
                            //std::cout<<"Catch successfully"<<std::endl;
                            catch_no_i=i;
                            catch_c_reduce=C_c_r;
                            indicator_c_if= true;
                            break;
                        }
                    }
                    if(indicator_c_if== true){
                        break;
                    }
                }
            }
            catch_c_reduce->Items.erase(catch_c_reduce->Items.begin()+catch_no_i);
            catch_c_reduce->number=catch_c_reduce->number-1;
            if(catch_c_reduce->number==0){
                catch_c_reduce->radius=0;
            }else{
                std::nth_element(catch_c_reduce->Items.begin(), catch_c_reduce->Items.begin() + catch_c_reduce->Items.size() - 1, catch_c_reduce->Items.end(),
                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                     return std::get<0>(a) <= std::get<0>(b);
                                 });
                catch_c_reduce->radius=std::get<0>(catch_c_reduce->Items[catch_c_reduce->Items.size()-1]);
            }
            update_item_hdr(catch_c_reduce->ptr, I, mode);
        }
    }
}




void HDR_Tree::update_user_add_doubletree_prune(){

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    add_user_num=add_user_num+1;

    //std::cout<<"Complete producing user"<<std::endl;



    new_user->center_low=data->Mat_low_d*new_user->center;

    /*projectUser_item(new_user);
    for(auto u_lowd_item:Trans_u_i){
        new_user->u_low_d_i.emplace_back(u_lowd_item);
    }*/

    //std::cout<<"1"<<std::endl;

    userComputeKNN_2(itemRoot_i, new_user);

    //std::cout<<"2"<<std::endl;

    Users.emplace_back(new_user);



    update_user_hdr(new_user, Mode::Add, root);



    /*auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    //update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;*/


};

void HDR_Tree::update_item_add_doubletree_prune(){
    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    add_item_num = add_item_num + 1;


    slidingWindow.emplace_back(new_item);
    update_item_hdr(itemRoot_i, new_item, Mode::Add);

    new_item->v_low=data->Mat_low_d*new_item->v;

    Rp.clear();

    projectItem(new_item);

    Search_fast(root, new_item);

    if (Rp.size() > 0) {

        //std::cout<<Rp.size()<<std::endl;

        for (auto u:Rp) {
            u->update(new_item, Mode::Add);
        }

        AdjustMaxdknnPaper(root);

    }

};

void HDR_Tree::update_item_delete_doubletree_prune(){


    auto i_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());


    //std::chrono::steady_clock::time_point check_time_start;
    //std::chrono::steady_clock::time_point check_time_end;

    //check time for others; check time for others; check time for others; check time for others; check time for others;
    //check_time_start=std::chrono::steady_clock::now();
    //check time for others; check time for others; check time for others; check time for others; check time for others;

    update_item_hdr(itemRoot_i,i_out,Mode::Remove);

    Rp.clear();

    projectItem(i_out);

    Search_fast(root, i_out);

    //check time for others; check time for others; check time for others; check time for others; check time for others;
    //check_time_end=std::chrono::steady_clock::now();
    //time_other=time_other+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
    //check time for others; check time for others; check time for others; check time for others; check time for others;


    if(Rp.size()>0){

        //std::cout<<Rp.size()<<std::endl;

        for(auto u:Rp){

            //check time for others; check time for others; check time for others; check time for others; check time for others;
            //check_time_start=std::chrono::steady_clock::now();
            //check time for others; check time for others; check time for others; check time for others; check time for others;

            userComputeKNN_2(itemRoot_i, u);

            //check time for others; check time for others; check time for others; check time for others; check time for others;
            //check_time_end=std::chrono::steady_clock::now();
            //time_compute_knn_cross=time_compute_knn_cross+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
            //check time for others; check time for others; check time for others; check time for others; check time for others;

        }

        AdjustMaxdknnPaper(root);

    }





    /*auto i_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());*/
    /*update_item_hdr(itemRoot_i,i_out,Mode::Remove);

    Rp.clear();

    projectItem(i_out);

    Search(root, i_out);

    if(Rp.size()>0){
        for(auto u:Rp){
            userComputeKNN(itemRoot_i, u);
        }
    }

    AdjustMaxdknnPaper(root);*/

    /*for(auto u:Users){
        u->computeKNN(slidingWindow);
    }*/

};










void HDR_Tree::update_user(){
    auto u_out=Users[0];
    Users.erase(Users.begin());
    update_user_hdr(u_out,Mode::Remove, root);


    /*auto u_out=Users[0];
    Users.erase(Users.begin());*/
    //update_user_hdr(u_out,Mode::Remove, root);
}

void HDR_Tree::update_item(){
    auto i_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());


    //std::chrono::steady_clock::time_point check_time_start;
    //std::chrono::steady_clock::time_point check_time_end;

    //check time for others; check time for others; check time for others; check time for others; check time for others;
    //check_time_start=std::chrono::steady_clock::now();
    //check time for others; check time for others; check time for others; check time for others; check time for others;

    update_item_hdr(itemRoot_i,i_out,Mode::Remove);

    Rp.clear();

    projectItem(i_out);

    Search(root, i_out);

    //check time for others; check time for others; check time for others; check time for others; check time for others;
    //check_time_end=std::chrono::steady_clock::now();
    //time_other=time_other+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
    //check time for others; check time for others; check time for others; check time for others; check time for others;


    if(Rp.size()>0){

        //std::cout<<Rp.size()<<std::endl;

        for(auto u:Rp){

            //check time for others; check time for others; check time for others; check time for others; check time for others;
            //check_time_start=std::chrono::steady_clock::now();
            //check time for others; check time for others; check time for others; check time for others; check time for others;

            userComputeKNN(itemRoot_i, u);

            //check time for others; check time for others; check time for others; check time for others; check time for others;
            //check_time_end=std::chrono::steady_clock::now();
            //time_compute_knn_cross=time_compute_knn_cross+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
            //check time for others; check time for others; check time for others; check time for others; check time for others;

        }

        AdjustMaxdknnPaper(root);
    }





    /*auto i_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());*/
    /*update_item_hdr(itemRoot_i,i_out,Mode::Remove);

    Rp.clear();

    projectItem(i_out);

    Search(root, i_out);

    if(Rp.size()>0){
        for(auto u:Rp){
            userComputeKNN(itemRoot_i, u);
        }
    }

    AdjustMaxdknnPaper(root);*/

    /*for(auto u:Users){
        u->computeKNN(slidingWindow);
    }*/

}

void HDR_Tree::update_user_add(){


    std::chrono::steady_clock::time_point begin_clock;

    std::chrono::steady_clock::time_point end_clock;

    begin_clock=std::chrono::steady_clock::now();

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);

    //float rate_nuprun;

    //unprun_num_knn=0;

    end_clock=std::chrono::steady_clock::now();

    time_update_user_add_exceptknn=time_update_user_add_exceptknn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock-begin_clock).count();

    //std::cout<<"Complete producing user"<<std::endl;



    /*new_user->center_low=data->Mat_low_d*new_user->center;

    projectUser_item(new_user);
    for(auto u_lowd_item:Trans_u_i){
        new_user->u_low_d_i.emplace_back(u_lowd_item);
    }*/


    begin_clock=std::chrono::steady_clock::now();

    userComputeKNN(itemRoot_i, new_user);



    end_clock=std::chrono::steady_clock::now();

    time_update_user_add_knn=time_update_user_add_knn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock-begin_clock).count();


    begin_clock=std::chrono::steady_clock::now();

    Users.emplace_back(new_user);

    //rate_nuprun=(unprun_num_knn*1.0)/(windowSize*1.0);

    //vector_unprun_proportion_knn.emplace_back(rate_nuprun);





    update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;

    end_clock=std::chrono::steady_clock::now();

    time_update_user_add_exceptknn=time_update_user_add_exceptknn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock-begin_clock).count();

    /*auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    //update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;*/


}

void HDR_Tree::update_user_add_fast(){

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);

    //std::cout<<"Complete producing user"<<std::endl;



    new_user->center_low=data->Mat_low_d*new_user->center;

    projectUser_item(new_user);
    for(auto u_lowd_item:Trans_u_i){
        new_user->u_low_d_i.emplace_back(u_lowd_item);
    }


    userComputeKNN_2(itemRoot_i, new_user);



    Users.emplace_back(new_user);



    update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;

    /*auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    //update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;*/


}
void HDR_Tree::update_user_add_batchversion(long n){


    num_total_meet=0;
    num_cal_twice=0;

    int i;
    for(i=0; i<n; i++){
        update_user_add_fast();
    }

    std::cout<<"Totally meet "<<num_total_meet<<" itmes, and within which, "<<num_cal_twice<<" items need two times of calculation"<<std::endl;

};

void HDR_Tree::update_user_add_statistic(){

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);

    //std::cout<<"Complete producing user"<<std::endl;



    new_user->center_low=data->Mat_low_d*new_user->center;

    projectUser_item(new_user);
    for(auto u_lowd_item:Trans_u_i){
        new_user->u_low_d_i.emplace_back(u_lowd_item);
    }


    userComputeKNN_statistic(itemRoot_i, new_user, &p1_adduser, &p2_adduser);



    Users.emplace_back(new_user);



    update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;

    /*auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    //update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;*/


};

void HDR_Tree::update_user_add_batchstatistic(long n){

    p2_adduser.clear();
    p1_adduser.clear();
    int i;
    for(i=0; i<n; i++){
        update_user_add_statistic();
    }


    if(p2_adduser.size()!=0){
        float p2=0;
        for(auto p:p2_adduser){
            p2=p2+p;
        }
        p2=float(p2/(p2_adduser.size()*1.0));
        std::cout<<"p2 is "<<p2<<std::endl;
    }
    else{
        std::cout<<"For an arbitrary user, all the met items are updated into its knn list, or all the items that are not updated into its knn list are pruned by one time calculation."<<std::endl;
    }


    if(p1_adduser.size()!=0){
        float p1=0;
        for(auto p:p1_adduser){
            p1=p1+p;
        }
        p1=float(p1/(p1_adduser.size()*1.0));
        std::cout<<"p1 is "<<p1<<std::endl;
    }
    else{
        std::cout<<"No items are updated into the candidate knn lists"<<std::endl;
    }


    p2_adduser.clear();
    p1_adduser.clear();

};


void HDR_Tree::update_item_add() {
    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    update_item_hdr(itemRoot_i, new_item, Mode::Add);




    unprun_num_rknn=0;

    float rate_unprun;


    Rp.clear();

    projectItem(new_item);

    //new_item->v_low=data->Mat_low_d*new_item->v;

    //std::chrono::steady_clock::time_point time_start_search = std::chrono::steady_clock::now();

    //std::cout<<"Start RkNN Search"<<std::endl;
    Search(root, new_item);
    //std::cout<<"End RkNN Search"<<std::endl;
    //std::chrono::steady_clock::time_point time_end_search = std::chrono::steady_clock::now();

    //time_statistic=time_statistic+std::chrono::duration_cast<std::chrono::microseconds>( time_end_search- time_start_search).count();

    rate_unprun=(unprun_num_rknn*1.0)/((numUsers+insert_user_num)*1.0);

    vector_unprun_proportion_rknn.emplace_back(rate_unprun);



    if (Rp.size() > 0) {

        //std::cout<<Rp.size()<<std::endl;

        for (auto u:Rp) {
            u->update(new_item, Mode::Add);
        }

        AdjustMaxdknnPaper(root);

    }

    add_item_num = add_item_num + 1;

}
    /*auto new_item= new Item(data_i->I.row(numItems_i+add_item_num));
    slidingWindow.emplace_back(new_item);*/
    /*update_item_hdr(itemRoot_i,new_item, Mode::Add);

    Rp.clear();

    projectItem(new_item);

    Search(root, new_item);

    if(Rp.size()>0){
        for(auto u:Rp){
            u->update(new_item, Mode::Add);
        }
    }

    AdjustMaxdknnPaper(root);

    for(auto u:Users){
        u->computeKNN(slidingWindow);
    }


    add_item_num=add_item_num+1;*/
//};

void HDR_Tree::Search_statistic(Node* node, Item* I){

    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);



        for (auto Ut : *LN) {


            meet_num_additem=meet_num_additem+1;

            num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);

            if((user->center_low-I->v_low).norm()<user->dknn){

                float distance=dist(I, user);

                twice_num_additem=twice_num_additem+1;

                if ( distance< user->dknn) {
                    Rp.emplace_back(user);

                    update_num_additem=update_num_additem+1;


                }
            }
        }

        /*long num_not_affected=num_meet-num_affected;
        long num_not_affected_twice=num_twice-num_affected;
        if(num_meet!=0){
            float p1=float((1.0f*num_affected)/(1.0f*num_meet));
            p1_itemunit.emplace_back(p1);
        }
        if(num_not_affected!=0){
            float p2=float((1.0f*num_not_affected_twice)/(1.0f*num_not_affected));
            p2_itemunit.emplace_back(p2);
        } */


    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                Search_statistic(NLN->children[j], I);
            }
        }
    }
};







void HDR_Tree::update_item_add_statistic(){

    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    update_item_hdr(itemRoot_i, new_item, Mode::Add);

    //p1_itemunit.clear();

    //p2_itemunit.clear();

    Rp.clear();

    projectItem(new_item);

    new_item->v_low=data->Mat_low_d*new_item->v;

    Search_statistic(root, new_item);

    if (Rp.size() > 0) {

        //std::cout<<Rp.size()<<std::endl;

        for (auto u:Rp) {
            u->update(new_item, Mode::Add);
        }

        AdjustMaxdknnPaper(root);

    }

    /*float sum_p1=0;
    float sum_p2=0;

    for(auto p1_gained:p1_itemunit){
        sum_p1=sum_p1+p1_gained;
    }


    if(p1_itemunit.size()!=0){
        float average_p1=float(sum_p1/(1.0*p1_itemunit.size()));
        p1_additem.emplace_back(average_p1);
        p1_itemunit.clear();
    }


    if(p2_itemunit.size()!=0){
        for(auto p2_gained:p2_itemunit){
            sum_p2=sum_p2+p2_gained;
        }
        float average_p2=float(sum_p2/(1.0*p2_itemunit.size()));
        p2_additem.emplace_back(average_p2);
        p2_itemunit.clear();
    }*/









    add_item_num = add_item_num + 1;

};
void HDR_Tree::update_item_add_batch_statistic(long n){

    //p2_additem.clear();

    meet_num_additem=0;
    update_num_additem=0;
    twice_num_additem=0;

    int i;
    for(i=0; i<n;i++){
        update_item_add_statistic();

    }

    if((meet_num_additem-update_num_additem)!=0){
        float p2=float(((twice_num_additem-update_num_additem)*1.0)/((meet_num_additem-update_num_additem)*1.0));
        std::cout<<"p2 is "<<p2<<std::endl;
    }else{
        std::cout<<"Totally meet "<<meet_num_additem<<" users, and "<<update_num_additem<<" users are affected, and "<<twice_num_additem<<" users are processed by two times of calculation. ";
    }

    float p1=float((update_num_additem*1.0)/(meet_num_additem*1.0));
    //p1_additem.emplace_back(p1);
    std::cout<<"p1 is "<<p1<<std::endl;
    /*if(p2_additem.size()!=0){



        float p2=0;
        for(auto p:p2_additem){
            p2=p2+p;
        }
        p2=float(p2/(p2_additem.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"For an arbitrary user, all the met items are updated into its knn list, or all the items that are not updated into its knn list are pruned by one time calculation."<<std::endl;
    }*/

    //p2_additem.clear();
};




void HDR_Tree::update_item_add_assemble(long n){
    /*auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    //update_item_hdr(itemRoot_i, new_item, Mode::Add);

    for(auto user:Users){
        if (dist(new_item, user) < user->dknn) {
            user->update(new_item, Mode::Add);
        }
    }


    add_item_num = add_item_num + 1;*/



    int i;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
        slidingWindow.emplace_back(new_item);
        update_item_hdr(itemRoot_i, new_item, Mode::Add);

        Rp.clear();

        projectItem(new_item);

        Search(root, new_item);

        if (Rp.size() > 0) {

            //std::cout<<Rp.size()<<std::endl;

            for (auto u:Rp) {
                u->update(new_item, Mode::Add);
            }

            AdjustMaxdknnPaper(root);

        }
        add_item_num = add_item_num + 1;
    }

    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/


};







void HDR_Tree::update_user_user_tree(){
    auto u_out=Users[0];
    Users.erase(Users.begin());
    update_user_hdr(u_out,Mode::Remove, root);
};

void HDR_Tree::update_user_add_user_tree(){

    //std::cout<<"1"<<std::endl;

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);

    //std::cout<<"2"<<std::endl;

    new_user->computeKNN(slidingWindow);

    //std::cout<<"3"<<std::endl;

    Users.emplace_back(new_user);

    //std::cout<<"4"<<std::endl;

    update_user_hdr(new_user,Mode::Add, root);

    //std::cout<<"5"<<std::endl;

    add_user_num=add_user_num+1;

    //std::cout<<"6"<<std::endl;
};

void HDR_Tree::update_item_user_tree(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    Rp.clear();

    //std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, beginning

    //int test_0=2;

    projectItem(item_out);

    /*std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, ending

    int test_1=3;

    projection_time=projection_time+test_1-test_0;//std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();   //Add the projection time

    std::chrono::steady_clock::time_point begin_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, beginning*/

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_hdr_cal_clusters=0;
    //num_hdr_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    Search(root, item_out);

    /*std::cout<<"The number of influenced users "<<Rp.size()<<std::endl;

    std::cout<<"The dknn values before recomputing the knn lists of deletion"<<std::endl;

    for(auto user_before: Rp){
        std::cout<<user_before->dknn<<" ";
    }
    std::cout<<" "<<std::endl;*/

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_hdr_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_hdr_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    /*std::chrono::steady_clock::time_point end_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, ending

    search_time_HDR=search_time_HDR+std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add_hdr_0 - begin_clock_item_add_hdr_0).count();*/


    if(Rp.size()>0){
        for(auto u:Rp){
            u->computeKNN(slidingWindow);
        }
        AdjustMaxdknnPaper(root);
    }



    /*std::cout<<"The dknn values after recomputing the knn lists of deletion"<<std::endl;

    for(auto user_after:Rp){
        std::cout<<user_after->dknn<<" ";
    }

    std::cout<<" "<<std::endl;

    std::cout<<" "<<std::endl;*/

};

void HDR_Tree::update_item_add_user_tree(){
    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    //update_item_hdr(itemRoot_i, new_item, Mode::Add);

    //new_item->v_low=data->Mat_low_d*new_item->v_low;

    Rp.clear();

    projectItem(new_item);

    Search(root, new_item);

    if (Rp.size() > 0) {
        for (auto u:Rp) {
            u->update(new_item, Mode::Add);
        }

        AdjustMaxdknnPaper(root);

    }




    add_item_num = add_item_num + 1;
};


void HDR_Tree::update_item_add_naive(long n){
    /*auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    //update_item_hdr(itemRoot_i, new_item, Mode::Add);

    for(auto user:Users){
        if (dist(new_item, user) < user->dknn) {
            user->update(new_item, Mode::Add);
        }
    }



    add_item_num = add_item_num + 1;*/

    //distance_time=0;

    float distance_user_item;
    distance_time=0;
    item_add_update=0;
    time_distance_calculation=0;
    time_update_user_knn_additem=0;


    int i;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));

        //new_item->v_low=data->Mat_low_d*new_item->v;

        slidingWindow.emplace_back(new_item);
        //update_item_hdr(itemRoot_i, new_item, Mode::Add);

        for(auto user:Users){

            distance_time=distance_time+1;

            std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

            distance_user_item=distance_check(user, new_item);

            std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
            float diff_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance -begin_clock_calculatedistance).count();
            time_distance_calculation=time_distance_calculation+diff_1;





            if ( distance_user_item< user->dknn) {



                std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                user->update(new_item, Mode::Add);


                std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                time_update_user_knn_additem=time_update_user_knn_additem+diff_2;

                item_add_update=item_add_update+1;



            }
        }


        add_item_num = add_item_num + 1;
    }

    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/

    std::cout<<"The times of calculating the full dimensional distance is "<<distance_time<<std::endl;
    std::cout<<"The times of updating new items into users is "<<item_add_update<<std::endl;
    std::cout<<"The time for calculation of distance is "<<time_distance_calculation/1000000000.0<<std::endl;

    std::cout<<"The time for update the added items into the users is "<<time_update_user_knn_additem/1000000000.0<<std::endl;

    std::cout<<"The average time for distance calculation is "<<time_distance_calculation/(distance_time*1.0)<<std::endl;
};


void HDR_Tree::update_user_add_deltatree(){

    auto new_user= new User(data->U.row(numUsers+add_user_num), k);

    //std::cout<<"Complete producing user"<<std::endl;



    /*new_user->center_low=data->Mat_low_d*new_user->center;

    projectUser_item(new_user);
    for(auto u_lowd_item:Trans_u_i){
        new_user->u_low_d_i.emplace_back(u_lowd_item);
    }*/


    userComputeKNN(itemRoot_i, new_user);



    Users.emplace_back(new_user);



    update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;

    /*auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    //update_user_hdr(new_user, Mode::Add, root);

    add_user_num=add_user_num+1;*/


};



//item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function item_hdr_tree_function


void HDR_Tree::update_item_add_deltatree(){

    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    update_item_hdr(itemRoot_i, new_item, Mode::Add);

    //new_item->v_low=data->Mat_low_d*new_item->v;

    for(auto user: Users){
        if( (user->center-new_item->v).norm()<user->dknn){
            user->update(new_item, Mode::Add);
        }
    }

    add_item_num = add_item_num + 1;

}

void HDR_Tree::update_item_deltatree() {
    auto item_out = slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());
    update_item_hdr(itemRoot_i, item_out, Mode::Remove);

    //new_item->v_low=data->Mat_low_d*new_item->v;



    for (auto user: Users) {
        if ((user->center - item_out->v).norm() < user->dknn) {
            userComputeKNN(itemRoot_i, user);
        }
    }
}


bool HDR_Tree::AdjustMaxdknnsphere(userNode* n) {

    bool result = false;

    if (n->type == userNodeType::userLeafNode) {
        auto LN = dynamic_cast<userLeafNode*>(n);

        long Len = Rp_sphere.size();
        for (auto Ut : *LN) {
            auto& user = std::get<1>(Ut);
            for (long i = 0; i < Len; i++) {
                if (Rp_sphere[i] == user) {
                    return true;
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<userNonLeafNode*>(n);
        auto& Cp = NLN->clusters;

        for (auto Cj : Cp) {
            bool res = AdjustMaxdknnsphere(Cj->ptr);

            if (res) {
                result = true;

                float Dist = 0;

                if (Cj->ptr->type == userNodeType::userLeafNode) {

                    auto LN = dynamic_cast<userLeafNode*>(Cj->ptr);
                    for (auto Ut : *LN) {

                        auto user = std::get<1>(Ut);
                        Dist = std::max(Dist, user->dknn);
                    }
                }
                else {

                    auto NLN = dynamic_cast<userNonLeafNode*>(Cj->ptr);
                    auto Cp = NLN->clusters;

                    for (auto Cj : Cp) {
                        Dist = std::max(Dist, float(Cj->maxdknn));
                    }
                }

                Cj->maxdknn = Dist;
            }
        }
    }

    return result;
}


void HDR_Tree::update_sphere(User* U, Mode m, userNode* node){
    if(m==Mode::Add){

        if(node->type==userNodeType::userNonLeafNode){
            auto NLN=dynamic_cast<userNonLeafNode*>(node);
            std::vector<std::tuple<float, Clusteru*>>V;
            auto v_transformed=U->center;
            for(auto C_0: NLN->clusters){
                V.emplace_back(std::make_tuple((v_transformed-(C_0->center).cast<float>()).norm(), C_0));
            }
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, Clusteru*>& a, const std::tuple<float, Clusteru* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            auto C_add=std::get<1>(V[V.size()-1]);
            C_add->Users.emplace_back(std::make_tuple(std::get<0>(V[V.size()-1]), U));
            C_add->number=C_add->number+1;
            std::sort(C_add->Users.begin(),  C_add->Users.end(),
                      [&](const std::tuple<float, User*>& a, const std::tuple<float, User*>& b) -> bool {
                          return std::get<0>(a) <= std::get<0>(b);
                      });
            C_add->radius=std::get<0>(C_add->Users[(C_add->Users).size()-1]);
            if (double(U->dknn)>C_add->maxdknn){
                C_add->maxdknn=double(U->dknn);
            }
            update_sphere( U, m, C_add->ptr);
        }
    }else{
        if(node->type==userNodeType::userNonLeafNode){
            auto NLN=dynamic_cast<userNonLeafNode*>(node);
            //std::cout<<"1"<<std::endl;   //show show show show show show show show
            Clusteru* C_catch;

            //std::cout<<"The original state of the C_catch "<<(C_catch->Users).size()<<std::endl;   //show show show show show show show show

            long i_catch;
            for(auto C:NLN->clusters){
                if(C->number!=0){
                    //std::cout<<"Not zero"<<std::endl;         //show show show show show show show
                    bool indicator= false;
                    for(long i=0; i<(C->Users).size(); i++){
                        if(std::get<1>(C->Users[i])==U){


                            C_catch=C;

                            //std::cout<<"catch success"<<std::endl;   //show show show show show show show

                            i_catch=i;
                            indicator= true;
                            break;
                        }
                    }
                    if(indicator==true){
                        break;
                    }
                }
            }

            //std::cout<<"2"<<std::endl;//show show show show show show show show
            //std::cout<<(C_catch->Users).size()<<" 3"<<std::endl;  //show show show show show show show show
            C_catch->number=C_catch->number-1;
            if(C_catch->number==0){
                C_catch->radius=0;
                C_catch->maxdknn=0;
                C_catch->Users.erase(C_catch->Users.begin()+i_catch);
            }else{
                //std::cout<<"4"<<std::endl;//show show show show show show show show
                //std::cout<<(C_catch->Users).size()<<" 5"<<std::endl;  //show show show show show show show show
                C_catch->Users.erase(C_catch->Users.begin()+i_catch);
                //std::cout<<"6"<<std::endl;//show show show show show show show show
                //std::cout<<(C_catch->Users).size()<<" 7"<<std::endl;   //show show show show show show show show
                C_catch->radius=std::get<0>(C_catch->Users[C_catch->Users.size()-1]);
                //std::cout<<"8"<<std::endl;//show show show show show show show show
                if(U->dknn==C_catch->maxdknn){
                    std::vector<float> V;
                    for(auto tp: C_catch->Users){
                        V.emplace_back(std::get<1>(tp)->dknn);
                    }
                    std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                                     [&](const float& a, const float& b) -> bool {
                                         return a<=b;
                                     });
                    C_catch->maxdknn=V[V.size()-1];
                }
            }

            update_sphere( U, m, C_catch->ptr);
        }
    }
}

void HDR_Tree::update_user_spheretree(){
    auto u_out=Users[0];
    Users.erase(Users.begin());
    update_sphere(u_out,Mode::Remove, root_sphere);
};

void HDR_Tree::update_user_add_spheretree(){
    auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    new_user->computeKNN(slidingWindow);
    Users.emplace_back(new_user);
    update_sphere(new_user,Mode::Add, root_sphere);
    add_user_num=add_user_num+1;
};

void HDR_Tree::update_item_spheretree(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    Rp_sphere.clear();



    //projectItem(item_out);

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_sphere_cal_clusters=0;
    //num_sphere_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    search_sphere(root_sphere, item_out);

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_sphere_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_sphere_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    if(Rp_sphere.size()>0){

        //std::cout<<Rp_sphere.size()<<std::endl;

        for(auto u:Rp_sphere){
            u->computeKNN(slidingWindow);
        }
        AdjustMaxdknnsphere(root_sphere);
    }
};

void HDR_Tree::update_item_add_spheretree(){
    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);

    //std::cout<<"The center of the new item is "<<std::endl;

    //std::cout<<new_item->v<<std::endl;



    Rp_sphere.clear();

    search_sphere(root_sphere, new_item);
    //std::cout<<" "<<std::endl;
    //std::cout<<"The number of influenced users "<<Rp_sphere.size()<<std::endl;
    //std::cout<<" "<<std::endl;
    //std::cout<<"The centers of the affected users and the dknn values of the affected users before recomputing the knn lists of deletion"<<std::endl;

    /*for(auto user_before: Rp_sphere){
        //std::cout<<"The center of the affected user: "<<std::endl;
        //std::cout<<user_before->center<<std::endl;
        std::cout<<" The dknn of the affected user: "<<user_before->dknn<<" "<<std::endl;
        //std::cout<<" "<<std::endl;
    }*/
    //std::cout<<" "<<std::endl;
    //std::cout<<" "<<std::endl;

    if (Rp_sphere.size() > 0) {

        for (auto u:Rp_sphere) {

            /*std::cout<<"The center of the affected user: "<<std::endl;
            std::cout<<u->center<<std::endl;
            std::cout<<" The dknn of the affected user: "<<u->dknn<<" "<<std::endl;
            std::cout<<" "<<std::endl;*/
            u->update(new_item, Mode::Add);
        }
        AdjustMaxdknnsphere(root_sphere);
    }

    //std::cout<<" "<<std::endl;
    //std::cout<<" "<<std::endl;

    /*std::cout<<"The dknn values after recomputing the knn lists of deletion"<<std::endl;

    for(auto user_after:Rp_sphere){
        std::cout<<user_after->dknn<<" ";
    }

    std::cout<<" "<<std::endl;*/

    //std::cout<<" "<<std::endl;



    add_item_num = add_item_num + 1;
};

void HDR_Tree::update_user_knnjoinplus(){
    auto u_out=Users[0];
    Users.erase(Users.begin());
    update_sphere(u_out,Mode::Remove, root_sphere);
};
void HDR_Tree::update_user_add_knnjoinplus(){
    auto new_user= new User(data->U.row(numUsers+add_user_num), k);
    user_computeknn_id(new_user, 0.01, 0.01, false);
    Users.emplace_back(new_user);
    update_sphere(new_user,Mode::Add, root_sphere);
    add_user_num=add_user_num+1;
}

void HDR_Tree::update_item_knnjoinplus(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    bptree_item->Remove(item_out->index_b_plus);

    /*if((item_out->index_b_plus-130.068493)*(item_out->index_b_plus-130.068493)<0.000001){
        std::cout<<"Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch Catch"<<std::endl;
    }*/

    Rp_sphere.clear();

    //projectItem(item_out);

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_sphere_cal_clusters=0;
    //num_sphere_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    search_sphere(root_sphere, item_out);

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_sphere_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_sphere_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    if(Rp_sphere.size()>0){

        //std::cout<<Rp_sphere.size()<<std::endl;
        //int count_user=1;
        for(auto u:Rp_sphere){

            //std::cout<<"Start calculate the knn list of the "<<count_user<<"th user"<<std::endl;

            //std::cout<<" "<<std::endl;

            user_computeknn_id(u,0.01,0.01, false);

            //std::cout<<"Complete calculate the knn list of the "<<count_user<<"th user"<<std::endl;

            //std::cout<<" "<<std::endl;

            //count_user=count_user+1;
        }
        AdjustMaxdknnsphere(root_sphere);
    }
};




void HDR_Tree::update_item_add_knnjoinplus(){

    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);

    //std::cout<<"The center of the new item is "<<std::endl;
    //std::cout<<new_item->v<<std::endl;

    Rp_sphere.clear();

    search_sphere(root_sphere, new_item);

    //std::cout<<" "<<std::endl;

    //std::cout<<"The number of influenced users: "<<Rp_sphere.size()<<std::endl;

    //std::cout<<" "<<std::endl;

    //std::cout<<"The centers of the affected users and the dknn values of the affected users before recomputing the knn lists of deletion"<<std::endl;

    //std::cout<<" "<<std::endl;

    /*for(auto user_before: Rp_sphere){
        //std::cout<<"The center of the affected user: "<<std::endl;
        //std::cout<<user_before->center<<std::endl;
        std::cout<<" The dknn of the affected user: "<<user_before->dknn<<" "<<std::endl;
    }
    //std::cout<<" "<<std::endl;
    std::cout<<" "<<std::endl;*/


    if (Rp_sphere.size() > 0) {

        for (auto u:Rp_sphere) {
            /*std::cout<<"The center of the affected user: "<<std::endl;
            std::cout<<u->center<<std::endl;
            std::cout<<" The dknn of the affected user: "<<u->dknn<<" "<<std::endl;*/
            u->update(new_item, Mode::Add);
            //std::cout<<" "<<std::endl;
        }

    }

    AdjustMaxdknnsphere(root_sphere);

    //std::cout<<" "<<std::endl;
    //std::cout<<" "<<std::endl;

    /*std::cout<<"The dknn values after recomputing the knn lists of deletion"<<std::endl;

    for(auto user_after:Rp_sphere){
        std::cout<<user_after->dknn<<" ";
    }

    std::cout<<" "<<std::endl;*/

    //std::cout<<" "<<std::endl;


    add_item_num = add_item_num + 1;


    //The original version of the add function introduced from the sphere mode
};







//Functions for the sphere tree




//Funtion for optimization of deletion

void HDR_Tree::update_item_op_delet_usertree(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    Rp.clear();

    //std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, beginning

    //int test_0=2;

    projectItem(item_out);

    /*std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, ending

    int test_1=3;

    projection_time=projection_time+test_1-test_0;//std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();   //Add the projection time

    std::chrono::steady_clock::time_point begin_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, beginning*/

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_hdr_cal_clusters=0;
    //num_hdr_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    Search(root, item_out);

    /*std::cout<<"The number of influenced users "<<Rp.size()<<std::endl;

    std::cout<<"The dknn values before recomputing the knn lists of deletion"<<std::endl;

    for(auto user_before: Rp){
        std::cout<<user_before->dknn<<" ";
    }
    std::cout<<" "<<std::endl;*/


    //std::cout<<"Check 1"<<std::endl;

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_hdr_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_hdr_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    /*std::chrono::steady_clock::time_point end_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, ending

    search_time_HDR=search_time_HDR+std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add_hdr_0 - begin_clock_item_add_hdr_0).count();*/


    if(Rp.size()>0){
        for(auto u:Rp){
            std::vector<float> stored_bound;
            auto LN=dynamic_cast<LeafNode*>(u->belong_leaf);
            for(auto u_friend:LN->cluster->Users){
                for(auto i_knn: std::get<1>(u_friend)->R){
                    float dist_bound=dist(i_knn,u);
                    if (dist_bound>u->dknn){
                        stored_bound.emplace_back(dist_bound);
                    }
                }
            }
            if(stored_bound.size()!=0){
                float bound_deletion=*min_element(stored_bound.begin(),stored_bound.end());
                u->update(item_out, Mode::Remove);
                /*float floor_bound=dist(u->R[k-2], u);*/
                std::vector<std::tuple<float, Item*>> stored_knn;
                for(auto item_0: slidingWindow){
                    float dist_low=(u->u_low_d[2]-item_0->i_low_d[2]).norm();
                    if(dist_low<=bound_deletion){
                        float real_dist=dist(item_0, u);
                        if(real_dist>u->dknn){
                            stored_knn.emplace_back(std::make_tuple(real_dist, item_0));
                        }
                    }
                }
                if(stored_knn.size()!=0){
                    std::nth_element(stored_knn.begin(), stored_knn.begin() + stored_knn.size() - 1, stored_knn.end(),
                                     [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item* >& b) -> bool {
                                         return std::get<0>(a) > std::get<0>(b);
                                     });
                    u->R.emplace_back(std::get<1>(stored_knn[stored_knn.size()-1]));
                    u->dknn=std::get<0>(stored_knn[stored_knn.size()-1]);
                }else{
                    u->computeKNN_sort(slidingWindow);
                }

            }else{
                u->computeKNN_sort(slidingWindow);
            }
        }

        AdjustMaxdknnPaper(root);


    }

    /*std::cout<<"The dknn values after recomputing the knn lists of deletion"<<std::endl;

    for(auto user_after:Rp){
        std::cout<<user_after->dknn<<" ";
    }

    std::cout<<" "<<std::endl;

    std::cout<<" "<<std::endl;*/


};

void HDR_Tree::search_deletion_opt(Node* node, Item* I){
    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {

            num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);
            if (dist(I, user) < user->dknn) {
                Rp_0.emplace_back(user);
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters
            std::cout<<"Check 2"<<std::endl;
            d1 = (I->i_low_d[Cp[j]->l - 1] - Cp[j]->center).norm();
            std::cout<<"Check 3"<<std::endl;
            d2 = Cp[j]->maxdknn+Cp[j]->radius;
            if (d1 <= d2) {
                search_deletion_opt(NLN->children[j], I);
            }
        }
    }
}





//Funtion for optimization of deletion

//Funtion for the deletion update when using only one HDR tree for the users for comparison with the optimization deletion method, using computeKNN_sort.

void HDR_Tree::update_item_user_tree_compute_sort(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    Rp.clear();

    //std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, beginning

    //int test_0=2;

    projectItem(item_out);

    /*std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, ending

    int test_1=3;

    projection_time=projection_time+test_1-test_0;//std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();   //Add the projection time

    std::chrono::steady_clock::time_point begin_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, beginning*/

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_hdr_cal_clusters=0;
    //num_hdr_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    Search(root, item_out);

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_hdr_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_hdr_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    /*std::chrono::steady_clock::time_point end_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, ending

    search_time_HDR=search_time_HDR+std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add_hdr_0 - begin_clock_item_add_hdr_0).count();*/


    if(Rp.size()>0){
        for(auto u:Rp){
            u->computeKNN_sort(slidingWindow);
        }
    }

    AdjustMaxdknnPaper(root);
};


//Function for the deletion update when using only one HDR tree for the users for comparison with the optimization deletion method, using computeKNN_sort.

//The function for the distance between item and user cluster, combination of dimension reduction and distance calculation


float HDR_Tree::distance_u_ucluster(Item *i, Cluster *c) {
    numDistanceComputions++;
    float res = (data->T[c->l-1] * i->v - c->center).norm();
    return res - c->radius;
}

//The function for the distance between item and user cluster, combination of dimension reduction and distance calculation

//The function for building the little tree The function for building the little tree The function for building the little tree

int HDR_Tree::construct_littletree(std::vector<User *> *tree_user, LeafNode *tree_node, NonLeafNode*tree_root, DataBase *tree_data,
                                    long fan, long thresh) {
    std::cout << "Constructing little HDR Tree" << std::endl;
    std::cout << "Users: " << tree_user->size() << std::endl;
    std::cout << "Features: " << numFeatures << std::endl;
    //std::cout << "Window: " << windowSize << std::endl;
    std::cout << "Fanout: " << fan << std::endl << std::endl;
    std::cout << "Threshold: "<<thresh<<std::endl<<std::endl;

    //init();

    tree_data->U.resize(tree_user->size(), numFeatures);

    int k = 0;
    for (long index = 0; index < tree_user->size(); index++)
    {

        tree_data->U.row(k) = (*tree_user)[index]->center;
        k = k + 1;
    }

    tree_data->computePCA();

    if(tree_user->size()<thresh){
        free(tree_root);
        auto C_single=new Cluster;
        for(auto user_less:*tree_user){
            C_single->Users.emplace_back(std::make_tuple(0,user_less));
        }
        tree_node->cluster=C_single;
        return 0;
    }else{
        free(tree_node);
        int L_tem;
        L_tem = log(tree_user->size()) / log(fan);
        std::cout << "Estimated Height: " << L_tem << " levels." << std::endl;

        tree_data->initTransform(L_tem);


        /*for(auto u_ini: Users){
            projectUser(u_ini);
            for(int count_d=0; count_d<L; count_d=count_d+1){
                u_ini->u_low_d.emplace_back(uTrans[count_d]);
            }
        }


        for(auto i_ini: slidingWindow){
            projectItem(i_ini);
            for(int count_d_0=0; count_d_0<L_i; count_d_0=count_d_0+1){
                i_ini->i_low_d.emplace_back(iTrans[count_d_0]);
            }
        }*/



        long numClusters = 0;

        auto C = new Clusters(*tree_user, fan, tree_data->T[0]);
        for (auto Cj : *C) {

            if (Cj->Users.size() > 0) {
                Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
                for (auto t : Cj->Users) {
                    Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
                }
            }

            numClusters += 1;
            Cj->l = 1;
        }




        tree_root->l=1;
        //root = new NonLeafNode(C, 1, false);

        //tree_node->clusters=C->children;
        for(auto cluster_begin: C->children){
            tree_root->clusters.emplace_back(cluster_begin);
        }

        std::cout<<"Successfull goinng out"<<std::endl;

        std::queue<Node *> q;

        q.push(tree_root);

        long maxL = 0;

        while (!q.empty()) {
            auto node = dynamic_cast<NonLeafNode *>(q.front());
            q.pop();

            auto Cp = node->clusters;

            for (auto Cj : Cp) {

                if (Cj->number < thresh) {
                    auto LN = new LeafNode(Cj, false);

                    node->children.emplace_back(LN);
                    Cj->ptr = LN;


                    for(auto user_leaf: LN->cluster->Users){
                        std::get<1>(user_leaf)->belong_leaf=LN;
                    }


                }
                else {
                    auto level = node->l + 1;
                    maxL = std::max(maxL, level);

                    if(level>L_tem){
                        std::cout<<"Excceed the estimated level"<<std::endl;
                    }

                    auto Cpp = new Clusters(Cj, fan, tree_data->T[level-1]);
                    for (auto Cj : *Cpp) {

                        if (Cj->Users.size() > 0) {
                            Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
                            for (auto t : Cj->Users) {
                                Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
                            }
                        }

                        numClusters += 1;
                        Cj->l = level;
                    }

                    auto NLN = new NonLeafNode(Cpp, level, false);

                    node->children.emplace_back(NLN);
                    Cj->ptr = NLN;
                    q.push(NLN);
                }
            }
        }

        std::cout << "Actual Height: " << maxL << " levels." << std::endl;
        std::cout << "Clusters: " << numClusters << std::endl;
        return L_tem;
    }







}


//The function for building the little tree The function for building the little tree The function for building the little tree

//The function for building the clusters of the users The function for building the clusters of the users The function for building the clusters of the users

void HDR_Tree::build_clustersu(long n_c) {
    clusters_user_tree=new Clustersu(Users, n_c);
}

//The function for building the clusters of the users The function for building the clusters of the users The function for building the clusters of the users

//The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees

void HDR_Tree::complete_little_trees(long n_t, long l_f, long l_thresh){
    int divide_r;
    double r1;
    double r2;

    //This is for calculate the sequence of the cluster computed

    int k_cluster=1;

    //This is for calculate the sequence of the cluster computed

    for (auto cluster_u: *clusters_user_tree){

        //This is for calculate the sequence of the cluster computed

        std::cout<<"The "<<k_cluster<<"th cluster is being computed"<<std::endl;

        //This is for calculate the sequence of the cluster computed

        //This is for showing the number of users that the being computed cluster contains

        std::cout<<"The "<<k_cluster<<"th cluster has "<<cluster_u->Users.size()<<" users"<<std::endl;

        //This is for showing the number of users that the being computed cluster contains

        //This is for showing the radius of the being computed cluster

        std::cout<<"The "<<k_cluster<<"th cluster has the radius of "<<cluster_u->radius<<std::endl;

        //This is for showing the radius of the being computed cluster

        //Seperate the description of the cluster and the blocks

        std::cout<<" "<<std::endl;

        //Seperate the description of the cluster and the blocks

        for(divide_r=1; divide_r<=n_t; divide_r++){

            //This is for show the sequence of the block that is being computed

            std::cout<<"The "<<divide_r<<"th block is being computed"<<std::endl;

            //This is for show the sequence of the block that is being computed


            auto v_section=new std::vector<User*>;
            std::vector<std::tuple<double, User*>> distance_user;
            r1=cluster_u->radius*(divide_r-1)*1.0/(n_t*1.0);
            r2=cluster_u->radius*(divide_r)*1.0/(n_t*1.0);

            //This is for showing the lower bound and the upper bound of a block

            std::cout<<"The "<<divide_r<<"th block in the "<<k_cluster<<"th cluster has the lower bound of "<<r1<<" in the radius of "<<cluster_u->radius<<std::endl;

            std::cout<<"The "<<divide_r<<"th block in the "<<k_cluster<<"th cluster has the upper bound of "<<r2<<" in the radius of "<<cluster_u->radius<<std::endl;


            //This is for showing the lower bound and the upper bound of a block


            for(auto user_l:cluster_u->Users){
                if((std::get<0>(user_l)>r1)&&(std::get<0>(user_l)<=r2)){
                    distance_user.emplace_back(user_l);
                    v_section->emplace_back(std::get<1>(user_l));
                }
            }
            if(v_section->size()==0){
                free(v_section);

                //For informing that the block is empty
                std::cout<<"The "<<divide_r<<"th block is empty"<<std::endl;
                //For informing that the block is empty
            }else{

                //For informing the number of users in a block

                std::cout<<"The "<<divide_r<<"th block has "<<v_section->size()<<" users"<<std::endl;

                //For informing the number of users in a block


                cluster_u->section_user.emplace_back(v_section);
                std::vector<float> store_dknn;
                for(auto u_little:*v_section){
                    store_dknn.emplace_back(u_little->dknn);
                }
                std::sort(store_dknn.begin(),  store_dknn.end(),
                                 [&](const float& a, const float& b) -> bool {
                                     return a <=b;
                                 });
                std::sort(distance_user.begin(), distance_user.end(),
                                 [&](const std::tuple<double, User*>& a, const std::tuple<double, User* >& b) -> bool {
                                     return std::get<0>(a) <= std::get<0>(b);
                                 });
                //Clusters* substitude;

                //The information for the block

                std::cout<<"The maxdknn of the "<<divide_r<<"th block in the "<<k_cluster<<"th cluster is "<<store_dknn[store_dknn.size()-1]<<std::endl;
                std::cout<<"The real lower bound of the "<<divide_r<<"th block in the "<<k_cluster<<"th cluster is "<<std::get<0>(distance_user[0])<<std::endl;
                std::cout<<"The real upper bound of the "<<divide_r<<"th block in the "<<k_cluster<<"th cluster is "<<std::get<0>(distance_user[distance_user.size()-1])<<std::endl;
                //The information for the block

                //Start building tree

                std::cout<<"Start building the tree for the "<<divide_r<<"th block in the "<<k_cluster<<"th cluster"<<std::endl;

                //Start building tree
                std::cout<<"Temporary check, start, if the nullptr can be used to initialize a nonleafnode"<<std::endl;

                auto little_root=new NonLeafNode();
                little_root->type=NodeType::NonLeafNode;

                auto little_node=new LeafNode();
                little_node->type=NodeType::LeafNode;

                std::cout<<"Temporary check, complete, if the nullptr can be used to initialize a nonleafnode"<<std::endl;
                auto little_data=new DataBase;
                little_data->dim=numFeatures;
                int height_return=construct_littletree(v_section,little_node, little_root, little_data, l_f, l_thresh);
                auto little_tree=new Littletree;
                little_tree->height=height_return;
                little_tree->littledatabase=little_data;
                little_tree->whole_maxdknn=store_dknn[store_dknn.size()-1];
                little_tree->whole_radius=std::get<0>(distance_user[distance_user.size()-1]);
                little_tree->whole_low_radius=std::get<0>(distance_user[0]);
                if(height_return==0){
                    little_tree->littleroot=little_node;
                }else{
                    little_tree->littleroot=little_root;
                }
                //little_tree->height=height_return;
                little_tree->little_section=v_section;
                cluster_u->little_tree.emplace_back(little_tree);

                //Complete building tree

                std::cout<<"Complete building the tree for the "<<divide_r<<"th block in the "<<k_cluster<<"th cluster"<<std::endl;

                //Complete building tree


            }
            //For dividing the description between different blocks

            std::cout<<" "<<std::endl;

            //For dividing the description between different blocks
        }

        //This is for calculate the sequence of the cluster computed

        k_cluster=k_cluster+1;

        //This is for calculate the sequence of the cluster computed

        //This is for divide the description of different blocks in a cluster

        std::cout<<" "<<std::endl;
        std::cout<<" "<<std::endl;
        std::cout<<" "<<std::endl;

        //This is for divide the description of different blocks in a cluster
    }
}

//The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees The function for constructing the clusters for building the trees


//A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data

void HDR_Tree::project_item_little(Item* i_p, DataBase* db_l, int height_in){
    iTrans.clear();
    for (int i = 0; i < height_in; i++) {
        iTrans.emplace_back(db_l->T[i] * (i_p->v));
    }
}

//A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data A kind of projection for projecting the item by the little data


//Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees.

void HDR_Tree::update_item_multipletree(){
    auto item_out=slidingWindow[0];
    slidingWindow.erase(slidingWindow.begin());

    //Rp.clear();

    //root_influenced.clear();

    //std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, beginning

    //int test_0=2;

    //projectItem(item_out);

    /*std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, ending

    int test_1=3;

    projection_time=projection_time+test_1-test_0;//std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();   //Add the projection time

    std::chrono::steady_clock::time_point begin_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, beginning*/

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_hdr_cal_clusters=0;
    //num_hdr_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //Search(root, item_out);

    /*std::cout<<"The number of influenced users "<<Rp.size()<<std::endl;

    std::cout<<"The dknn values before recomputing the knn lists of deletion"<<std::endl;

    for(auto user_before: Rp){
        std::cout<<user_before->dknn<<" ";
    }
    std::cout<<" "<<std::endl;*/

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_hdr_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_hdr_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    /*std::chrono::steady_clock::time_point end_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, ending

    search_time_HDR=search_time_HDR+std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add_hdr_0 - begin_clock_item_add_hdr_0).count();*/

    //int size_rp=Rp.size();
    double distance_icu;
    for(auto cluster_full:*clusters_user_tree){
        distance_icu=dist_cu_i(cluster_full, item_out);
        for(auto tree_structure:cluster_full->little_tree){
            if((distance_icu<=tree_structure->whole_radius+tree_structure->whole_maxdknn)&&(distance_icu>=tree_structure->whole_low_radius-tree_structure->whole_maxdknn)){
                if(tree_structure->littleroot->type==NodeType::NonLeafNode){
                    project_item_little(item_out, tree_structure->littledatabase, tree_structure->height);
                    tree_structure->littlerp.clear();
                    Search_block(tree_structure->littleroot,item_out,tree_structure);
                    if(tree_structure->littlerp.size()!=0){
                        for(auto user_partial:tree_structure->littlerp){

                        }
                        AdjustMaxdknnPaper_littletree(tree_structure->littleroot, tree_structure);
                        std::vector<float> dknnlist;
                        for(auto user_partial:tree_structure->littlerp){
                            dknnlist.emplace_back(user_partial->dknn);
                        }
                        std::sort(dknnlist.begin(),  dknnlist.end(),
                                  [&](const float& a, const float& b) -> bool {
                                      return a <=b;
                                  });
                        if(dknnlist[dknnlist.size()-1]>tree_structure->whole_maxdknn){
                            tree_structure->whole_maxdknn=dknnlist[dknnlist.size()-1];
                        }
                    }
                }else{
                    tree_structure->littlerp.clear();
                    Search_block(tree_structure->littleroot,item_out,tree_structure);
                    if(tree_structure->littlerp.size()!=0){
                        for(auto user_partial:tree_structure->littlerp){
                            user_partial->computeKNN(slidingWindow);
                        }
                        std::vector<float> dknnlist;
                        for(auto user_partial:tree_structure->littlerp){
                            dknnlist.emplace_back(user_partial->dknn);
                        }
                        std::sort(dknnlist.begin(),  dknnlist.end(),
                                  [&](const float& a, const float& b) -> bool {
                                      return a <=b;
                                  });
                        if(dknnlist[dknnlist.size()-1]>tree_structure->whole_maxdknn){
                            tree_structure->whole_maxdknn=dknnlist[dknnlist.size()-1];
                        }
                    }
                }
            }
        }
    }

    /*std::cout<<"The dknn values after recomputing the knn lists of deletion"<<std::endl;

    for(auto user_after:Rp){
        std::cout<<user_after->dknn<<" ";
    }

    std::cout<<" "<<std::endl;

    std::cout<<" "<<std::endl;*/
};

//Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees. Delete item using multiple hdr trees.

//Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees.

void HDR_Tree::update_item_add_multipletree(){
    auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));
    slidingWindow.emplace_back(new_item);
    //update_item_hdr(itemRoot_i, new_item, Mode::Add);

    //Rp.clear();

    //root_influenced.clear();

    //std::chrono::steady_clock::time_point begin_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, beginning

    //int test_0=2;

    //projectItem(item_out);

    /*std::chrono::steady_clock::time_point end_clock_item_add = std::chrono::steady_clock::now();  //For checking of the projection time, ending

    int test_1=3;

    projection_time=projection_time+test_1-test_0;//std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add - begin_clock_item_add).count();   //Add the projection time

    std::chrono::steady_clock::time_point begin_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, beginning*/

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //num_hdr_cal_clusters=0;
    //num_hdr_cal_users=0;

    //To initialize the count values for the times of calculation of the distances between the clusters and the item and of the distances between the users and the item

    //Search(root, item_out);

    /*std::cout<<"The number of influenced users "<<Rp.size()<<std::endl;

    std::cout<<"The dknn values before recomputing the knn lists of deletion"<<std::endl;

    for(auto user_before: Rp){
        std::cout<<user_before->dknn<<" ";
    }
    std::cout<<" "<<std::endl;*/

    //To print the times of calculation of the distance

    //std::cout<<"The times of calculation of the distance between the item and the clusters is "<<num_hdr_cal_clusters<<std::endl;
    //std::cout<<"The times of calculation of the distance between the item and the users is "<<num_hdr_cal_users<<std::endl;
    //std::cout<<" "<<std::endl;

    //To print the times of calculation of the distance

    /*std::chrono::steady_clock::time_point end_clock_item_add_hdr_0 = std::chrono::steady_clock::now();  //For checking of the search time on hdr tree, ending

    search_time_HDR=search_time_HDR+std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_add_hdr_0 - begin_clock_item_add_hdr_0).count();*/

    double distance_icu;
    for(auto cluster_full:*clusters_user_tree){
        distance_icu=dist_cu_i(cluster_full, new_item);
        for(auto tree_structure:cluster_full->little_tree){
            if((distance_icu<=tree_structure->whole_radius+tree_structure->whole_maxdknn)&&(distance_icu>=tree_structure->whole_low_radius-tree_structure->whole_maxdknn)){
                if(tree_structure->littleroot->type==NodeType::NonLeafNode){
                    project_item_little(new_item, tree_structure->littledatabase, tree_structure->height);
                    tree_structure->littlerp.clear();
                    Search_block(tree_structure->littleroot,new_item,tree_structure);
                    if(tree_structure->littlerp.size()!=0){
                        for(auto user_partial:tree_structure->littlerp){
                            user_partial->update(new_item,Mode::Add);
                        }
                        AdjustMaxdknnPaper_littletree(tree_structure->littleroot, tree_structure);
                        std::vector<float> dknnlist;
                        for(auto user_partial:*(tree_structure->little_section)){
                            dknnlist.emplace_back(user_partial->dknn);
                        }
                        std::sort(dknnlist.begin(),  dknnlist.end(),
                                  [&](const float& a, const float& b) -> bool {
                                      return a <=b;
                                  });
                        tree_structure->whole_maxdknn=dknnlist[dknnlist.size()-1];

                    }
                }else{
                    tree_structure->littlerp.clear();
                    Search_block(tree_structure->littleroot,new_item,tree_structure);
                    if(tree_structure->littlerp.size()!=0){
                        for(auto user_partial:tree_structure->littlerp){
                            user_partial->update(new_item, Mode::Add);
                        }
                        std::vector<float> dknnlist;
                        for(auto user_partial:*(tree_structure->little_section)){
                            dknnlist.emplace_back(user_partial->dknn);
                        }
                        std::sort(dknnlist.begin(),  dknnlist.end(),
                                  [&](const float& a, const float& b) -> bool {
                                      return a <=b;
                                  });
                        tree_structure->whole_maxdknn=dknnlist[dknnlist.size()-1];

                    }
                }
            }
        }
    }
    add_item_num = add_item_num + 1;
};

//Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees. Add item using multiple hdr trees.

//For searching on the blocks, for searching on the blocks, for searching on the blocks, for searching on the blocks

void HDR_Tree::Search_block(Node* node, Item* I, Littletree* lp_pointer){
    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {

            num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);
            if (dist(I, user) < user->dknn) {
                lp_pointer->littlerp.emplace_back(user);
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                Search_block(NLN->children[j], I, lp_pointer);
            }
        }
    }
}

//For searching on the blocks, for searching on the blocks, for searching on the blocks, for searching on the blocks



//For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees

bool HDR_Tree::AdjustMaxdknnPaper_littletree(Node* n, Littletree* lpt_pointer){
    bool result = false;

    if (n->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(n);

        long Len = lpt_pointer->littlerp.size();
        for (auto Ut : *LN) {
            auto& user = std::get<1>(Ut);
            for (long i = 0; i < Len; i++) {
                if (lpt_pointer->littlerp[i] == user) {
                    return true;
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(n);
        auto& Cp = NLN->clusters;

        for (auto Cj : Cp) {
            bool res = AdjustMaxdknnPaper_littletree(Cj->ptr, lpt_pointer);

            if (res) {
                result = true;

                float Dist = 0;

                if (Cj->ptr->type == NodeType::LeafNode) {

                    auto LN = dynamic_cast<LeafNode*>(Cj->ptr);
                    for (auto Ut : *LN) {

                        auto user = std::get<1>(Ut);
                        Dist = std::max(Dist, user->dknn);
                    }
                }
                else {

                    auto NLN = dynamic_cast<NonLeafNode*>(Cj->ptr);
                    auto Cp = NLN->clusters;

                    for (auto Cj : Cp) {
                        Dist = std::max(Dist, Cj->maxdknn);
                    }
                }

                Cj->maxdknn = Dist;
            }
        }
    }

    return result;
};


//For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees For adjusting the maxdknn of the little trees


void HDR_Tree::initialize_two_dimensional_array(){
    int i;
    int j;
    for(i=0;i<=4;i++){
        for(j=0;j<=6;j++){
           check_prune[i][j]=0;
        }
    }
};

//For calculating the distance between itemclusters and userleafnodes. For calculating the distance between itemclusters and userleafnodes. For calculating the distance between itemclusters and userleafnodes.

void HDR_Tree::calculate_distance_usercluster_itemleaf(){
    for (auto vector_user_cluster: dictionary_for_layer_usercluster){
        for(auto cluster_user: *vector_user_cluster){
            for(auto leafnode_item: itemleafnodelist){
                cluster_user->dis_itemleafnode_list.emplace_back((cluster_user->center-(data->T[cluster_user->l-1])*(leafnode_item->center)).norm());
            }
        }
    }
};

//For calculating the distance between itemclusters and userleafnodes. For calculating the distance between itemclusters and userleafnodes. For calculating the distance between itemclusters and userleafnodes.



//For calculating the distance between userclusters and itemleafnodes. For calculating the distance between userclusters and itemleafnodes. For calculating the distance between userclusters and itemleafnodes.

void HDR_Tree::calculate_distance_itemcluster_userleaf(){
    for (auto vector_item_cluster: dictionary_for_layer_itemcluster){
        for(auto cluster_item: *vector_item_cluster){
            for(auto leafnode_user: leafnodelist){
                cluster_item->dis_userleafnode_list.emplace_back((cluster_item->center-(data_i->T[cluster_item->l-1])*(leafnode_user->center)).norm());
            }
        }
    }
};

//For calculating the distance between userclusters and itemleafnodes. For calculating the distance between userclusters and itemleafnodes. For calculating the distance between userclusters and itemleafnodes.


//The function for insertion of the items and update the delta tree. The function for insertion of the items and update the delta tree. The function for insertion of the items and update the delta tree.

void HDR_Tree::update_item_collectleaf(itemNode* root_item_tree, Item* I, Mode mode){
    if(mode==Mode::Add){
        if(root_item_tree->type==itemNodeType::itemNonLeafNode){

            //std::cout<<"Get itemnonleafnode"<<std::endl;

            auto root_i=dynamic_cast<itemNonLeafNode*>(root_item_tree);
            itemCluster* catch_c;
            std::vector<std::tuple<float, itemCluster*>> V_d_c;
            auto v_i_transformed=data_i->T[root_i->l-1]*I->v;
            for(auto C:root_i->clusters){
                float distance_i_c=(v_i_transformed-C->center).norm();
                V_d_c.emplace_back(std::make_tuple(distance_i_c, C));
            }
            std::nth_element(V_d_c.begin(), V_d_c.begin() + V_d_c.size() - 1, V_d_c.end(),
                             [&](const std::tuple<float, itemCluster*>& a, const std::tuple<float, itemCluster*>& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            catch_c=std::get<1>(V_d_c[V_d_c.size()-1]);
            catch_c->number=catch_c->number+1;
            catch_c->Items.emplace_back(std::make_tuple(std::get<0>(V_d_c[V_d_c.size()-1]), I));
            std::nth_element(catch_c->Items.begin(), catch_c->Items.begin() + catch_c->Items.size() - 1, catch_c->Items.end(),
                             [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                 return std::get<0>(a) <= std::get<0>(b);
                             });
            catch_c->radius=std::get<0>(catch_c->Items[catch_c->Items.size()-1]);
            update_item_collectleaf(catch_c->ptr,  I, mode);
        }else{

            //std::cout<<"Get leafnode"<<std::endl;


            auto leaf_i=dynamic_cast<itemLeafNode*>(root_item_tree);
            if(leaf_i->size_available== false){
                list_of_noleaf_updateitem.emplace_back(I);
            }else{
                if(leaf_i->if_item_update== false){
                    leaf_i->list_updated_item.emplace_back(I);
                    float distance_iupdate_center=(I->v-leaf_i->center).norm();
                    leaf_i->radius_updated_items=std::max(distance_iupdate_center, leaf_i->radius_updated_items);
                    leaf_i->if_item_update=true;
                    updated_itemleafnode_list.emplace_back(leaf_i);
                }else{
                    leaf_i->list_updated_item.emplace_back(I);
                    float distance_iupdate_center=(I->v-leaf_i->center).norm();
                    leaf_i->radius_updated_items=std::max(distance_iupdate_center, leaf_i->radius_updated_items);
                }
            }
        }
    }else{
        if(root_item_tree->type==itemNodeType::itemNonLeafNode){
            auto root_i_reduce=dynamic_cast<itemNonLeafNode*>(root_item_tree);
            itemCluster* catch_c_reduce;
            int catch_no_i;
            for(auto C_c_r: root_i_reduce->clusters){
                if(C_c_r->Items.size()!=0){
                    bool indicator_c_if= false;
                    for(int i=0; i<C_c_r->Items.size();i++){
                        if(std::get<1>(C_c_r->Items[i])==I){
                            //std::cout<<"Catch successfully"<<std::endl;
                            catch_no_i=i;
                            catch_c_reduce=C_c_r;
                            indicator_c_if= true;
                            break;
                        }
                    }
                    if(indicator_c_if== true){
                        break;
                    }
                }
            }
            catch_c_reduce->Items.erase(catch_c_reduce->Items.begin()+catch_no_i);
            catch_c_reduce->number=catch_c_reduce->number-1;
            if(catch_c_reduce->number==0){
                catch_c_reduce->radius=0;
            }else{
                std::nth_element(catch_c_reduce->Items.begin(), catch_c_reduce->Items.begin() + catch_c_reduce->Items.size() - 1, catch_c_reduce->Items.end(),
                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                     return std::get<0>(a) <= std::get<0>(b);
                                 });
                catch_c_reduce->radius=std::get<0>(catch_c_reduce->Items[catch_c_reduce->Items.size()-1]);
            }
            update_item_collectleaf(catch_c_reduce->ptr, I, mode);
        }
        else{
            auto leaf_i=dynamic_cast<itemLeafNode*>(root_item_tree);
            if(leaf_i->size_available== false){
                list_of_noleaf_updateitem.emplace_back(I);
            }else{
                if(leaf_i->if_item_update== false){
                    leaf_i->list_updated_item.emplace_back(I);
                    float distance_iupdate_center=(I->v-leaf_i->center).norm();
                    leaf_i->radius_updated_items=std::max(distance_iupdate_center, leaf_i->radius_updated_items);
                    leaf_i->if_item_update=true;
                    updated_itemleafnode_list.emplace_back(leaf_i);
                }else{
                    leaf_i->list_updated_item.emplace_back(I);
                    float distance_iupdate_center=(I->v-leaf_i->center).norm();
                    leaf_i->radius_updated_items=std::max(distance_iupdate_center, leaf_i->radius_updated_items);
                }
            }
        }
    }
};




//The function for insertion of the items and update the delta tree. The function for insertion of the items and update the delta tree. The function for insertion of the items and update the delta tree.



//The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree.

void HDR_Tree::update_user_collectleaf(User* U, Mode m, Node* node){
    if(m==Mode::Add){

        if(node->type==NodeType::NonLeafNode){
            auto NLN=dynamic_cast<NonLeafNode*>(node);

            //std::cout<<"Calculating the "<<NLN->l<<" level node"<<std::endl;

            std::vector<std::tuple<float, Cluster*>>V;
            auto v_transformed=data->T[NLN->l-1]*U->center;
            for(auto C_0: NLN->clusters){
                V.emplace_back(std::make_tuple((v_transformed-C_0->center).norm(), C_0));
            }

            //std::cout<<"Complete computing the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, Cluster*>& a, const std::tuple<float, Cluster* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });

            //std::cout<<"Complete sorting the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

            auto C_add=std::get<1>(V[V.size()-1]);

            //std::cout<<"Catch the cluster that the new user should be added to"<<std::endl;

            C_add->Users.emplace_back(std::make_tuple(std::get<0>(V[V.size()-1]), U));

            //std::cout<<"Complete adding the user into the proper node"<<std::endl;

            C_add->number=C_add->number+1;

            //std::cout<<"The current number of users "<<C_add->number<<" in the catched cluster after adding the new user"<<std::endl;

            std::sort(C_add->Users.begin(),  C_add->Users.end(),
                      [&](const std::tuple<float, User*>& a, const std::tuple<float, User*>& b) -> bool {
                          return std::get<0>(a) <= std::get<0>(b);
                      });
            C_add->radius=std::get<0>(C_add->Users[(C_add->Users).size()-1]);
            update_user_collectleaf( U, m, C_add->ptr);
        }else{
            auto leaf_u=dynamic_cast<LeafNode*>(node);
            if(leaf_u->size_available== false){
                list_of_noleaf_updateuser.emplace_back(U);
            }else{
                if(leaf_u->if_update_user== false){
                    leaf_u->if_update_user=true;
                    update_userleafnode_list.emplace_back(leaf_u);
                    leaf_u->list_updated_user.emplace_back(U);
                    float distance_uupdate_center=(U->center-leaf_u->center).norm();
                    leaf_u->radius_updated_users=std::max(distance_uupdate_center, leaf_u->radius_updated_users);

                }else{
                    leaf_u->list_updated_user.emplace_back(U);
                    float distance_uupdate_center=(U->center-leaf_u->center).norm();
                    leaf_u->radius_updated_users=std::max(distance_uupdate_center, leaf_u->radius_updated_users);
                }
            }
        }

    }else{

    }
};

//The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree. The function for updating of the users and updating the HDR tree.


//The function for searching the influenced users by the updated item leafnodes and updating the KNN of the influenced users and updating the HDR Tree.

void HDR_Tree::search_and_updateadd_by_itemleafnode(Node* node, itemLeafNode* itemleafnode){

    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);

            /*if((user->u_low_d_i[itemleafnode->cluster->l-1]-itemleafnode->cluster->center).norm()-itemleafnode->cluster->radius<user->dknn){
                for(auto item_update:itemleafnode->list_updated_item){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(item_update, user) < user->dknn) {
                            user->update(item_update, Mode::Add);
                        }
                    }
                }
            }*/
            for(auto item_update:itemleafnode->list_updated_item){
                //if((user->center_low-item_update->v_low).norm()<user->dknn){

                    distance_time=distance_time+1;

                    if (dist(item_update, user) < user->dknn) {
                        user->update(item_update, Mode::Add);
                    }
                //}
            }
        }

    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;
        for (long j = 0; j < Cp.size(); j++) {
            if (Cp[j]->dis_itemleafnode_list[itemleafnode->sequence_ileafnode]-itemleafnode->radius_updated_items-Cp[j]->radius < Cp[j]->maxdknn) {
                search_and_updateadd_by_itemleafnode(NLN->children[j], itemleafnode);
            }
        }
    }

};

//The function for searching the influenced users by the updated item leafnodes and updating the KNN of the influenced users and updating the HDR Tree.


//The function for add items for batch. The function for add items for batch. The function for add items for batch.

void HDR_Tree::add_item_batch(long n){
    int i;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));

        //new_item->v_low=data->Mat_low_d*new_item->v;

        slidingWindow.emplace_back(new_item);
        //update_item_hdr(itemRoot_i, new_item, Mode::Add);

        update_item_collectleaf(itemRoot_i, new_item, Mode::Add);

        add_item_num = add_item_num + 1;
    }

    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/



    for(auto item_update_noleaf:list_of_noleaf_updateitem){
        Rp.clear();
        projectItem(item_update_noleaf);
        search_using_middle_dim(root, item_update_noleaf);
        if (Rp.size() > 0) {
            for (auto u:Rp) {
                u->update(item_update_noleaf, Mode::Add);
            }
        }
        AdjustMaxdknnPaper(root);
    }

    for(auto leafnode_update:updated_itemleafnode_list){
        search_and_updateadd_by_itemleafnode(root, leafnode_update);
        AdjustMaxdknnPaper(root);
        leafnode_update->if_item_update=false;
        leafnode_update->radius_updated_items=0;
        leafnode_update->list_updated_item.clear();
    }

    updated_itemleafnode_list.clear();
    list_of_noleaf_updateitem.clear();

};

//The function for add items for batch. The function for add items for batch. The function for add items for batch.


//The function for knn calculating of the users surrounding the leafnode; The function for knn calculating of the users surrounding the leafnode;

void HDR_Tree::leafnodecomputeknn(LeafNode* hdrtreeleafnode, itemNode* node){

    //project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca


    for(auto u:hdrtreeleafnode->list_updated_user){
        u->R.clear();
        u->knn_temporary.clear();
        u->dknn=std::numeric_limits<float>::max();
    }

    hdrtreeleafnode->maxdknncell_update=std::numeric_limits<float>::max();


    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    std::vector<std::tuple<float, itemNonLeafNode*>> V;

    int i; //for get knn from knn_temporary to R
    std::vector<float> dknn_list;  //For storing the dknn values to get the maxdknn

    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    long item_add;
    float distance_item_user;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>=hdrtreeleafnode->radius_updated_users+hdrtreeleafnode->maxdknncell_update){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation


// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(C->dis_userleafnode_list[hdrtreeleafnode->sequence]-C->radius<hdrtreeleafnode->radius_updated_users+hdrtreeleafnode->maxdknncell_update){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){
                        for(auto user_leafnode:hdrtreeleafnode->list_updated_user){

                            item_add=0;

                            if((user_leafnode->u_low_d_i[C->l-1]-C->center).norm()-C->radius<user_leafnode->dknn){
                                for(auto I: C->Items){
                                    //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                    //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);
                                    if((user_leafnode->center_low-(std::get<1>(I))->v_low).norm()<user_leafnode->dknn){

                                        distance_time=distance_time+1;

                                        distance_item_user=(user_leafnode->center-(std::get<1>(I))->v).norm();{
                                            if(distance_item_user<user_leafnode->dknn){
                                                user_leafnode->knn_temporary.emplace_back(std::make_tuple(dist(std::get<1>(I), user_leafnode), std::get<1>(I)));
                                                item_add=item_add+1;
                                            }
                                        }

                                    }
                                }
                            }
                            if((user_leafnode->knn_temporary.size()>=k)&&(item_add>0)){
                                std::nth_element(user_leafnode->knn_temporary.begin(), user_leafnode->knn_temporary.begin() + k - 1, user_leafnode->knn_temporary.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                user_leafnode->knn_temporary.assign(user_leafnode->knn_temporary.begin(), user_leafnode->knn_temporary.begin()+k);
                                user_leafnode->dknn=std::get<0>(user_leafnode->knn_temporary[k-1]);
                            }
                        }
                        dknn_list.clear();
                        for(auto user_in_leafnode: hdrtreeleafnode->list_updated_user){
                            dknn_list.emplace_back(user_in_leafnode->dknn);
                        }
                        hdrtreeleafnode->maxdknncell_update=*max_element(dknn_list.begin(), dknn_list.end());

                    }else{
                        V.emplace_back(std::make_tuple(C->dis_userleafnode_list[hdrtreeleafnode->sequence]-C->radius, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto user: hdrtreeleafnode->list_updated_user){
        for(i=0;i<k;i++){
            user->R.emplace_back(std::get<1>(user->knn_temporary[i]));
        }
    }
};

//The function for knn calculating of the users surrounding the leafnode; The function for knn calculating of the users surrounding the leafnode;


//For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted.

void HDR_Tree::leafnode_influenced_computeknn(LeafNode* hdrtreeleafnode, itemNode* node){

    //project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca


    for(auto u:hdrtreeleafnode->list_influenced_user){
        u->R.clear();
        u->knn_temporary.clear();
        u->dknn=std::numeric_limits<float>::max();
    }

    hdrtreeleafnode->maxdknncell=std::numeric_limits<float>::max();
    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    std::vector<std::tuple<float, itemNonLeafNode*>> V;

    int i; //for get knn from knn_temporary to R
    std::vector<float> dknn_list;  //For storing the dknn values to get the maxdknn

    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;
    float distance_user_item;
    long item_add;



    //std::cout<<"Successfully go through the beginning stage"<<std::endl;


    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();

            //std::cout<<"We now pop the root of the delta tree"<<std::endl;

        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();

            //std::cout<<"We now pop out the closest non leaf delta node to the influenced hdr leafnode from the v vector"<<std::endl;

        }
        if((std::get<0>(pop_node))>=hdrtreeleafnode->radius_influenced_users+hdrtreeleafnode->maxdknncell){
            break;

            //std::cout<<"The closest non leaf delta node to the influenced hdr leafnode from the v vector can be pruned, so the loop of pop and push can be broken"<<std::endl;

        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation


// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(C->dis_userleafnode_list[hdrtreeleafnode->sequence]-C->radius<hdrtreeleafnode->radius_influenced_users+hdrtreeleafnode->maxdknncell){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){

                        //std::cout<<"A leaf delta node is got, so the process of analysis of the items in it and the influenced users is began"<<std::endl;

                        for(auto user_leafnode:hdrtreeleafnode->list_influenced_user){

                            item_add=0;

                            if((user_leafnode->u_low_d_i[C->l-1]-C->center).norm()-C->radius<user_leafnode->dknn){



                                for(auto I: C->Items){
                                    //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                    //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);

                                    if((user_leafnode->center_low-std::get<1>(I)->v_low).norm()<user_leafnode->dknn){
                                        distance_user_item=dist(std::get<1>(I), user_leafnode);

                                        distance_time=distance_time+1;

                                        if(distance_user_item<user_leafnode->dknn){
                                            item_add=item_add+1;
                                            user_leafnode->knn_temporary.emplace_back(std::make_tuple(dist(std::get<1>(I), user_leafnode), std::get<1>(I)));
                                        }
                                    }
                                }
                            }

                            if((user_leafnode->knn_temporary.size()>=k)&&(item_add>0)){
                                std::nth_element(user_leafnode->knn_temporary.begin(), user_leafnode->knn_temporary.begin() + k - 1, user_leafnode->knn_temporary.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                user_leafnode->knn_temporary.assign(user_leafnode->knn_temporary.begin(), user_leafnode->knn_temporary.begin()+k);
                                user_leafnode->dknn=std::get<0>(user_leafnode->knn_temporary[k-1]);
                            }

                        }

                        //std::cout<<"The process of nested analysis of the influenced users and the items in the delta leafnode has been completed"<<std::endl;

                        dknn_list.clear();
                        for(auto user_in_leafnode: hdrtreeleafnode->list_influenced_user){
                            dknn_list.emplace_back(user_in_leafnode->dknn);
                        }

                        //std::cout<<"Middle"<<std::endl;

                        /*for(auto knn:dknn_list){
                            std::cout<<knn<<" ";
                        }*/
                        //std::cout<<dknn_list.size()<<std::endl;

                        /*std::nth_element(dknn_list.begin(), dknn_list.begin() + dknn_list.size() - 1, dknn_list.end(),
                                         [&](const float& a, const float& b) -> bool {
                                             return a <= b;
                                         });*/

                        hdrtreeleafnode->maxdknncell=*max_element(dknn_list.begin(),dknn_list.end());

                        //std::cout<<"Middle 2"<<std::endl;




                        //std::cout<<"The process of maxdknncell calculation has been completed"<<std::endl;


                    }else{
                        V.emplace_back(std::make_tuple(C->dis_userleafnode_list[hdrtreeleafnode->sequence]-C->radius, dynamic_cast<itemNonLeafNode*>(C->ptr)));

                        //std::cout<<"A nonleaf delta node is got, so it is pushed into the vector"<<std::endl;

                    }
                }
            }
        }
    }
    for(auto user: hdrtreeleafnode->list_influenced_user){
        for(i=0;i<k;i++){
            user->R.emplace_back(std::get<1>(user->knn_temporary[i]));
        }
    }

    //std::cout<<"The knn lists of the users has been made"<<std::endl;

};

//For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted. For compute knn by hdr leaf node when items are deleted.








//The function for deleting items for batch. The function for deleting items for batch. The function for deleting items for batch.

void HDR_Tree::delete_item_batch(long n){
    int i;
    for(i=0; i<n; i++){
        auto i_out=slidingWindow[0];
        slidingWindow.erase(slidingWindow.begin());

        update_item_collectleaf(itemRoot_i, i_out, Mode::Remove);


    }

    //std::cout<<"Among the updated items on the delta tree, the number of updated items that have no non-empty itemleafnode to contain is "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"The number of the updated itemleafnode is "<<updated_itemleafnode_list.size()<<std::endl;

    for(auto item_update_noleaf:list_of_noleaf_updateitem) {

        search_influenced_singleuser_leafnodes_single_delete_item(root, item_update_noleaf);

        for (auto user_noleaf:influenced_user_noleaf) {
            userComputeKNN(itemRoot_i, user_noleaf);
        }

        for (auto user_leafnode: influenced_leafnode) {



            leafnode_influenced_computeknn(user_leafnode, itemRoot_i);


        }

        AdjustMaxdknnPaper(root);
        influenced_user_noleaf.clear();
        for (auto leafnode: influenced_leafnode) {
            leafnode->if_influenced_user = false;
            leafnode->list_influenced_user.clear();
            leafnode->radius_influenced_users = 0;
        }
        influenced_leafnode.clear();
    }



    //int p=0;



    for(auto update_itemleafnode:updated_itemleafnode_list){
        search_influenced_singleuser_leafnodes_delete_itemleafnode(root, update_itemleafnode);



        //std::cout<<"This is the number of the affected single users by the "<<p+1<<"th updated item leafnode: "<<influenced_user_noleaf.size()<<std::endl;
        //std::cout<<"This is the number of the affected hdr leafnodes by the "<<p+1<<"th updated item leafnode: "<<influenced_leafnode.size()<<std::endl;

        //p=p+1;

        for(auto user_noleaf:influenced_user_noleaf){
            userComputeKNN(itemRoot_i, user_noleaf);
        }



        //std::cout<<"check_1"<<std::endl;
        for(auto user_leafnode: influenced_leafnode){
            leafnode_influenced_computeknn(user_leafnode, itemRoot_i);
        }
        //std::cout<<"check_2"<<std::endl;


        AdjustMaxdknnPaper(root);


        influenced_user_noleaf.clear();
        for(auto leafnode: influenced_leafnode){
            leafnode->if_influenced_user=false;
            leafnode->list_influenced_user.clear();
            leafnode->radius_influenced_users=0;
        }
        influenced_leafnode.clear();

        update_itemleafnode->if_item_update=false;
        update_itemleafnode->list_updated_item.clear();
        update_itemleafnode->radius_updated_items=0;

    }

    updated_itemleafnode_list.clear();
    list_of_noleaf_updateitem.clear();

};


//The function for deleting items for batch. The function for deleting items for batch. The function for deleting items for batch.

//For searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. For searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

void HDR_Tree::search_influenced_singleuser_leafnodes_single_delete_item(Node* node, Item* single_deleted_item){
    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {

            //num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);
            if((user->center_low-single_deleted_item->v_low).norm()<user->dknn){

                distance_time=distance_time+1;

                if (dist(single_deleted_item, user) < user->dknn){
                    if(LN->size_available==false){
                        influenced_user_noleaf.emplace_back(user);
                    }else{
                        if(LN->if_influenced_user==false){
                            LN->list_influenced_user.emplace_back(user);
                            float distance_user_leafcenter=(user->center-LN->center).norm();
                            LN->radius_influenced_users=std::max(LN->radius_influenced_users, distance_user_leafcenter);
                            LN->if_influenced_user=true;
                            influenced_leafnode.emplace_back(LN);
                        }else{
                            LN->list_influenced_user.emplace_back(user);
                            float distance_user_leafcenter=(user->center-LN->center).norm();
                            LN->radius_influenced_users=std::max(LN->radius_influenced_users, distance_user_leafcenter);
                        }
                    }
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {


            d1 = dist(single_deleted_item, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                search_influenced_singleuser_leafnodes_single_delete_item(NLN->children[j], single_deleted_item);
            }
        }
    }
};

//For searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. For searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.


//When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.

void HDR_Tree::search_influenced_singleuser_leafnodes_delete_itemleafnode(Node* node, itemLeafNode* include_deleted_itemleafnode){
    if (node->type == NodeType::LeafNode) {

        //std::cout<<"Get leafnode"<<std::endl;


        auto LN = dynamic_cast<LeafNode*>(node);

        //std::cout<<LN->if_influenced_user<<std::endl;

        for (auto Ut : *LN) {

            //num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);

            if((user->u_low_d_i[LN->cluster->l-1]-LN->cluster->center).norm()-LN->cluster->radius<user->dknn){
                for(auto deleted_item:include_deleted_itemleafnode->list_updated_item){
                    if((user->center_low-deleted_item->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(deleted_item, user) < user->dknn) {
                            if(LN->size_available==false){
                                influenced_user_noleaf.emplace_back(user);
                                //std::cout<<"The user has no leafnode contain it"<<std::endl;
                            }else{
                                if(LN->if_influenced_user==false){
                                    LN->list_influenced_user.emplace_back(user);
                                    float distance_user_leafcenter=(user->center-LN->center).norm();
                                    LN->radius_influenced_users=std::max(LN->radius_influenced_users, distance_user_leafcenter);
                                    LN->if_influenced_user=true;
                                    influenced_leafnode.emplace_back(LN);
                                    //std::cout<<"The influenced_leafnode is not empty"<<std::endl;
                                }else{

                                    //std::cout<<"The leafnode has been opened"<<std::endl;

                                    LN->list_influenced_user.emplace_back(user);
                                    float distance_user_leafcenter=(user->center-LN->center).norm();
                                    LN->radius_influenced_users=std::max(LN->radius_influenced_users, distance_user_leafcenter);
                                }
                            }
                            //std::cout<<"Catch"<<std::endl;
                            break;
                        }
                    }
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {


            d1 = Cp[j]->dis_itemleafnode_list[include_deleted_itemleafnode->sequence_ileafnode]-Cp[j]->radius-include_deleted_itemleafnode->radius_updated_items;
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                search_influenced_singleuser_leafnodes_delete_itemleafnode(NLN->children[j], include_deleted_itemleafnode);
            }
        }
    }
};

//When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them. When deleted item in leafnode, for searching for the influenced leafnodes and influenced users without no-empty leafnodes including them.


//RKNN search applying the middle dimensions between the full dimensions and the highest dimensions on the tree structure.

void HDR_Tree::search_using_middle_dim(Node* node, Item* I){
    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {

            //num_hdr_cal_users=num_hdr_cal_users+1;   //Add the calculation time of the users

            auto user = std::get<1>(Ut);
            if((user->center_low-I->v_low).norm()<user->dknn){

                distance_time=distance_time+1;

                if (dist(I, user) < user->dknn) {
                    Rp.emplace_back(user);
                }
            }
        }
    }
    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;


        float d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            //num_hdr_cal_clusters=num_hdr_cal_clusters+1;   //Add the calculation time of the clusters

            d1 = dist(I, Cp[j]);
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                search_using_middle_dim(NLN->children[j], I);
            }
        }
    }
};

//RKNN search applying the middle dimensions between the full dimensions and the highest dimensions on the tree structure.


//Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree.

void HDR_Tree::add_user_batch(long n){



    long i;

    for(i=0; i<n; i++){

        auto new_user= new User(data->U.row(numUsers+add_user_num), k);

        Users.emplace_back(new_user);

        add_user_num=add_user_num+1;

        new_user->center_low=data->Mat_low_d*new_user->center;

        projectUser_item(new_user);

        for(auto u_lowd_item:Trans_u_i){
            new_user->u_low_d_i.emplace_back(u_lowd_item);
        }

        update_user_collectleaf(new_user, Mode::Add, root);
    }

    std::cout<<list_of_noleaf_updateuser.size()<<" "<<update_userleafnode_list.size()<<std::endl;

    for(auto user_update_noleaf: list_of_noleaf_updateuser){
        userComputeKNN(itemRoot_i, user_update_noleaf);
    }

    for(auto updated_hdr_leafnode:update_userleafnode_list){
        leafnodecomputeknn(updated_hdr_leafnode, itemRoot_i);
        updated_hdr_leafnode->if_update_user=false;
        updated_hdr_leafnode->radius_updated_users=0;
        updated_hdr_leafnode->list_updated_user.clear();
    }

    AdjustMaxdknnPaper(root);
    list_of_noleaf_updateuser.clear();
    update_userleafnode_list.clear();


};

//Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree. Add users and calculate their KNN lists and adjust the HDR Tree.



//The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN;

void HDR_Tree::form_clusters_user(long dim, long f ){
    Eigen::MatrixXf Mat_low_d;
    Mat_low_d.resize(dim, numFeatures);
    for (int i = 0; i < dim; i++) {
        Mat_low_d.row(i) = std::get<1>(data->PrincipalComponents[i]);
    }
    auto Clusters_user=new Clusters(Users, f,  Mat_low_d);

    int sequence=0;

    for(auto cluster_u: Clusters_user->children){
        cluster_u->sequence=sequence;
        V_lowd_U_C.emplace_back(cluster_u);
        sequence=sequence+1;
    }


    /*std::cout<<"Now show the information of the user clusters"<<std::endl;
    for(auto usercluster: V_lowd_U_C){
        std::cout<<"The No."<<usercluster->sequence<<" usercluster has "<<usercluster->Users.size()<<" users."<<std::endl;
    }*/




};

void HDR_Tree::form_clusters_item(long dim, long f ){
    Eigen::MatrixXf Mat_low_d;
    Mat_low_d.resize(dim, numFeatures);
    for (int i = 0; i < dim; i++) {
        Mat_low_d.row(i) = std::get<1>(data->PrincipalComponents[i]);
    }
    auto Clusters_item=new itemClusters(slidingWindow, f,  Mat_low_d);

    int sequence=0;

    for(auto cluster_i: Clusters_item->children){
        cluster_i->sequence=sequence;
        V_lowd_I_C.emplace_back(cluster_i);
        sequence=sequence+1;
    }
};

void HDR_Tree::calculate_distance_utree_icluster(){

    Eigen::VectorXf center;
    int i;

    for (auto vector_user_cluster: dictionary_for_layer_usercluster){

        center.resize(data->T[(*vector_user_cluster)[0]->l-1].rows());

        for(auto cluster_user: *vector_user_cluster){

            for(auto cluster_item: V_lowd_I_C){
                for(i=0; i<data->T[cluster_user->l-1].rows(); i++){
                    center[i]=cluster_item->center[i];
                }
                auto distance=(center-cluster_user->center).norm();
                cluster_user->dis_itemcluster.emplace_back(distance);
            }
        }
    }
};

void HDR_Tree::calculate_distance_itree_ucluster(){
    Eigen::VectorXf center;
    int i;
    int rows;


    for (auto cluster_user: V_lowd_U_C){
        for (auto vector_item_cluster: dictionary_for_layer_itemcluster){

            rows=data_i->T[(*vector_item_cluster)[0]->l-1].rows();

            center.resize(rows);

            for(i=0; i<rows; i++) {
                center[i] = cluster_user->center[i];
            }

            for(auto cluster_item: *vector_item_cluster){

                auto distance=(center-cluster_item->center).norm();
                cluster_item->dis_usercluster.emplace_back(distance);

            }
        }
    }

    /*std::cout<<"Now show the distance from the user clusters to the item clusters on the delta tree"<<std::endl;

    int j=1;
    for(auto layer: dictionary_for_layer_itemcluster){
        for(auto itemcluster: *layer){
            std::cout<<"The distance from the user clusters to the No."<<itemcluster->i_cluster_sequense<<" item clusters on the "<<j<<"th layer of the delta tree are ";
            for(auto distance: itemcluster->dis_usercluster){
                std::cout<<distance<<" ";
            }
            std::cout<<" "<<std::endl;
        }

        j=j+1;
    }*/

};





void HDR_Tree::itemclusters_false(){
    for(auto itemcluster: V_lowd_I_C){
        itemcluster->influenced=false;
    }
}

void HDR_Tree::itemcluster_radius_zero(){
    for(auto itemcluster: V_lowd_I_C){
        itemcluster->low_d_radius=0;
    }
};

void HDR_Tree::itemcluster_updated_item_clear(){
    for(auto itemcluster: V_lowd_I_C){
        itemcluster->updated_items.clear();
    }
};

void HDR_Tree::clear_updated_itemclusters(){
    influenced_item_clusters.clear();
};


void HDR_Tree::userclusters_false(){
    for(auto usercluster: V_lowd_U_C){
        usercluster->have_user=false;
    }
};

void HDR_Tree::usercluster_radius_zero(){
    for(auto usercluster: V_lowd_U_C){
        usercluster->radius_user_need_knn=0;
    }
};

void HDR_Tree::usercluster_updated_user_clear(){
    for(auto usercluster: V_lowd_U_C){
        usercluster->user_need_knnlist.clear();
    }
};


void HDR_Tree::clear_clusters_haveuserknn(){
    clusters_haveuserknn.clear();
};


void HDR_Tree::distribute_updated_item(std::vector<Item*>* updated_item){

    for(auto item:*updated_item){
        std::vector<std::tuple<float, itemCluster*>>V;
        for(auto C_item: V_lowd_I_C){
            V.emplace_back(std::make_tuple((item->v_low -C_item->center).norm(), C_item));
        }

        //std::cout<<"Complete computing the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

        std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                         [&](const std::tuple<float, itemCluster*>& a, const std::tuple<float, itemCluster* >& b) -> bool {
                             return std::get<0>(a) >= std::get<0>(b);
                         });

        //std::cout<<"Complete sorting the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

        auto C_add=std::get<1>(V[V.size()-1]);
        auto distance_item_itemcluster=std::get<0>(V[V.size()-1]);
        if(C_add->influenced==false){
            C_add->influenced=true;
            C_add->updated_items.emplace_back(item);
            if(distance_item_itemcluster>C_add->low_d_radius){
                C_add->low_d_radius=distance_item_itemcluster;
            }
            influenced_item_clusters.emplace_back(C_add);
        }
        else{
            C_add->updated_items.emplace_back(item);
            if(distance_item_itemcluster>C_add->low_d_radius){
                C_add->low_d_radius=distance_item_itemcluster;
            }
        }
    }
}

void HDR_Tree::rknn_search_update_additem_prune(Node* node, itemCluster* include_itemcluster){




    if (node->type == NodeType::LeafNode) {

        //std::chrono::steady_clock::time_point begin_clock_leafnode = std::chrono::steady_clock::now();

        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);

            /*if((user->u_low_d_i[itemleafnode->cluster->l-1]-itemleafnode->cluster->center).norm()-itemleafnode->cluster->radius<user->dknn){
                for(auto item_update:itemleafnode->list_updated_item){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(item_update, user) < user->dknn) {
                            user->update(item_update, Mode::Add);
                        }
                    }
                }
            }*/





            if((user->center_low-include_itemcluster->center).norm()-include_itemcluster->low_d_radius<user->dknn) {
                /*for (auto item_update:include_itemcluster->updated_items) {
                    //if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time = distance_time + 1;




                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        float distance_user_item =(user->center-item_update->v).norm();

                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance - begin_clock_calculatedistance).count();
                        //time_distance_calculation = time_distance_calculation + diff_3;


                        if (distance_user_item < user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            user->update(item_update, Mode::Add);


                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem = time_update_user_knn_additem + diff_2;

                            item_add_update = item_add_update + 1;

                        }
                        //}
                    //}
                }*/


                for(auto item_update:include_itemcluster->updated_items){


                    if((user->center_low-item_update->v_low).norm()<user->dknn){
                        distance_time=distance_time+1;

                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        float distance_user_item=distance_check(user, item_update);


                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance -begin_clock_calculatedistance).count();
                        //time_distance_calculation=time_distance_calculation+diff_3;


                        if ( distance_user_item< user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            user->update(item_update, Mode::Add);


                            //Rp.emplace_back(user);
                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem=time_update_user_knn_additem+diff_2;

                            //item_add_update=item_add_update+1;

                        }
                    }
                }

                /*float p1=float((update_in_num*1.0)/(meet_num*1.0));
                p1_additem.emplace_back(p1);
                if(meet_num-update_in_num!=0){
                    float p2=float(((two_times-update_in_num)*1.0)/((meet_num-update_in_num)*1.0));
                    p2_additem.emplace_back(p2);
                }*/

            }
        }

        /*std::chrono::steady_clock::time_point end_clock_leafnode = std::chrono::steady_clock::now();
        float diff_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_leafnode - begin_clock_leafnode).count();
        time_recorder=time_recorder+diff_1;*/



    }
    else {

        //std::chrono::steady_clock::time_point begin_clock_noleafnode = std::chrono::steady_clock::now();

        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;
        for (long j = 0; j < Cp.size(); j++) {

            //meet_clusters_rknn=meet_clusters_rknn+1;

            if (Cp[j]->dis_itemcluster[include_itemcluster->sequence] -include_itemcluster->low_d_radius-Cp[j]->radius < Cp[j]->maxdknn) {

                //notprune_clusters_rknn=notprune_clusters_rknn+1;

                rknn_search_update_additem_prune(NLN->children[j], include_itemcluster);
            }

            /*else{
                prune_clusters_rknn=prune_clusters_rknn+1;
            }*/
        }

        //std::chrono::steady_clock::time_point end_clock_noleafnode = std::chrono::steady_clock::now();

    }





};

void HDR_Tree::rknn_search_update_additem(Node* node, itemCluster* include_itemcluster){






    if (node->type == NodeType::LeafNode) {

        std::chrono::steady_clock::time_point begin_clock_leafnode = std::chrono::steady_clock::now();

        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);

            /*if((user->u_low_d_i[itemleafnode->cluster->l-1]-itemleafnode->cluster->center).norm()-itemleafnode->cluster->radius<user->dknn){
                for(auto item_update:itemleafnode->list_updated_item){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(item_update, user) < user->dknn) {
                            user->update(item_update, Mode::Add);
                        }
                    }
                }
            }*/





            if((user->center_low-include_itemcluster->center).norm()-include_itemcluster->low_d_radius<user->dknn) {
                /*for (auto item_update:include_itemcluster->updated_items) {
                    //if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time = distance_time + 1;




                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        float distance_user_item =(user->center-item_update->v).norm();

                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance - begin_clock_calculatedistance).count();
                        //time_distance_calculation = time_distance_calculation + diff_3;


                        if (distance_user_item < user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            user->update(item_update, Mode::Add);


                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem = time_update_user_knn_additem + diff_2;

                            item_add_update = item_add_update + 1;

                        }
                        //}
                    //}
                }*/


                for(auto item_update:include_itemcluster->updated_items){


                    //if((user->center_low-item_update->v_low).norm()<user->dknn){
                        distance_time=distance_time+1;

                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        std::chrono::steady_clock::time_point time_start_cal = std::chrono::steady_clock::now();

                        float distance_user_item=distance_check(user, item_update);

                        std::chrono::steady_clock::time_point time_start_end = std::chrono::steady_clock::now();

                        full_distance_time_batch_add_items=full_distance_time_batch_add_items+std::chrono::duration_cast<std::chrono::nanoseconds>(time_start_end -time_start_cal).count();


                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance -begin_clock_calculatedistance).count();
                        //time_distance_calculation=time_distance_calculation+diff_3;


                        if ( distance_user_item< user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            user->update(item_update, Mode::Add);


                            //Rp.emplace_back(user);
                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem=time_update_user_knn_additem+diff_2;

                            //item_add_update=item_add_update+1;

                        }
                    //}
                }

                /*float p1=float((update_in_num*1.0)/(meet_num*1.0));
                p1_additem.emplace_back(p1);
                if(meet_num-update_in_num!=0){
                    float p2=float(((two_times-update_in_num)*1.0)/((meet_num-update_in_num)*1.0));
                    p2_additem.emplace_back(p2);
                }*/

            }
        }

        std::chrono::steady_clock::time_point end_clock_leafnode = std::chrono::steady_clock::now();
        time_leafnode_batch_add_items = time_leafnode_batch_add_items + std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_leafnode - begin_clock_leafnode).count();



    }
    else {

        //std::chrono::steady_clock::time_point begin_clock_noleafnode = std::chrono::steady_clock::now();

        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;
        for (long j = 0; j < Cp.size(); j++) {

            //meet_clusters_rknn=meet_clusters_rknn+1;

            if (Cp[j]->dis_itemcluster[include_itemcluster->sequence] -include_itemcluster->low_d_radius-Cp[j]->radius < Cp[j]->maxdknn) {

                //notprune_clusters_rknn=notprune_clusters_rknn+1;

                rknn_search_update_additem(NLN->children[j], include_itemcluster);
            }

            /*else{
                prune_clusters_rknn=prune_clusters_rknn+1;
            }*/
        }

        //std::chrono::steady_clock::time_point end_clock_noleafnode = std::chrono::steady_clock::now();

    }

};

void HDR_Tree::rknn_search_update_additem_section(int o, int n){
    for (long i = o; i < influenced_item_clusters.size(); i = i + n) {
        //-------------------------------------------------------------------------------//
        /*std::cout<<"check the root of the item hdr tree"<<std::endl;
        std::cout<<itemRoot_ptr<<std::endl;
        std::cout<<"check success"<<std::endl;*/
        //-------------------------------------------------------------------------------//
        //computeknnuser_noroot(Users[i]);   //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
        rknn_search_update_additem(root, influenced_item_clusters[i]);     //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
        influenced_item_clusters[i]->influenced=false;
        influenced_item_clusters[i]->low_d_radius=0;
        influenced_item_clusters[i]->updated_items.clear();
        //user_computeknn_id(Users[i], 0.01,0.01, false);
    }
};

void HDR_Tree::rknn_search_update_additem_paralell(){

    int n = std::thread::hardware_concurrency();

    std::cout << "Using " << n << " paralell threads to do rknn search based on clusters" << std::endl;

    std::vector<std::thread> threads;
    //begin = std::chrono::steady_clock::now();

    for (long i = 0; i < n; i++) {
        threads.push_back(std::thread(
                &HDR_Tree::rknn_search_update_additem_section, this, i, n
        ));
    }

    for (auto& th : threads) {
        th.join();
    }
};




void HDR_Tree::add_item_batch_bycluster_prune(long n){


    int i;
    std::vector<Item*> add_item_list;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));

        add_item_num = add_item_num + 1;

        new_item->v_low=data->Mat_low_d*new_item->v;

        slidingWindow.emplace_back(new_item);
        //update_item_hdr(itemRoot_i, new_item, Mode::Add);

        add_item_list.emplace_back(new_item);

        update_item_hdr(itemRoot_i, new_item, Mode::Add);


    }



    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();





    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/
    //distribute_updated_item(&add_item_list);

    /*for(auto itemcluster:influenced_item_clusters){
        std::cout<<"This cluster has "<<itemcluster->updated_items.size()<<" update items in it, and the low_d radius is "<<itemcluster->low_d_radius<<", and its siwtch is "<<itemcluster->influenced<<std::endl;

    }*/


    //std::cout<<"check 1"<<std::endl;




    for(auto itemcluster:influenced_item_clusters){

        /*meet_clusters_rknn=0;
        prune_clusters_rknn=0;
        notprune_clusters_rknn=0;

        time_recorder=0;
        distance_time=0;
        item_add_update=0;
        time_distance_calculation=0;
        time_update_user_knn_additem=0;*/





        //Rp.clear();
        //std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();

        rknn_search_update_additem_prune(root, itemcluster);
        //std::cout<<" "<<std::endl;

        //std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();

        //float diff_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
        //diff_1 = diff_1 / 1000.0f;
        //printf("Time for update the cluster of items is %f \n", diff_1);


        //std::cout<<"The number of meet clusters is "<<meet_clusters_rknn<<", and the number of pruned clusters is "<<prune_clusters_rknn<<", and the number of clusters that are not been pruned is "<<notprune_clusters_rknn<<"."<<std::endl;

        //std::cout<<"The time for calculation on the leafnodes is "<<time_recorder/1000000000.0<<std::endl;

        //std::cout<<"The times of calculating the full dimensional distance is "<<distance_time<<std::endl;

        //std::cout<<"The times of updating new items into users is "<<item_add_update<<std::endl;

        //std::cout<<"The time for calculation of distance is "<<time_distance_calculation/1000000000.0<<std::endl;

        //std::cout<<"The time for update the added items into the users is "<<time_update_user_knn_additem/1000000000.0<<std::endl;

        //std::cout<<"The average time for distance calculation is "<<time_distance_calculation/(distance_time*1.0)<<std::endl;

        //std::set<User*> userknn_set (Rp.begin(), Rp.end());
        //Rp.clear();
        //Rp.assign(userknn_set.begin(), userknn_set.end());

        AdjustMaxdknnPrev(root);
        itemcluster->influenced=false;
        itemcluster->low_d_radius=0;
        itemcluster->updated_items.clear();
        //Rp.clear();
    }


    //std::cout<<"check 2"<<std::endl;

    influenced_item_clusters.clear();


    /*if(p2_additem.size()!=0){
        float p2=0;
        for(auto p:p2_additem){
          p2=p2+p;
        }
        p2=float(p2/(p2_additem.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"All the items are not been updated have been pruned by low dimensions"<<std::endl;
    }*/

};


void HDR_Tree::add_item_batch_bycluster(long n){


    int i;
    std::vector<Item*> add_item_list;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));

        add_item_num = add_item_num + 1;

        new_item->v_low=data->Mat_low_d*new_item->v;

        slidingWindow.emplace_back(new_item);
        //update_item_hdr(itemRoot_i, new_item, Mode::Add);

        add_item_list.emplace_back(new_item);

        update_item_hdr(itemRoot_i, new_item, Mode::Add);


    }



    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();





    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/
    distribute_updated_item(&add_item_list);

    /*for(auto itemcluster:influenced_item_clusters){
        std::cout<<"This cluster has "<<itemcluster->updated_items.size()<<" update items in it, and the low_d radius is "<<itemcluster->low_d_radius<<", and its siwtch is "<<itemcluster->influenced<<std::endl;

    }*/


    //std::cout<<"check 1"<<std::endl;




    for(auto itemcluster:influenced_item_clusters){

        /*meet_clusters_rknn=0;
        prune_clusters_rknn=0;
        notprune_clusters_rknn=0;

        time_recorder=0;
        distance_time=0;
        item_add_update=0;
        time_distance_calculation=0;
        time_update_user_knn_additem=0;*/





        //Rp.clear();
        //std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();

        std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

        rknn_search_update_additem(root, itemcluster);

        std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();

        time_statistic=time_statistic+std::chrono::duration_cast<std::chrono::microseconds>( time_end- time_start).count();

        //std::cout<<" "<<std::endl;

        //std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();

        //float diff_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
        //diff_1 = diff_1 / 1000.0f;
        //printf("Time for update the cluster of items is %f \n", diff_1);


        //std::cout<<"The number of meet clusters is "<<meet_clusters_rknn<<", and the number of pruned clusters is "<<prune_clusters_rknn<<", and the number of clusters that are not been pruned is "<<notprune_clusters_rknn<<"."<<std::endl;

        //std::cout<<"The time for calculation on the leafnodes is "<<time_recorder/1000000000.0<<std::endl;

        //std::cout<<"The times of calculating the full dimensional distance is "<<distance_time<<std::endl;

        //std::cout<<"The times of updating new items into users is "<<item_add_update<<std::endl;

        //std::cout<<"The time for calculation of distance is "<<time_distance_calculation/1000000000.0<<std::endl;

        //std::cout<<"The time for update the added items into the users is "<<time_update_user_knn_additem/1000000000.0<<std::endl;

        //std::cout<<"The average time for distance calculation is "<<time_distance_calculation/(distance_time*1.0)<<std::endl;

        //std::set<User*> userknn_set (Rp.begin(), Rp.end());
        //Rp.clear();
        //Rp.assign(userknn_set.begin(), userknn_set.end());

        AdjustMaxdknnPrev(root);
        itemcluster->influenced=false;
        itemcluster->low_d_radius=0;
        itemcluster->updated_items.clear();
        //Rp.clear();
    }


    //std::cout<<"check 2"<<std::endl;

    influenced_item_clusters.clear();


    /*if(p2_additem.size()!=0){
        float p2=0;
        for(auto p:p2_additem){
          p2=p2+p;
        }
        p2=float(p2/(p2_additem.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"All the items are not been updated have been pruned by low dimensions"<<std::endl;
    }*/


};


void HDR_Tree::add_item_batch_bycluster_paralell(long n){


    int i;
    std::vector<Item*> add_item_list;
    for(i=0; i<n; i++){
        auto new_item = new Item(data_i->I.row(numItems_i + add_item_num));

        add_item_num = add_item_num + 1;

        new_item->v_low=data->Mat_low_d*new_item->v;

        slidingWindow.emplace_back(new_item);
        //update_item_hdr(itemRoot_i, new_item, Mode::Add);

        add_item_list.emplace_back(new_item);

        update_item_hdr(itemRoot_i, new_item, Mode::Add);


    }



    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();
    users_affected_deleteditems.clear();





    //std::cout<<"This is the number of items that be added into the empty itemleafnode "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"This is the number of items that be added into each itemleafnode "<<std::endl;
    /*for(auto itemleafnode:updated_itemleafnode_list){
        std::cout<<itemleafnode->sequence_ileafnode<<" "<<itemleafnode->list_updated_item.size()<<std::endl;
    }*/
    distribute_updated_item(&add_item_list);

    /*for(auto itemcluster:influenced_item_clusters){
        std::cout<<"This cluster has "<<itemcluster->updated_items.size()<<" update items in it, and the low_d radius is "<<itemcluster->low_d_radius<<", and its siwtch is "<<itemcluster->influenced<<std::endl;

    }*/


    //std::cout<<"check 1"<<std::endl;




    //rknn_search_update_additem_paralell();



    for(auto itemcluster:influenced_item_clusters){







        //Rp.clear();
        //std::chrono::steady_clock::time_point begin_clock_item_remove = std::chrono::steady_clock::now();

        std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();

        rknn_search_update_additem(root, itemcluster);

        std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();

        time_statistic=time_statistic+std::chrono::duration_cast<std::chrono::microseconds>( time_end- time_start).count();

        //std::cout<<" "<<std::endl;

        //std::chrono::steady_clock::time_point end_clock_item_remove = std::chrono::steady_clock::now();

        //float diff_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock_item_remove - begin_clock_item_remove).count();
        //diff_1 = diff_1 / 1000.0f;
        //printf("Time for update the cluster of items is %f \n", diff_1);


        //std::cout<<"The number of meet clusters is "<<meet_clusters_rknn<<", and the number of pruned clusters is "<<prune_clusters_rknn<<", and the number of clusters that are not been pruned is "<<notprune_clusters_rknn<<"."<<std::endl;

        //std::cout<<"The time for calculation on the leafnodes is "<<time_recorder/1000000000.0<<std::endl;

        //std::cout<<"The times of calculating the full dimensional distance is "<<distance_time<<std::endl;

        //std::cout<<"The times of updating new items into users is "<<item_add_update<<std::endl;

        //std::cout<<"The time for calculation of distance is "<<time_distance_calculation/1000000000.0<<std::endl;

        //std::cout<<"The time for update the added items into the users is "<<time_update_user_knn_additem/1000000000.0<<std::endl;

        //std::cout<<"The average time for distance calculation is "<<time_distance_calculation/(distance_time*1.0)<<std::endl;

        //std::set<User*> userknn_set (Rp.begin(), Rp.end());
        //Rp.clear();
        //Rp.assign(userknn_set.begin(), userknn_set.end());

        AdjustMaxdknnPrev(root);
        itemcluster->influenced=false;
        itemcluster->low_d_radius=0;
        itemcluster->updated_items.clear();
        //Rp.clear();
    }


    //std::cout<<"check 2"<<std::endl;

    influenced_item_clusters.clear();


    /*if(p2_additem.size()!=0){
        float p2=0;
        for(auto p:p2_additem){
          p2=p2+p;
        }
        p2=float(p2/(p2_additem.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"All the items are not been updated have been pruned by low dimensions"<<std::endl;
    }*/



    //std::cout<<"check 2"<<std::endl;

    /*if(p2_additem.size()!=0){
        float p2=0;
        for(auto p:p2_additem){
          p2=p2+p;
        }
        p2=float(p2/(p2_additem.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"All the items are not been updated have been pruned by low dimensions"<<std::endl;
    }*/


};

void HDR_Tree::distribute_user_needknn(std::vector<User*>* user_needknn){

    for(auto user:*user_needknn){
        std::vector<std::tuple<float, Cluster*>>V;
        for(auto C_user: V_lowd_U_C){
            V.emplace_back(std::make_tuple((user->center_low -C_user->center).norm(), C_user));
        }

        //std::cout<<"Complete computing the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

        std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                         [&](const std::tuple<float, Cluster*>& a, const std::tuple<float, Cluster* >& b) -> bool {
                             return std::get<0>(a) >= std::get<0>(b);
                         });

        //std::cout<<"Complete sorting the distances between the newly inserted user and the centers of the clusters in the node"<<std::endl;

        auto C_add=std::get<1>(V[V.size()-1]);
        auto distance_user_usercluster=std::get<0>(V[V.size()-1]);
        if(C_add->have_user==false){
            C_add->have_user=true;
            C_add->user_need_knnlist.emplace_back(user);
            if(distance_user_usercluster>C_add->radius_user_need_knn ){
                C_add->radius_user_need_knn=distance_user_usercluster;
            }
            clusters_haveuserknn.emplace_back(C_add);
        }
        else{
            C_add->user_need_knnlist.emplace_back(user);
            if(distance_user_usercluster>C_add->radius_user_need_knn){
                C_add->radius_user_need_knn=distance_user_usercluster;
            }
        }
    }
};

void HDR_Tree::knn_by_clusters(Cluster* influenced_cluster, itemNode* node){
    //project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca

    //std::chrono::steady_clock::time_point begin_clock;
    //std::chrono::steady_clock::time_point end_clock;





    for(auto u:influenced_cluster->user_need_knnlist){
        u->R.clear();
        u->knn_temporary.clear();
        u->dknn=std::numeric_limits<float>::max();
    }

    influenced_cluster->max_dknncell=std::numeric_limits<float>::max();


    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    std::vector<std::tuple<float, itemNonLeafNode*>> V;

    int i; //for get knn from knn_temporary to R
    std::vector<float> dknn_list;  //For storing the dknn values to get the maxdknn

    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    long item_add;
    float distance_item_user;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>=influenced_cluster->radius_user_need_knn+influenced_cluster->max_dknncell){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation


// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(C->dis_usercluster[influenced_cluster->sequence]-C->radius<influenced_cluster->radius_user_need_knn +influenced_cluster->max_dknncell){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){
                        for(auto user:influenced_cluster->user_need_knnlist){

                            if((user->u_low_d_i[C->l-1]-C->center).norm()-C->radius<user->dknn){

                                item_add=0;


                                for(auto I: C->Items){





                                    //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                    //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);
                                    //if((user->center_low-(std::get<1>(I))->v_low).norm()<user->dknn){


                                    //item_access_count_batch[std::get<1>(I)]++;

                                    //distance_time=distance_time+1;

                                    auto item_cache=(std::get<1>(I));

                                    //begin_clock=std::chrono::steady_clock::now();

                                    //VTUNE_START_BATCH();
                                    //distance_item_user=(user->center-item_chache->v).norm();
                                    distance_item_user=distance_test_chache(user, item_cache);
                                    //VTUNE_END_BATCH();

                                    //end_clock=std::chrono::steady_clock::now();

                                    //time_full_dimensional_distance_add_batch_knn=time_full_dimensional_distance_add_batch_knn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();


                                    if(distance_item_user<user->dknn){
                                        user->knn_temporary.emplace_back(std::make_tuple(distance_item_user, std::get<1>(I)));
                                        item_add=item_add+1;


                                    }

                                    //}
                                }

                                if((user->knn_temporary.size()>=k)&&(item_add>0)){
                                    std::nth_element(user->knn_temporary.begin(), user->knn_temporary.begin() + k - 1, user->knn_temporary.end(),
                                                     [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                         return std::get<0>(a) <= std::get<0>(b);
                                                     });
                                    user->knn_temporary.assign(user->knn_temporary.begin(), user->knn_temporary.begin()+k);
                                    user->dknn=std::get<0>(user->knn_temporary[k-1]);
                                }

                                /*float p1=float((update_num*1.0)/(meet_num*1.0));
                                p1_adduser.emplace_back(p1);
                                if(meet_num-update_num!=0){
                                    float p2=float(((twice_num-update_num)*1.0)/((meet_num-update_num)*1.0));
                                    p2_adduser.emplace_back(p2);
                                }*/
                            }

                        }
                        dknn_list.clear();
                        for(auto user_in_cluster: influenced_cluster->user_need_knnlist){
                            dknn_list.emplace_back(user_in_cluster->dknn);
                        }
                        influenced_cluster->max_dknncell=*max_element(dknn_list.begin(), dknn_list.end());

                    }else{
                        V.emplace_back(std::make_tuple(C->dis_usercluster[influenced_cluster->sequence]-C->radius, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto user: influenced_cluster->user_need_knnlist){
        for(i=0;i<k;i++){
            user->R.emplace_back(std::get<1>(user->knn_temporary[i]));
        }
    }
}

void HDR_Tree::knn_by_clusters_prune(Cluster* influenced_cluster, itemNode* node){
    //project_u_iclust_pca_init(user);     //generate the vector for user including every low dimension form for the user using item pca


    for(auto u:influenced_cluster->user_need_knnlist){
        u->R.clear();
        u->knn_temporary.clear();
        u->dknn=std::numeric_limits<float>::max();
    }

    influenced_cluster->max_dknncell=std::numeric_limits<float>::max();


    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(node);
    std::vector<std::tuple<float, itemNonLeafNode*>> V;

    int i; //for get knn from knn_temporary to R
    std::vector<float> dknn_list;  //For storing the dknn values to get the maxdknn

    float ini_dist=1;
    V.emplace_back(std::make_tuple(ini_dist, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    long item_add;
    float distance_item_user;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>=influenced_cluster->radius_user_need_knn+influenced_cluster->max_dknncell){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                //float dist_r=dist_u_ic_str(user,C)-(C->radius);
// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation


// an optional from of distance calculation an optional from of distance calculation an optional from of distance calculation
                if(C->dis_usercluster[influenced_cluster->sequence]-C->radius<influenced_cluster->radius_user_need_knn +influenced_cluster->max_dknncell){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){
                        for(auto user:influenced_cluster->user_need_knnlist){

                            if((user->u_low_d_i[C->l-1]-C->center).norm()-C->radius<user->dknn){

                                item_add=0;


                                for(auto I: C->Items){





                                    //float distance=dist(std::get<1>(I), user); Item* ite=std::get<1>(I);
                                    //std::tuple<float, Item*>tuple_d_i =std::make_tuple(distance, ite);
                                    if((user->center_low-(std::get<1>(I))->v_low).norm()<user->dknn){

                                        distance_time=distance_time+1;


                                        distance_item_user=(user->center-(std::get<1>(I))->v).norm();



                                        if(distance_item_user<user->dknn){
                                            user->knn_temporary.emplace_back(std::make_tuple(distance_item_user, std::get<1>(I)));
                                            item_add=item_add+1;


                                        }

                                    }
                                }

                                if((user->knn_temporary.size()>=k)&&(item_add>0)){
                                    std::nth_element(user->knn_temporary.begin(), user->knn_temporary.begin() + k - 1, user->knn_temporary.end(),
                                                     [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                         return std::get<0>(a) <= std::get<0>(b);
                                                     });
                                    user->knn_temporary.assign(user->knn_temporary.begin(), user->knn_temporary.begin()+k);
                                    user->dknn=std::get<0>(user->knn_temporary[k-1]);
                                }

                                /*float p1=float((update_num*1.0)/(meet_num*1.0));
                                p1_adduser.emplace_back(p1);
                                if(meet_num-update_num!=0){
                                    float p2=float(((twice_num-update_num)*1.0)/((meet_num-update_num)*1.0));
                                    p2_adduser.emplace_back(p2);
                                }*/
                            }

                        }
                        dknn_list.clear();
                        for(auto user_in_cluster: influenced_cluster->user_need_knnlist){
                            dknn_list.emplace_back(user_in_cluster->dknn);
                        }
                        influenced_cluster->max_dknncell=*max_element(dknn_list.begin(), dknn_list.end());

                    }else{
                        V.emplace_back(std::make_tuple(C->dis_usercluster[influenced_cluster->sequence]-C->radius, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
    for(auto user: influenced_cluster->user_need_knnlist){
        for(i=0;i<k;i++){
            user->R.emplace_back(std::get<1>(user->knn_temporary[i]));
        }
    }
};

void HDR_Tree::update_user_add_batch_bycluster_prune(long n){

    long i;

    std::vector<User*> user_knn;





    for(i=0; i<n; i++){

        auto new_user= new User(data->U.row(numUsers+add_user_num), k);

        Users.emplace_back(new_user);

        add_user_num=add_user_num+1;

        new_user->center_low=data->Mat_low_d*new_user->center;

        projectUser_item(new_user);

        for(auto u_lowd_item:Trans_u_i){
            new_user->u_low_d_i.emplace_back(u_lowd_item);
        }

        user_knn.emplace_back(new_user);
    }

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    distribute_user_needknn(&user_knn);



    for(auto user_cluster: clusters_haveuserknn){
        knn_by_clusters_prune(user_cluster, itemRoot_i);
        user_cluster->user_need_knnlist.clear();
        user_cluster->have_user=false;
        user_cluster->radius_user_need_knn=0;
    }

    clear_clusters_haveuserknn();

    for(auto user: user_knn){
        update_user_hdr(user, Mode::Add, root);
    }


    /*if(p2_adduser.size()!=0){
        float p2=0;
        for(auto p:p2_adduser){
            p2=p2+p;
        }
        p2=float(p2/(p2_adduser.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"For an arbitrary user, all the met items are updated into its knn list, or all the items that are not updated into its knn list are pruned by one time calculation."<<std::endl;
    }*/

    //std::cout<<"Totally meet "<<num_total_meet<<" itmes, and within which, "<<num_cal_twice<<" items need two times of calculation"<<std::endl;


};




// parallel variant: the per-cluster collective searches are independent and
// are distributed over a pool of hardware threads
void HDR_Tree::update_user_add_batch_bycluster_paralell(long n){

    long i;

    std::vector<User*> user_knn;

    for(i=0; i<n; i++){

        auto new_user= new User(data->U.row(numUsers+add_user_num), k);

        Users.emplace_back(new_user);

        add_user_num=add_user_num+1;

        new_user->center_low=data->Mat_low_d*new_user->center;

        projectUser_item(new_user);

        for(auto u_lowd_item:Trans_u_i){
            new_user->u_low_d_i.emplace_back(u_lowd_item);
        }

        user_knn.emplace_back(new_user);
    }

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();

    distribute_user_needknn(&user_knn);

    {
        unsigned nt = std::thread::hardware_concurrency();
        if (nt == 0) nt = 4;
        std::atomic<size_t> next(0);
        std::vector<std::thread> pool;
        for (unsigned t = 0; t < nt; ++t) {
            pool.emplace_back([&]() {
                size_t idx;
                while ((idx = next.fetch_add(1)) < clusters_haveuserknn.size()) {
                    auto user_cluster = clusters_haveuserknn[idx];
                    knn_by_clusters(user_cluster, itemRoot_i);
                    user_cluster->user_need_knnlist.clear();
                    user_cluster->have_user=false;
                    user_cluster->radius_user_need_knn=0;
                }
            });
        }
        for (auto& th : pool) th.join();
    }

    clear_clusters_haveuserknn();

    for(auto user: user_knn){
        update_user_hdr(user, Mode::Add, root);
    }
};


void HDR_Tree::update_user_add_batch_bycluster(long n){

    //std::chrono::steady_clock::time_point begin_clock;
    //std::chrono::steady_clock::time_point end_clock;


    //begin_clock=std::chrono::steady_clock::now();

    long i;

    std::vector<User*> user_knn;





    for(i=0; i<n; i++){

        auto new_user= new User(data->U.row(numUsers+add_user_num), k);

        Users.emplace_back(new_user);

        add_user_num=add_user_num+1;

        new_user->center_low=data->Mat_low_d*new_user->center;

        projectUser_item(new_user);

        for(auto u_lowd_item:Trans_u_i){
            new_user->u_low_d_i.emplace_back(u_lowd_item);
        }

        user_knn.emplace_back(new_user);
    }

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    distribute_user_needknn(&user_knn);

    //std::cout<<"check_1"<<std::endl;

    //end_clock=std::chrono::steady_clock::now();

    //time_update_user_add_batch_except_knn=time_update_user_add_batch_except_knn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();


    //begin_clock=std::chrono::steady_clock::now();

    for(auto user_cluster: clusters_haveuserknn){
        knn_by_clusters(user_cluster, itemRoot_i);
        user_cluster->user_need_knnlist.clear();
        user_cluster->have_user=false;
        user_cluster->radius_user_need_knn=0;

        //std::cout<<"check"<<std::endl;

    }

    //end_clock=std::chrono::steady_clock::now();

    //time_update_user_add_batch_knn=time_update_user_add_batch_knn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

    //begin_clock=std::chrono::steady_clock::now();

    clear_clusters_haveuserknn();




    for(auto user: user_knn){
        update_user_hdr(user, Mode::Add, root);
    }

    //end_clock=std::chrono::steady_clock::now();

    //time_update_user_add_batch_except_knn=time_update_user_add_batch_except_knn+std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - begin_clock).count();

    /*if(p2_adduser.size()!=0){
        float p2=0;
        for(auto p:p2_adduser){
            p2=p2+p;
        }
        p2=float(p2/(p2_adduser.size()*1.0));
        std::cout<<p2<<" ";
    }
    else{
        std::cout<<"For an arbitrary user, all the met items are updated into its knn list, or all the items that are not updated into its knn list are pruned by one time calculation."<<std::endl;
    }*/

    //std::cout<<"Totally meet "<<num_total_meet<<" itmes, and within which, "<<num_cal_twice<<" items need two times of calculation"<<std::endl;


};


void HDR_Tree::rknn_delete_item(Node* node, itemCluster* include_deleted_itemcluster){

    float distance_user_item;


    if (node->type == NodeType::LeafNode) {

        //std::chrono::steady_clock::time_point begin_clock_leafnode = std::chrono::steady_clock::now();

        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);

            /*if((user->u_low_d_i[itemleafnode->cluster->l-1]-itemleafnode->cluster->center).norm()-itemleafnode->cluster->radius<user->dknn){
                for(auto item_update:itemleafnode->list_updated_item){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(item_update, user) < user->dknn) {
                            user->update(item_update, Mode::Add);
                        }
                    }
                }
            }*/
            if((user->center_low-include_deleted_itemcluster->center).norm()-include_deleted_itemcluster->low_d_radius<user->dknn){
                for(auto item_update:include_deleted_itemcluster->updated_items){
                    //if((user->center_low-item_update->v_low).norm()<user->dknn){
                        distance_time=distance_time+1;

                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        distance_user_item=distance_check(user, item_update);

                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance -begin_clock_calculatedistance).count();
                        //time_distance_calculation=time_distance_calculation+diff_3;


                        if ( distance_user_item<= user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            users_affected_deleteditems.emplace_back(user);
                            break;

                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem=time_update_user_knn_additem+diff_2;

                            //item_add_update=item_add_update+1;

                        }

                    //}
                }
            }


        }

        //std::chrono::steady_clock::time_point end_clock_leafnode = std::chrono::steady_clock::now();
        //float diff_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_leafnode - begin_clock_leafnode).count();
        //time_recorder=time_recorder+diff_1;



    }
    else {

        //std::chrono::steady_clock::time_point begin_clock_noleafnode = std::chrono::steady_clock::now();

        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;
        for (long j = 0; j < Cp.size(); j++) {

            //meet_clusters_rknn=meet_clusters_rknn+1;

            if (Cp[j]->dis_itemcluster[include_deleted_itemcluster->sequence] -include_deleted_itemcluster->low_d_radius-Cp[j]->radius < Cp[j]->maxdknn) {

                //notprune_clusters_rknn=notprune_clusters_rknn+1;

                rknn_delete_item(NLN->children[j], include_deleted_itemcluster);
            }

            /*else{
                prune_clusters_rknn=prune_clusters_rknn+1;
            }*/
        }

        //std::chrono::steady_clock::time_point end_clock_noleafnode = std::chrono::steady_clock::now();

    }

};

void HDR_Tree::rknn_delete_item_prune(Node* node, itemCluster* include_deleted_itemcluster){

    float distance_user_item;


    if (node->type == NodeType::LeafNode) {

        //std::chrono::steady_clock::time_point begin_clock_leafnode = std::chrono::steady_clock::now();

        auto LN = dynamic_cast<LeafNode*>(node);
        for (auto Ut : *LN) {
            auto user = std::get<1>(Ut);

            /*if((user->u_low_d_i[itemleafnode->cluster->l-1]-itemleafnode->cluster->center).norm()-itemleafnode->cluster->radius<user->dknn){
                for(auto item_update:itemleafnode->list_updated_item){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){

                        distance_time=distance_time+1;

                        if (dist(item_update, user) < user->dknn) {
                            user->update(item_update, Mode::Add);
                        }
                    }
                }
            }*/
            if((user->center_low-include_deleted_itemcluster->center).norm()-include_deleted_itemcluster->low_d_radius<user->dknn){
                for(auto item_update:include_deleted_itemcluster->updated_items){
                    if((user->center_low-item_update->v_low).norm()<user->dknn){
                        distance_time=distance_time+1;

                        //std::chrono::steady_clock::time_point begin_clock_calculatedistance = std::chrono::steady_clock::now();

                        distance_user_item=distance_check(user, item_update);

                        //std::chrono::steady_clock::time_point end_clock_calculatedistance = std::chrono::steady_clock::now();
                        //float diff_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_calculatedistance -begin_clock_calculatedistance).count();
                        //time_distance_calculation=time_distance_calculation+diff_3;


                        if ( distance_user_item<= user->dknn) {

                            //std::chrono::steady_clock::time_point begin_clock_update = std::chrono::steady_clock::now();

                            users_affected_deleteditems.emplace_back(user);
                            break;

                            //std::chrono::steady_clock::time_point end_clock_update = std::chrono::steady_clock::now();
                            //float diff_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_update - begin_clock_update).count();
                            //time_update_user_knn_additem=time_update_user_knn_additem+diff_2;

                            //item_add_update=item_add_update+1;

                        }
                    }
                }
            }


        }

        //std::chrono::steady_clock::time_point end_clock_leafnode = std::chrono::steady_clock::now();
        //float diff_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock_leafnode - begin_clock_leafnode).count();
        //time_recorder=time_recorder+diff_1;



    }
    else {

        //std::chrono::steady_clock::time_point begin_clock_noleafnode = std::chrono::steady_clock::now();

        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;
        for (long j = 0; j < Cp.size(); j++) {

            //meet_clusters_rknn=meet_clusters_rknn+1;

            if (Cp[j]->dis_itemcluster[include_deleted_itemcluster->sequence] -include_deleted_itemcluster->low_d_radius-Cp[j]->radius < Cp[j]->maxdknn) {

                //notprune_clusters_rknn=notprune_clusters_rknn+1;

                rknn_delete_item_prune(NLN->children[j], include_deleted_itemcluster);
            }

            /*else{
                prune_clusters_rknn=prune_clusters_rknn+1;
            }*/
        }

        //std::chrono::steady_clock::time_point end_clock_noleafnode = std::chrono::steady_clock::now();

    }

};




void HDR_Tree::delete_item_batch_bycluster(long n){

    std::vector<Item*> deleted_items;

    int i;
    for(i=0; i<n; i++){
        auto i_out=slidingWindow[0];
        slidingWindow.erase(slidingWindow.begin());
        update_item_hdr(itemRoot_i, i_out, Mode::Remove);
        deleted_items.emplace_back(i_out);
    }

    //std::cout<<"The size of slidingwindow now is "<<slidingWindow.size()<<std::endl;

    /*for(auto user: Users){
        user->computeKNN(slidingWindow);
    }*/



    /*for(auto item: deleted_items){
        Rp.clear();

        projectItem(item);

        Search(root, item);

        //check time for others; check time for others; check time for others; check time for others; check time for others;
        //check_time_end=std::chrono::steady_clock::now();
        //time_other=time_other+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
        //check time for others; check time for others; check time for others; check time for others; check time for others;


        if(Rp.size()>0){

            //std::cout<<Rp.size()<<std::endl;

            for(auto u:Rp){

                //check time for others; check time for others; check time for others; check time for others; check time for others;
                //check_time_start=std::chrono::steady_clock::now();
                //check time for others; check time for others; check time for others; check time for others; check time for others;

                userComputeKNN(itemRoot_i, u);

                //check time for others; check time for others; check time for others; check time for others; check time for others;
                //check_time_end=std::chrono::steady_clock::now();
                //time_compute_knn_cross=time_compute_knn_cross+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                //check time for others; check time for others; check time for others; check time for others; check time for others;

            }

            AdjustMaxdknnPaper(root);
        }
    }*/


    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();




    distribute_updated_item(&deleted_items);
        //std::cout<<"Check"<<std::endl;


    users_affected_deleteditems.clear();
    for(auto itemcluster: influenced_item_clusters){
        rknn_delete_item(root, itemcluster);

    }

    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();

    //std::cout<<"Totally have "<<users_affected_deleteditems.size()<<" users that are regarded as influenced"<<std::endl;

    std::set<User*> userknn_set (users_affected_deleteditems.begin(), users_affected_deleteditems.end());
    //std::cout<<"Totally have "<<userknn_set.size()<<" users that are regarded as influenced after repeated users removed"<<std::endl;

    users_affected_deleteditems.clear();
    users_affected_deleteditems.assign(userknn_set.begin(), userknn_set.end());

    //std::cout<<"Totally have "<<users_affected_deleteditems.size()<<" users that are regarded as influenced after repeated users removed"<<std::endl;
    //std::cout<<"Check"<<std::endl;


    /*for(auto user: userknn_set){
        userComputeKNN(itemRoot_i, user);
    }

    AdjustMaxdknnPaper(root);*/

    /*itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();*/

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    distribute_user_needknn(&users_affected_deleteditems);
    users_affected_deleteditems.clear();








    for(auto usercluster:clusters_haveuserknn){

        //std::cout<<"The cluster has "<<usercluster->user_need_knnlist.size()<<" users"<<std::endl;
        knn_by_clusters(usercluster, itemRoot_i);


    }

    AdjustMaxdknnPrev(root);

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    //std::cout<<"Among the updated items on the delta tree, the number of updated items that have no non-empty itemleafnode to contain is "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"The number of the updated itemleafnode is "<<updated_itemleafnode_list.size()<<std::endl;

};

void HDR_Tree::delete_item_batch_bycluster_prune(long n){

    std::vector<Item*> deleted_items;

    int i;
    for(i=0; i<n; i++){
        auto i_out=slidingWindow[0];
        slidingWindow.erase(slidingWindow.begin());
        update_item_hdr(itemRoot_i, i_out, Mode::Remove);
        deleted_items.emplace_back(i_out);
    }

    //std::cout<<"The size of slidingwindow now is "<<slidingWindow.size()<<std::endl;

    /*for(auto user: Users){
        user->computeKNN(slidingWindow);
    }*/



    /*for(auto item: deleted_items){
        Rp.clear();

        projectItem(item);

        Search(root, item);

        //check time for others; check time for others; check time for others; check time for others; check time for others;
        //check_time_end=std::chrono::steady_clock::now();
        //time_other=time_other+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
        //check time for others; check time for others; check time for others; check time for others; check time for others;


        if(Rp.size()>0){

            //std::cout<<Rp.size()<<std::endl;

            for(auto u:Rp){

                //check time for others; check time for others; check time for others; check time for others; check time for others;
                //check_time_start=std::chrono::steady_clock::now();
                //check time for others; check time for others; check time for others; check time for others; check time for others;

                userComputeKNN(itemRoot_i, u);

                //check time for others; check time for others; check time for others; check time for others; check time for others;
                //check_time_end=std::chrono::steady_clock::now();
                //time_compute_knn_cross=time_compute_knn_cross+std::chrono::duration_cast<std::chrono::nanoseconds>(check_time_end -check_time_start).count();
                //check time for others; check time for others; check time for others; check time for others; check time for others;

            }

            AdjustMaxdknnPaper(root);
        }
    }*/


    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();




    distribute_updated_item(&deleted_items);
    //std::cout<<"Check"<<std::endl;


    users_affected_deleteditems.clear();
    for(auto itemcluster: influenced_item_clusters){
        rknn_delete_item_prune(root, itemcluster);

    }

    itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();

    //std::cout<<"Totally have "<<users_affected_deleteditems.size()<<" users that are regarded as influenced"<<std::endl;

    std::set<User*> userknn_set (users_affected_deleteditems.begin(), users_affected_deleteditems.end());
    //std::cout<<"Totally have "<<userknn_set.size()<<" users that are regarded as influenced after repeated users removed"<<std::endl;

    users_affected_deleteditems.clear();
    users_affected_deleteditems.assign(userknn_set.begin(), userknn_set.end());

    //std::cout<<"Totally have "<<users_affected_deleteditems.size()<<" users that are regarded as influenced after repeated users removed"<<std::endl;
    //std::cout<<"Check"<<std::endl;


    /*for(auto user: userknn_set){
        userComputeKNN(itemRoot_i, user);
    }

    AdjustMaxdknnPaper(root);*/

    /*itemclusters_false();
    itemcluster_radius_zero();
    itemcluster_updated_item_clear();
    clear_updated_itemclusters();*/

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    distribute_user_needknn(&users_affected_deleteditems);
    users_affected_deleteditems.clear();








    for(auto usercluster:clusters_haveuserknn){

        //std::cout<<"The cluster has "<<usercluster->user_need_knnlist.size()<<" users"<<std::endl;
        knn_by_clusters_prune(usercluster, itemRoot_i);


    }

    AdjustMaxdknnPrev(root);

    userclusters_false();
    usercluster_radius_zero();
    usercluster_updated_user_clear();
    clear_clusters_haveuserknn();


    //std::cout<<"Among the updated items on the delta tree, the number of updated items that have no non-empty itemleafnode to contain is "<<list_of_noleaf_updateitem.size()<<std::endl;
    //std::cout<<"The number of the updated itemleafnode is "<<updated_itemleafnode_list.size()<<std::endl;

};





//The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN; The functions for clustered based KNN and RKNN;








//To place the distance functions here; To place the distance functions here; To place the distance functions here; To place the distance functions here;


double HDR_Tree::distance_ci_u(User* u,  Clusteri* ci){
    double distance=((u->center).cast<double>()-(ci->center)).norm();
    return distance;
}

float HDR_Tree::dist(Item* I, Cluster* C) {

    numDistanceComputions++;
    float res = (iTrans[C->l - 1] - C->center).norm();
    return res - C->radius;
}
float HDR_Tree::dist(User* U, Cluster* C) {

    numDistanceComputions++;
    float res = (uTrans[C->l - 1] - C->center).norm();
    return res - C->radius;
}
float HDR_Tree::dist(Item* I, User* U) {
    numDistanceComputions++;
    float res = (U->center - (I->v)).norm();
    return res;

}
float HDR_Tree::dist(VectorRef A, VectorRef B) {

    if ((A.rows() != B.rows()) ||
        (A.cols() != B.cols())) {
        std::cout << "Error : float HDR_Tree::dist(VectorRef A, VectorRef B) : Dimension Mismatch" << std::endl;
        std::cout << " A : " << A.rows() << "x" << A.cols() << std::endl;
        std::cout << " B : " << B.rows() << "x" << B.cols() << std::endl;
    }


    numDistanceComputions++;
    float res = (A - B).norm();
    return res;
}

double HDR_Tree::dist_cu_i(Clusteru* cu, Item* i){
    double distance=(cu->center-i->v_d).norm();
    return distance;
}


double HDR_Tree::dist_ud_id(User* ud, Item* id){
    double distance=(ud->center.cast<double>(), id->v_d).norm();
    return distance;
}


float HDR_Tree::dist_cu_u(Clusteru* cu, User* u){
    float distance=((cu->center).cast<float>()-u->center).norm();
    return distance;
}


float HDR_Tree::dist(User* U, itemCluster* C) { // v1 for User and Cluster

//	D = (I->v - center).squaredNorm();
    return (U->center - C->center).norm();

}

float HDR_Tree::dist(User* U, Item* I)
{
    float distance;
    distance=(U->center - (I->v)).norm();
    return distance;

}

float HDR_Tree::dist_i(Item* I, itemCluster* C) {

    numDistanceComputions_i++;
    float res = (iTrans_i[C->l - 1] - C->center).norm();
    return res - C->radius;
}

float HDR_Tree::dist_i(User* U, itemCluster* C) {

    numDistanceComputions_i++;
    float res = (uTrans_i[C->l - 1] - C->center).norm();
    return res - C->radius;
}

float HDR_Tree::dist_i(Item* I, User* U) {
    numDistanceComputions_i++;
    float res = (U->center - I->v).norm();
    return res;
}

float HDR_Tree::dist_i(VectorRef A, VectorRef B) {

    if ((A.rows() != B.rows()) ||
        (A.cols() != B.cols())) {
        std::cout << "Error : float itemHDR_Tree::dist(VectorRef A, VectorRef B) : Dimension Mismatch" << std::endl;
        std::cout << " A : " << A.rows() << "x" << A.cols() << std::endl;
        std::cout << " B : " << B.rows() << "x" << B.cols() << std::endl;
    }


    numDistanceComputions_i++;
    float res = (A - B).norm();
    return res;
}


float HDR_Tree::dist_u_ic(User* U, itemCluster* C){
    float dis = (Trans_u_i[C->l - 1] - C->center).norm();
    return dis;
}

float HDR_Tree::dist_u_ic_str(User* U, itemCluster* C){
    float dis=(data_i->T[C->l-1]*U->center - C->center).norm();
    return dis;
}


float HDR_Tree::dist_u_iclust_init(itemCluster* C){
    float distance;
    distance=(u_trans_i_clus_init[C->l-1]-C->center).norm();
    return distance;
}


float HDR_Tree::dist_i_ic_update_itree(Item* I, itemCluster* IC){
    float distance_ic=(data_i->T[IC->l-1]*I->v-IC->center).norm();
    return distance_ic;
}

float HDR_Tree::dist_u_uc_update_utree(User* U, Cluster* UC){
    float distance_uc=(data->T[UC->l-1]*U->center-UC->center).norm();
    return distance_uc;
}

double HDR_Tree::distance_item_clusteri(Item* i_item, Clusteri* c_clusteri){
    double distance=(i_item->v_d-c_clusteri->center).norm();
    return distance;
}


float HDR_Tree::distance_check(User *u, Item *i) {
    float distance;
    distance=(u->center-i->v).norm();
    return distance;
}


__attribute__((noinline))
float HDR_Tree::distance_test_chache(User* user, Item* item) {
    //float dummy = 0.0f;

    float distance = (user->center - item->v).norm();

    /*if (distance < -1.0f) {
        return dummy;
    }*/

    return distance;
}

// HDR_Tree.cpp





//To place the distance functions here; To place the distance functions here; To place the distance functions here; To place the distance functions here;


//To place the projection function; To place the projection function; To place the projection function; To place the projection function;

void HDR_Tree::projectItem(Item* I) {

    iTrans.clear();
    for (int i = 0; i < L; i++) {
        iTrans.emplace_back(data->T[i] * (I->v));
    }
}

void HDR_Tree::projectUser(User* U) {

    uTrans.clear();
    for (int i = 0; i < L; i++) {
        uTrans.emplace_back(data->T[i] * U->center);
    }
}

void HDR_Tree::projectItem_i(Item* I) {

    iTrans_i.clear();
    for (int i = 0; i < L_i; i++) {
        iTrans_i.emplace_back(data_i->T[i] * I->v);
    }
}

void HDR_Tree::projectUser_item(User* U){
    Trans_u_i.clear();
    for(long i=0; i<L_i; i++){
        Trans_u_i.emplace_back(data_i->T[i] * U->center);
    }
}


void HDR_Tree::project_u_iclust_pca_init(User* U){
    u_trans_i_clus_init.clear();
    for(int i=0;i<L_i;i++){
        u_trans_i_clus_init.emplace_back((data_i->T[i])*(U->center));
    }
}


// sampling probe for the batch-size model (single-query search statistics)
void HDR_Tree::probe_user_leaf_stats(User* user, long& nVisited, float& maxDist){

    nVisited = 0;
    maxDist  = 0.0f;

    project_u_iclust_pca_init(user);

    auto ini_iNLN = dynamic_cast<itemNonLeafNode*>(itemRoot_i);
    std::vector<std::tuple<float, itemNonLeafNode*>> V;
    float upbound = std::numeric_limits<float>::max();
    std::vector<std::tuple<float, Item*>> d_i;
    V.emplace_back(std::make_tuple(1.0f, ini_iNLN));
    std::tuple<float, itemNonLeafNode*> pop_node;

    long  item_add;
    float distance_user_item;

    while(V.empty()==0){
        if(V.size()==1){
            pop_node=V.back();
            V.pop_back();
        }else{
            std::nth_element(V.begin(), V.begin() + V.size() - 1, V.end(),
                             [&](const std::tuple<float, itemNonLeafNode*>& a, const std::tuple<float, itemNonLeafNode* >& b) -> bool {
                                 return std::get<0>(a) >= std::get<0>(b);
                             });
            pop_node=V.back();
            V.pop_back();
        }
        if((std::get<0>(pop_node))>upbound){
            break;
        }else{
            for(auto C: std::get<1>(pop_node)->clusters){
                float dist_r=dist_u_iclust_init(C)-(C->radius);
                if(dist_r<=upbound){
                    if((C->ptr)->type==itemNodeType::itemLeafNode){
                        if(C->Items.size()!=0){
                            item_add=0;
                            for(auto I: C->Items){
                                distance_user_item=(user->center-(std::get<1>(I))->v).norm();
                                nVisited=nVisited+1;
                                if(distance_user_item>maxDist) maxDist=distance_user_item;
                                if(distance_user_item<upbound){
                                    d_i.emplace_back(std::make_tuple(distance_user_item, std::get<1>(I)));
                                    item_add=item_add+1;
                                }
                            }
                            if((d_i.size()>=(size_t)k)&&(item_add>0)){
                                std::nth_element(d_i.begin(), d_i.begin() + k - 1, d_i.end(),
                                                 [&](const std::tuple<float, Item*>& a, const std::tuple<float, Item*>& b) -> bool {
                                                     return std::get<0>(a) <= std::get<0>(b);
                                                 });
                                d_i.assign(d_i.begin(), d_i.begin()+k);
                                upbound=std::get<0>(d_i[k-1]);
                            }
                        }
                    }else{
                        V.emplace_back(std::make_tuple(dist_r, dynamic_cast<itemNonLeafNode*>(C->ptr)));
                    }
                }
            }
        }
    }
}


//To place the projection function; To place the projection function; To place the projection function; To place the projection function;

//Functions for the idistance and B_plus tree indexing structure
void HDR_Tree::form_item_clusters(long f){
    clusters_item=new Clustersi(slidingWindow, f);
    std::vector<double> radius_clusters;
    for(auto ic: *clusters_item){
        radius_clusters.emplace_back(ic->radius);
    }
    auto mp=std::max_element(radius_clusters.begin(), radius_clusters.end());
    max_r_item_cluster=radius_clusters[mp-radius_clusters.begin()];
}

void HDR_Tree::indexing_item(){
    for(int i=0; i<clusters_item->children.size();i++){
        //clusters_item->children[i]->sequece=i;
        for(auto I_T: clusters_item->children[i]->Items){
            std::get<1>(I_T)->cluster_i=i;
            std::get<1>(I_T)->dis_to_i=std::get<0>(I_T);
            std::get<1>(I_T)->index_b_plus=double(std::get<1>(I_T)->cluster_i)*(max_r_item_cluster+0.2)+std::get<1>(I_T)->dis_to_i;
        }
    }
}

void HDR_Tree::build_bp_tree(){
    bptree_item=new BPlusTree;
};


void HDR_Tree::insert_key_point(double key, Item* it){
    bptree_item->Insert(key, it);
}


void HDR_Tree::complete_tree(){
    indexing_item();
    build_bp_tree();
    for(auto I:slidingWindow){
        insert_key_point(I->index_b_plus, I);
    }
}




void HDR_Tree::user_computeknn_id(User* u, double r_rate, double delta_r_rate, bool print_if){
    if(print_if==true){
        std::vector<double> dis_u_ci_list;
        std::cout<<" "<<std::endl;                                                        //Count switch optional Count switch optional Count switch optional Count switch optional
        std::cout<<"Begin printing the information for every cluster"<<std::endl;         //Count switch optional Count switch optional Count switch optional Count switch optional
        std::cout<<" "<<std::endl;                                                        //Count switch optional Count switch optional Count switch optional Count switch optional
        int cluster_no=0;

        for(auto ci:clusters_item->children){
            std::cout<<"No."<<cluster_no<<"    ";                                       //Count switch optional Count switch optional Count switch optional Count switch optional
            ci->in_in=0;
            ci->out_in=0;
            ci->general_in=0;
            ci->in_visited_flag= false;
            ci->out_visited_flag=false;
            ci->visited_flag=false;
            double distance_u_ci=distance_ci_u(u, ci);

            std::cout<<"distance to user="<<distance_u_ci<<"    ";                   //Count switch optional Count switch optional Count switch optional Count switch optional
            std::cout<<"radius of "<<cluster_no<<"="<<ci->radius<<"    ";            //Count switch optional Count switch optional Count switch optional Count switch optional
            std::cout<<"the number of keys="<<ci->number<<"    ";                    //Count switch optional Count switch optional Count switch optional Count switch optional

            ci->distance_user=distance_u_ci;
            if(distance_u_ci>=ci->radius){
                ci->out= true;
                std::cout<<"out"<<" ";                                               //Count switch optional Count switch optional Count switch optional Count switch optional
            }else{
                ci->out=false;
                std::cout<<"in"<<" ";                                                //Count switch optional Count switch optional Count switch optional Count switch optional
            }
            dis_u_ci_list.emplace_back(distance_u_ci);
            std::cout<<" "<<std::endl;                                               //Count switch optional Count switch optional Count switch optional Count switch optional
            cluster_no=cluster_no+1;
        }
        auto max_r=std::max_element(dis_u_ci_list.begin(), dis_u_ci_list.end());
        auto max_r_real=dis_u_ci_list[max_r-dis_u_ci_list.begin()];
        double r=(r_rate)*(max_r_real+max_r_item_cluster+0.1);
        double delta_r=delta_r_rate*(max_r_real+max_r_item_cluster+0.1);
        std::vector<std::tuple<double, Item*>> item_distance_list;
        std::vector<Item*> candidate_list;
        std::vector<Item*>* pointer_candidate=&(candidate_list);
        double distance_k=std::numeric_limits<double>::max();
        int time=-1;
        int no_ci;
        //std::cout<<" "<<std::endl;
        while(time<int((1.0-r_rate)/delta_r_rate)+1){
            /*if((item_distance_list.size()==k)&&(distance_k<r)){
                candidate_list.clear();
                for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){
                    if(clusters_item->children[no_ci]->visited_flag==false){
                        if(clusters_item->children[no_ci]->out==true){
                            if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                                bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                            false, true, false);
                                clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, false);
                                clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                        else{
                            if(clusters_item->children[no_ci]->in_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, false);
                                    clusters_item->children[no_ci]->in_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, false);
                                    clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->in_visited_flag=true;
                                }
                            }
                            if(clusters_item->children[no_ci]->out_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, false );
                                    clusters_item->children[no_ci]->out_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, false );
                                    clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->out_visited_flag=true;
                                }
                            }
                            if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                    }
                }
                if(candidate_list.size()==0){
                    break;
                }
                else{
                    for(auto i:candidate_list){
                        double distance_item_user=dist_i(i, u);
                        distance_time=distance_time+1;
                        if(distance_item_user<distance_k){
                            item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                        }
                    }
                    std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                     [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                         return std::get<0>(a) <= std::get<0>(b);
                                     });
                    item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                    distance_k=std::get<0>(item_distance_list[k-1]);
                    break;
                }
            }
            else{
                candidate_list.clear();
                for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){
                    if(clusters_item->children[no_ci]->visited_flag==false){
                        if(clusters_item->children[no_ci]->out==true){
                            if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                                bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                            false, true, false);
                                clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, false);
                                clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                        else{
                            if(clusters_item->children[no_ci]->in_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, false);
                                    clusters_item->children[no_ci]->in_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, false);
                                    clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->in_visited_flag=true;
                                }
                            }
                            if(clusters_item->children[no_ci]->out_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, false );
                                    clusters_item->children[no_ci]->out_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, false );
                                    clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->out_visited_flag=true;
                                }
                            }
                            if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                    }
                }
                if(candidate_list.size()!=0){
                    for(auto i:candidate_list){
                        double distance_item_user=dist_i(i, u);
                        distance_time=distance_time+1;
                        if(distance_item_user<distance_k){
                            item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                        }
                    }
                    if(item_distance_list.size()>=k){
                        std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                         [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                             return std::get<0>(a) <= std::get<0>(b);
                                         });
                        item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                        distance_k=std::get<0>(item_distance_list[k-1]);
                        if(distance_k<r){
                            break;
                        }
                    }
                }
                r=r+delta_r;
                time=time+1;
            }*/

// replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one

            candidate_list.clear();
            for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){

                std::cout<<"No."<<no_ci<<"   "<<"radius="<<clusters_item->children[no_ci]->radius<<"   "<<"distance to user="<<clusters_item->children[no_ci]->distance_user<<"   "<<"The "<<time+2<<" time"<<"   "<<"search_r="<<r<<std::endl;

                if(clusters_item->children[no_ci]->visited_flag==false){
                    std::cout<<"The beginning flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                    if(clusters_item->children[no_ci]->out==true){
                        std::cout<<"Out   "<<"The beginning general in is "<<clusters_item->children[no_ci]->general_in<<std::endl;
                        std::cout<<"The contribution of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                        if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                            std::cout<<"Out, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in<<std::endl;
                            bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                        false, true, true);

                            clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                        }
                        if(r>=clusters_item->children[no_ci]->distance_user){
                            std::cout<<"Out, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in<<std::endl;
                            bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, true);
                            clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                            clusters_item->children[no_ci]->visited_flag=true;
                        }
                        std::cout<<"The contribution has been showed above"<<std::endl;
                        std::cout<<"After the "<<time+2<<"th loop, the general in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->general_in<<"   "<<"The visited flag is "<<clusters_item->children[no_ci]->visited_flag<<std::endl;
                    }
                    else{
                        std::cout<<"The "<<no_ci<<" cluster is in"<<std::endl;
                        if(clusters_item->children[no_ci]->in_visited_flag==false){
                            std::cout<<"The beginning inward flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                            std::cout<<"The beginning in_in is "<<clusters_item->children[no_ci]->in_in<<std::endl;
                            std::cout<<"The contribution of the inward part of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                            if(r<clusters_item->children[no_ci]->distance_user){
                                std::cout<<"In, inward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, true);
                                clusters_item->children[no_ci]->in_in=r;
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                std::cout<<"In, inward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, true);
                                clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                clusters_item->children[no_ci]->in_visited_flag=true;
                            }
                            std::cout<<"The contribution has been showed above"<<std::endl;
                            std::cout<<"After the "<<time+2<<"th loop, the in in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->in_in<<"   "<<"The inward visited flag is "<<clusters_item->children[no_ci]->in_visited_flag<<std::endl;
                        }
                        if(clusters_item->children[no_ci]->out_visited_flag==false){
                            std::cout<<"The beginning outward flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                            std::cout<<"The beginning out_in is "<<clusters_item->children[no_ci]->out_in<<std::endl;
                            std::cout<<"The contribution of the outward part of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                            if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                std::cout<<"In, outward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, true);
                                clusters_item->children[no_ci]->out_in=r;
                            }
                            if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                std::cout<<"In, outward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, true);
                                clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                clusters_item->children[no_ci]->out_visited_flag=true;
                            }
                            std::cout<<"The contribution has been showed above"<<std::endl;
                            std::cout<<"After the "<<time+2<<"th loop, the out in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->out_in<<"   "<<"The outward visited flag is "<<clusters_item->children[no_ci]->out_visited_flag<<std::endl;
                        }
                        if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                            clusters_item->children[no_ci]->visited_flag=true;
                        }
                        std::cout<<"After the "<<time+2<<"th loop, the visited flag of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->visited_flag<<std::endl;
                    }
                }else{
                    std::cout<<"The beginning flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is true, no need of search any more"<<std::endl;
                }
                std::cout<<" "<<std::endl;
            }
            if(candidate_list.size()!=0){
                for(auto i:candidate_list){
                    double distance_item_user=dist_i(i, u);
                    distance_time=distance_time+1;
                    if(distance_item_user<distance_k){
                        item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                    }
                }
                if(item_distance_list.size()>=k){
                    std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                     [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                         return std::get<0>(a) <= std::get<0>(b);
                                     });
                    item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                    distance_k=std::get<0>(item_distance_list[k-1]);
                    if(distance_k<r){
                        break;
                    }
                }
            }else{
                if(distance_k<r){
                    break;
                }
            }
            r=r+delta_r;
            time=time+1;





// replacement one  replacement one replacement one   replacement one   replacement one   replacement one   replacement one   replacement one  replacement one

        }
        u->R.clear();
        for(int count_item=0; count_item<k; count_item++){
            u->R.emplace_back(std::get<1>(item_distance_list[count_item]));
        }
        //std::cout<<candidate_list.size()<<std::endl;

        /*if(candidate_list.size()<6000){
            std::cout<<"--------------------------------------------------------------------------------------------------------"<<std::endl;
        }*/
        u->dknn=distance_k;
    }
    else{
        std::vector<double> dis_u_ci_list;
        //std::cout<<" "<<std::endl;                                                        //Count switch optional Count switch optional Count switch optional Count switch optional
        //std::cout<<"Begin printing the information for every cluster"<<std::endl;         //Count switch optional Count switch optional Count switch optional Count switch optional
        //std::cout<<" "<<std::endl;                                                        //Count switch optional Count switch optional Count switch optional Count switch optional
        int cluster_no=0;

        for(auto ci:clusters_item->children){
            //std::cout<<"No."<<cluster_no<<"    ";                                       //Count switch optional Count switch optional Count switch optional Count switch optional
            ci->in_in=0;
            ci->out_in=0;
            ci->general_in=0;
            ci->in_visited_flag= false;
            ci->out_visited_flag=false;
            ci->visited_flag=false;
            double distance_u_ci=distance_ci_u(u, ci);

            distance_time=distance_time+1;

            //std::cout<<"distance to user="<<distance_u_ci<<"    ";                   //Count switch optional Count switch optional Count switch optional Count switch optional
            //std::cout<<"radius of "<<cluster_no<<"="<<ci->radius<<"    ";            //Count switch optional Count switch optional Count switch optional Count switch optional
            //std::cout<<"the number of keys="<<ci->number<<"    ";                    //Count switch optional Count switch optional Count switch optional Count switch optional

            ci->distance_user=distance_u_ci;
            if(distance_u_ci>=ci->radius){
                ci->out= true;
                //std::cout<<"out"<<" ";                                               //Count switch optional Count switch optional Count switch optional Count switch optional
            }else{
                ci->out=false;
                //std::cout<<"in"<<" ";                                                //Count switch optional Count switch optional Count switch optional Count switch optional
            }
            dis_u_ci_list.emplace_back(distance_u_ci);
            //std::cout<<" "<<std::endl;                                               //Count switch optional Count switch optional Count switch optional Count switch optional
            cluster_no=cluster_no+1;
        }
        auto max_r=std::max_element(dis_u_ci_list.begin(), dis_u_ci_list.end());
        auto max_r_real=dis_u_ci_list[max_r-dis_u_ci_list.begin()];
        double r=(r_rate)*(max_r_real+max_r_item_cluster+0.1);
        double delta_r=delta_r_rate*(max_r_real+max_r_item_cluster+0.1);
        std::vector<std::tuple<double, Item*>> item_distance_list;
        std::vector<Item*> candidate_list;
        std::vector<Item*>* pointer_candidate=&(candidate_list);
        double distance_k=std::numeric_limits<double>::max();
        int time=-1;
        int no_ci;
        //std::cout<<" "<<std::endl;
        while(time<int((1.0-r_rate)/delta_r_rate)+1){
            /*if((item_distance_list.size()==k)&&(distance_k<r)){
                candidate_list.clear();
                for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){
                    if(clusters_item->children[no_ci]->visited_flag==false){
                        if(clusters_item->children[no_ci]->out==true){
                            if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                                bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                            false, true, false);
                                clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, false);
                                clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                        else{
                            if(clusters_item->children[no_ci]->in_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, false);
                                    clusters_item->children[no_ci]->in_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, false);
                                    clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->in_visited_flag=true;
                                }
                            }
                            if(clusters_item->children[no_ci]->out_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, false );
                                    clusters_item->children[no_ci]->out_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, false );
                                    clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->out_visited_flag=true;
                                }
                            }
                            if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                    }
                }
                if(candidate_list.size()==0){
                    break;
                }
                else{
                    for(auto i:candidate_list){
                        double distance_item_user=dist_i(i, u);
                        distance_time=distance_time+1;
                        if(distance_item_user<distance_k){
                            item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                        }
                    }
                    std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                     [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                         return std::get<0>(a) <= std::get<0>(b);
                                     });
                    item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                    distance_k=std::get<0>(item_distance_list[k-1]);
                    break;
                }
            }
            else{
                candidate_list.clear();
                for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){
                    if(clusters_item->children[no_ci]->visited_flag==false){
                        if(clusters_item->children[no_ci]->out==true){
                            if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                                bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                            false, true, false);
                                clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, false);
                                clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                        else{
                            if(clusters_item->children[no_ci]->in_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, false);
                                    clusters_item->children[no_ci]->in_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, false);
                                    clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->in_visited_flag=true;
                                }
                            }
                            if(clusters_item->children[no_ci]->out_visited_flag==false){
                                if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, false );
                                    clusters_item->children[no_ci]->out_in=r;
                                }
                                if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                    bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, false );
                                    clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                    clusters_item->children[no_ci]->out_visited_flag=true;
                                }
                            }
                            if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                                clusters_item->children[no_ci]->visited_flag=true;
                            }
                        }
                    }
                }
                if(candidate_list.size()!=0){
                    for(auto i:candidate_list){
                        double distance_item_user=dist_i(i, u);
                        distance_time=distance_time+1;
                        if(distance_item_user<distance_k){
                            item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                        }
                    }
                    if(item_distance_list.size()>=k){
                        std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                         [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                             return std::get<0>(a) <= std::get<0>(b);
                                         });
                        item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                        distance_k=std::get<0>(item_distance_list[k-1]);
                        if(distance_k<r){
                            break;
                        }
                    }
                }
                r=r+delta_r;
                time=time+1;
            }*/

// replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one  replacement one

            candidate_list.clear();
            for(no_ci=0; no_ci<(clusters_item->children).size(); no_ci++){

                //std::cout<<"No."<<no_ci<<"   "<<"radius="<<clusters_item->children[no_ci]->radius<<"   "<<"distance to user="<<clusters_item->children[no_ci]->distance_user<<"   "<<"The "<<time+2<<" time"<<"   "<<"search_r="<<r<<std::endl;

                if(clusters_item->children[no_ci]->visited_flag==false){
                    //std::cout<<"The beginning flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                    if(clusters_item->children[no_ci]->out==true){
                        //std::cout<<"Out   "<<"The beginning general in is "<<clusters_item->children[no_ci]->general_in<<std::endl;
                        //std::cout<<"The contribution of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                        if((r>clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius)&&((r<clusters_item->children[no_ci]->distance_user))){
                            //std::cout<<"Out, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in<<std::endl;
                            bptree_item->get_range_item(pointer_candidate,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in,
                                                        false, true, false);

                            clusters_item->children[no_ci]->general_in=r-(clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->radius);
                        }
                        if(r>=clusters_item->children[no_ci]->distance_user){
                            //std::cout<<"Out, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in<<std::endl;
                            bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->general_in, true, true, false);
                            clusters_item->children[no_ci]->general_in=clusters_item->children[no_ci]->radius;
                            clusters_item->children[no_ci]->visited_flag=true;
                        }
                        //std::cout<<"The contribution has been showed above"<<std::endl;
                        //std::cout<<"After the "<<time+2<<"th loop, the general in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->general_in<<"   "<<"The visited flag is "<<clusters_item->children[no_ci]->visited_flag<<std::endl;
                    }
                    else{
                        //std::cout<<"The "<<no_ci<<" cluster is in"<<std::endl;
                        if(clusters_item->children[no_ci]->in_visited_flag==false){
                            //std::cout<<"The beginning inward flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                            //std::cout<<"The beginning in_in is "<<clusters_item->children[no_ci]->in_in<<std::endl;
                            //std::cout<<"The contribution of the inward part of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                            if(r<clusters_item->children[no_ci]->distance_user){
                                //std::cout<<"In, inward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-r, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, false, true, false);
                                clusters_item->children[no_ci]->in_in=r;
                            }
                            if(r>=clusters_item->children[no_ci]->distance_user){
                                //std::cout<<"In, inward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2), double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user-clusters_item->children[no_ci]->in_in, true, true, false);
                                clusters_item->children[no_ci]->in_in=clusters_item->children[no_ci]->distance_user;
                                clusters_item->children[no_ci]->in_visited_flag=true;
                            }
                            //std::cout<<"The contribution has been showed above"<<std::endl;
                            //std::cout<<"After the "<<time+2<<"th loop, the in in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->in_in<<"   "<<"The inward visited flag is "<<clusters_item->children[no_ci]->in_visited_flag<<std::endl;
                        }
                        if(clusters_item->children[no_ci]->out_visited_flag==false){
                            //std::cout<<"The beginning outward flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is false"<<std::endl;
                            //std::cout<<"The beginning out_in is "<<clusters_item->children[no_ci]->out_in<<std::endl;
                            //std::cout<<"The contribution of the outward part of the "<<no_ci<<" cluster is to be showed, it has "<<clusters_item->children[no_ci]->number<<" items"<<std::endl;
                            if(r<clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                //std::cout<<"In, outward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+r, false, true, false);
                                clusters_item->children[no_ci]->out_in=r;
                            }
                            if(r>=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user){
                                //std::cout<<"In, outward, the base space is "<<max_r_item_cluster+0.2<<" The distance is "<<clusters_item->children[no_ci]->distance_user<<" The radius is "<<clusters_item->children[no_ci]->radius<<" the search floor is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in<<" the cell is "<<double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius<<std::endl;
                                bptree_item->get_range_item(pointer_candidate, double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->distance_user+clusters_item->children[no_ci]->out_in,double(no_ci)*(max_r_item_cluster+0.2)+clusters_item->children[no_ci]->radius, false, true, false);
                                clusters_item->children[no_ci]->out_in=clusters_item->children[no_ci]->radius-clusters_item->children[no_ci]->distance_user;
                                clusters_item->children[no_ci]->out_visited_flag=true;
                            }
                            //std::cout<<"The contribution has been showed above"<<std::endl;
                            //std::cout<<"After the "<<time+2<<"th loop, the out in of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->out_in<<"   "<<"The outward visited flag is "<<clusters_item->children[no_ci]->out_visited_flag<<std::endl;
                        }
                        if((clusters_item->children[no_ci]->in_visited_flag==true)&&(clusters_item->children[no_ci]->out_visited_flag==true)){
                            clusters_item->children[no_ci]->visited_flag=true;
                        }
                        //std::cout<<"After the "<<time+2<<"th loop, the visited flag of the "<<no_ci<<" cluster is "<<clusters_item->children[no_ci]->visited_flag<<std::endl;
                    }
                }else{
                    //std::cout<<"The beginning flag of "<<no_ci<<" cluster for the "<<time+2<<" time search is true, no need of search any more"<<std::endl;
                }
                //std::cout<<" "<<std::endl;
            }
            if(candidate_list.size()!=0){
                for(auto i:candidate_list){
                    double distance_item_user=dist_i(i, u);

                    distance_time=distance_time+1;

                    if(distance_item_user<distance_k){
                        item_distance_list.emplace_back(std::make_tuple(distance_item_user, i));
                    }
                }
                if(item_distance_list.size()>=k){
                    std::nth_element(item_distance_list.begin(), item_distance_list.begin() + k - 1, item_distance_list.end(),
                                     [&](const std::tuple<double, Item*>& a, const std::tuple<double, Item*>& b) -> bool {
                                         return std::get<0>(a) <= std::get<0>(b);
                                     });
                    item_distance_list.assign(item_distance_list.begin(), item_distance_list.begin()+k);
                    distance_k=std::get<0>(item_distance_list[k-1]);
                    if(distance_k<r){
                        break;
                    }
                }
            }else{
                if(distance_k<r){
                    break;
                }
            }
            r=r+delta_r;
            time=time+1;





// replacement one  replacement one replacement one   replacement one   replacement one   replacement one   replacement one   replacement one  replacement one

        }
        u->R.clear();
        for(int count_item=0; count_item<k; count_item++){
            u->R.emplace_back(std::get<1>(item_distance_list[count_item]));
        }
        //std::cout<<candidate_list.size()<<std::endl;

        /*if(candidate_list.size()<6000){
            std::cout<<"--------------------------------------------------------------------------------------------------------"<<std::endl;
        }*/
        u->dknn=distance_k;
    }
}



//Functions for the idistance and B_plus tree indexing structure


//Functions for the sphere tree

void HDR_Tree::construct_sphere(long fanout, long threshold) {

    std::cout << "Constructing The sphere tree" << std::endl;
    std::cout << "Users: " << numUsers << std::endl;
    std::cout << "Features: " << numFeatures << std::endl;
    std::cout << "Window: " << windowSize << std::endl;
    std::cout << "Fanout: " << fanout << std::endl << std::endl;

    //init();

    //L = log(data->U.size()) / log(fanout);
    //std::cout << "Estimated Height: " << L << " levels." << std::endl;

    //data->initTransform(L);
    long numClusters = 0;

    auto C = new Clustersu(Users, fanout);
    for (auto Cj : *C) {

        if (Cj->Users.size() > 0) {
            Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
            for (auto t : Cj->Users) {
                Cj->maxdknn = std::max(Cj->maxdknn, double(std::get<1>(t)->dknn));
            }
        }

        numClusters += 1;
        Cj->l = 1;
    }

    root_sphere = new userNonLeafNode(C, 1, false);

    std::queue<userNode *> q;

    q.push(root_sphere);

    long maxL = 0;

    while (!q.empty()) {
        auto node = dynamic_cast<userNonLeafNode *>(q.front());
        q.pop();

        auto Cp = node->clusters;

        for (auto Cj : Cp) {

            if (Cj->number < threshold) {
                auto LN = new userLeafNode(Cj, false);

                node->children.emplace_back(LN);
                Cj->ptr = LN;
            }
            else {
                auto level = node->l + 1;
                maxL = std::max(maxL, level);

                /*if(level>L){
                    std::cout<<"Excceed the estimated level"<<std::endl;
                }*/

                auto Cpp = new Clustersu(Cj, fanout);
                for (auto Cj : *Cpp) {

                    if (Cj->Users.size() > 0) {
                        Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
                        for (auto t : Cj->Users) {
                            Cj->maxdknn = std::max(Cj->maxdknn, double(std::get<1>(t)->dknn));
                        }
                    }

                    numClusters += 1;
                    Cj->l = level;
                }

                auto NLN = new userNonLeafNode(Cpp, level, false);

                node->children.emplace_back(NLN);
                Cj->ptr = NLN;
                q.push(NLN);
            }
        }
    }

    std::cout << "Actual Height: " << maxL << " levels." << std::endl;
    std::cout << "Clusters: " << numClusters << std::endl;

//	for (int i = 0; i < 100; i++) {
//		Users[0]->update(slidingWindow[i], Mode::Add);
//	}

//	for (int i = 0; i < 100; i++) {
//		if (Users[0]->dist(slidingWindow[i]) < Users[0]->dknn) {
//			Users[0]->update(slidingWindow[i], Mode::Remove);
//			break;
//		}
//	}


//	fillTree();

}



void HDR_Tree::search_sphere(userNode *node_sphere, Item *I) {






    if (node_sphere->type == userNodeType::userLeafNode) {
        auto LN = dynamic_cast<userLeafNode*>(node_sphere);
        for (auto Ut : *LN) {

            //num_sphere_cal_users=num_sphere_cal_users+1;    //Add the calculation time for users

            auto user = std::get<1>(Ut);

            distance_time=distance_time+1;

            if (dist(I, user) < user->dknn) {
                Rp_sphere.emplace_back(user);
            }
        }
    }
    else {
        auto NLN = dynamic_cast<userNonLeafNode*>(node_sphere);
        auto& Cp = NLN->clusters;


        double d1,d2;

        for (long j = 0; j < Cp.size(); j++) {

            //num_sphere_cal_clusters=num_sphere_cal_clusters+1;   //Add the calculation time for clusters

            distance_time=distance_time+1;

            d1 = dist_cu_i(Cp[j],I)-Cp[j]->radius;
            d2 = Cp[j]->maxdknn;
            if (d1 < d2) {
                search_sphere(NLN->children[j], I);
            }
        }
    }
}



//Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block;


void HDR_Tree::updateUsers(Item* I, Mode m, int o, int n) {

    for (auto u : Rp) {

        if (verbose) {
            std::cout << "User at " << u->center.transpose() << " effected" << std::endl;
        }

        u->update(I, m);

        if (u->R.size() < k) {
            u->computeKNN(slidingWindow);
            numKnnComputions++;
        }

    }
}
void HDR_Tree::updateUsersParalell(Item* I, Mode m) {

//	std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();

    int n = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;

    for (long i = 0; i < n; i++) {
        threads.push_back(std::thread(
                &HDR_Tree::updateUsers, this, I, m, i, n
        ));
    }

//	std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();

    for (auto& th : threads) {
        th.join();
    }

//	std::chrono::steady_clock::time_point tC = std::chrono::steady_clock::now();

//	float cT = std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count();
//	float pT = std::chrono::duration_cast<std::chrono::microseconds>(tC - tB).count();

//	std::cout << "Create: " << cT << ", Process: " << pT << std::endl;

}
void HDR_Tree::updateUsersSerial(Item* I, Mode m) {

    std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();

    for (auto u : Rp) {


        if (verbose) {
            std::cout << "User at " << u->center.transpose() << " effected" << std::endl;
        }

        u->update(I, m);

        if (u->R.size() < k) {
            u->computeKNN(slidingWindow);
            numKnnComputions++;
        }

    }

    std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();
    updateTime = updateTime + (std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count()) / 1000000.0;

}




void HDR_Tree::userUpdate(User* U, Mode m) {

    /*UpdateNaiveRKNN(I, m);*/
    userUpdateHdrKNN(U, m);

}
void HDR_Tree::update(Item* I, Mode m) {

    /*UpdateNaiveRKNN(I, m);*/
    UpdateHdrKNN(I, m);

}
void HDR_Tree::UpdateNaiveRKNN(Item* I, Mode m) {

    Rp.clear();

    for (auto u : Users) {
        if (dist(I, u) < u->dknn) {
            /*std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();
            u->computeKNN(slidingWindow);
            std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();
            updateTime = updateTime +  (std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count())/1000000.0;*/
            Rp.emplace_back(u);
            /*numKnnComputions++;*/
        }
    }

    if (Rp.size() > 0) {
        //	updateUsersParalell(I, m);
        updateUsersSerial(I, m);

        //		std::chrono::steady_clock::time_point tC = std::chrono::steady_clock::now();

        AdjustMaxdknnPaper(root);
        //		std::chrono::steady_clock::time_point tD = std::chrono::steady_clock::now();

        //		AdjustMaxdknnPrev(root);
        //		std::chrono::steady_clock::time_point tE = std::chrono::steady_clock::now();

        //		std::cout << "Search Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << std::endl;
        //		std::cout << "Update Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tC - tB).count() << std::endl;
        //		std::cout << "Adjust Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tD - tC).count() << std::endl;
        //		std::cout << "Adjust Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tE - tD).count() << std::endl;
    }
}
void HDR_Tree::UpdateHdrKNN(Item* I, Mode m) {

    //	std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();

    Rp.clear();

    projectItem(I);

    Search(root, I);

    //	std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();
    //	std::cout << "Search Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << std::endl;

    if (verbose) {
        std::cout << "Searching item " << I->v.transpose() << std::endl;
    }

    if (Rp.size() > 0) {
        //	updateUsersParalell(I, m);
        updateUsersSerial(I, m);

        //		std::chrono::steady_clock::time_point tC = std::chrono::steady_clock::now();

        AdjustMaxdknnPaper(root);
        //		std::chrono::steady_clock::time_point tD = std::chrono::steady_clock::now();

        //		AdjustMaxdknnPrev(root);
        //		std::chrono::steady_clock::time_point tE = std::chrono::steady_clock::now();

        //		std::cout << "Search Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << std::endl;
        //		std::cout << "Update Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tC - tB).count() << std::endl;
        //		std::cout << "Adjust Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tD - tC).count() << std::endl;
        //		std::cout << "Adjust Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tE - tD).count() << std::endl;
    }
}

void HDR_Tree::userUpdateHdrKNN(User* U, Mode m) {

    projectUser(U);

    updateUser(root, U);


    //if (Rp.size() > 0) {
    //	//	updateUsersParalell(I, m);
    //	updateUsersSerial(I, m);

    //	AdjustMaxdknnPaper(root);
    //}
}



void HDR_Tree::updateConstruct(Node* node) {

    std::queue<Node*> q;

    q.push(node);

    long maxL = 0;

    while (!q.empty()) {
        auto node = dynamic_cast<NonLeafNode*>(q.front());
        q.pop();

        auto Cp = node->clusters;

        for (auto Cj : Cp) {

            if (Cj->number < fix_threshold) {
                auto LN = new LeafNode(Cj, verbose);

                node->children.emplace_back(LN);
                Cj->ptr = LN;
            }
            else {
                auto level = node->l + 1;
                maxL = std::max(maxL, level);

                auto Cpp = new Clusters(Cj, 4, data->T[level - 1]);
                for (auto Cj : *Cpp) {

                    if (Cj->Users.size() > 0) {
                        Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
                        for (auto t : Cj->Users) {
                            Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
                        }
                    }

                    /*numClusters += 1;*/
                    Cj->l = level;
                }

                auto NLN = new NonLeafNode(Cpp, level, verbose);

                node->children.emplace_back(NLN);
                Cj->ptr = NLN;
                q.push(NLN);
            }
        }
    }
}

void HDR_Tree::updateUser(Node* node, User* U) {



    if (node->type == NodeType::LeafNode) {
        auto LN = dynamic_cast<LeafNode*>(node);
        long maxL = 0;

        if (LN->cluster->number < fix_threshold) {
            U->computeKNN(slidingWindow);

            LN->cluster->Users.emplace_back(std::tuple<float, User*>(U->dknn, U));
            /*LN->cluster->number += 1;*/

            std::cout << "User added Successfully!!!" << std::endl;
            std::cout << "Number of users after insertion in LN: " << LN->cluster->number << std::endl;
            std::cout << "Number of users after insertion in LN: " << LN->cluster->Users.size() << std::endl;
        }
        else
        {
            // We need to work on this part. If it gets overflow we have to split the node.

            /*long level = LN->cluster->l + 1;
            maxL = std::max(maxL, level);*/

            /*updateConstruct(node);*/

            /*auto Cpp = new Clusters(LN->cluster, fix_threshold, data->T[level-1]);
            for (auto Cj : *Cpp) {
                if (Cj->Users.size() > 0) {
                    Cj->maxdknn = std::get<1>(Cj->Users[0])->dknn;
                    for (auto t : Cj->Users) {
                        Cj->maxdknn = std::max(Cj->maxdknn, std::get<1>(t)->dknn);
                    }
                }
                Cj->l = level;
            }

            auto NLN = new NonLeafNode(Cpp, level, verbose);*/

            //auto New_NLN = dynamic_cast<NonLeafNode*>(LN);

            //New_NLN->children.emplace_back(NLN);
            //
            //
            //
            ///*parent->children.emplace_back(NLN);*/
            ///*LN->cluster->ptr(NLN);*/

            //for (auto Cj : NLN->clusters) {
            //	/*Cj->ptr = NLN;*/
            //	std::cout << "Number of users after creation in in LN: " << Cj->number << std::endl;
            //
            //	if (Cj->number < fix_threshold) {
            //		auto LN = new LeafNode(Cj, verbose);

            //		NLN->children.emplace_back(LN);
            //		Cj->ptr = LN;

            //	}
            //
            //}

            //node->children.emplace_back(NLN);
            //Cj->ptr = NLN;
            //q.push(NLN);

            /*if (LN->cluster->number < fix_threshold) {
                LN->cluster->Users.emplace_back(std::tuple(U->dknn, U));
            }*/
        }


    }

    else {
        auto NLN = dynamic_cast<NonLeafNode*>(node);
        auto& Cp = NLN->clusters;

        float d;
        int clust;
        auto minDist = std::numeric_limits<float>::max();

        for (long j = 0; j < Cp.size(); j++) {
            d = dist(U, Cp[j]);

            if (d < minDist) {
                minDist = d;
                clust = j;
            }
        }

        /*Cp[clust]->number += 1;*/

        if (Cp[clust]->radius < minDist) {
            Cp[clust]->radius = minDist;
        }

        /*parent = dynamic_cast<NonLeafNode*>(NLN);*/

        updateUser(NLN->children[clust], U);
    }
}


void HDR_Tree::remove() {

    auto L = slidingWindow.back();
    slidingWindow.pop_back();
    update(L, Mode::Remove);
}

void HDR_Tree::insert(Item* I) {

//	std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();

    if (slidingWindow.size() == windowSize) {

        remove();
    }

//	std::cout << "Computed Distance (Remove) " << numDistanceComputions << " times" << std::endl;
//	std::cout << "Computed KNN (Remove) " << numKnnComputions << " times" << std::endl;

//	std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();

    slidingWindow.emplace(slidingWindow.begin(), I);

//	std::chrono::steady_clock::time_point tC = std::chrono::steady_clock::now();
    update(I, Mode::Add);

//	std::cout << "Computed Distance " << numDistanceComputions << " times" << std::endl;
//	std::cout << "Computed KNN " << numKnnComputions << " times" << std::endl;

//	std::chrono::steady_clock::time_point tD = std::chrono::steady_clock::now();

//	std::cout << "Remove Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << std::endl;
//	std::cout << "Emplace Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tC - tB).count() << std::endl;
//	std::cout << "Update Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tD - tC).count() << std::endl;

}
void HDR_Tree::userInsert(User* U) {

    userUpdate(U, Mode::Add);

}

void HDR_Tree::fillTree() {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (long i = 0; i < windowSize; i++) {
        Item* I = new Item(data->D[i]);
        insert(I);

        if (i % 1000 == 0) {

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            diff = diff / 1000.0f;

            std::cout << "[" << diff << "] Processing Item " << i << std::endl;
        }
    }
}




void HDR_Tree::initUsersSerial() {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (long i = 0; i < Users.size(); i++) {
        Users[i]->computeKNN(slidingWindow);

        if (i % 1000 == 0) {

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            diff = diff / 1000.0f;

            std::cout << "[" << diff << "] Processing User " << i << std::endl;
        }
    }
}
void HDR_Tree::initUsersParalell() {

    int n = std::thread::hardware_concurrency();

    std::cout << "Using " << n << " paralell threads to initialize Users" << std::endl;

    std::vector<std::thread> threads;
    begin = std::chrono::steady_clock::now();

    for (long i = 0; i < n; i++) {
        threads.push_back(std::thread(
                &HDR_Tree::UsersParalell, this, i, n
        ));
    }

    for (auto& th : threads) {
        th.join();
    }
}
void HDR_Tree::UsersParalell(int o, int n) {

    for (long i = o; i < Users.size(); i = i + n) {
        //-------------------------------------------------------------------------------//
        /*std::cout<<"check the root of the item hdr tree"<<std::endl;
        std::cout<<itemRoot_ptr<<std::endl;
        std::cout<<"check success"<<std::endl;*/
        //-------------------------------------------------------------------------------//
        //computeknnuser_noroot(Users[i]);   //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
        Users[i]->computeKNN(slidingWindow);     //test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_test_0_
        //user_computeknn_id(Users[i], 0.01,0.01, false);
    }

    //for (long i = o; i < Users.size(); i=i+n) {
    //	/*Users[i]->computeKNN(slidingWindow);*/
    //	Users[i]->userComputeKNN(itemRoot_ptr);

    //	if (i % (n*250) == 0) {

    //		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //		float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    //		diff = diff / 1000.0f;

    //		std::cout << "[" << diff << "] Processing User " << i << std::endl;
    //	}
    //}
}



void HDR_Tree::streamIndex(long ind) {

//	std::chrono::steady_clock::time_point tA = std::chrono::steady_clock::now();

//	std::cout << "Streaming" << std::endl;
    Item* I = new Item(data->D[ind]);

//	std::chrono::steady_clock::time_point tB = std::chrono::steady_clock::now();

    insert(I);

//	std::chrono::steady_clock::time_point tC = std::chrono::steady_clock::now();

    //	std::cout << "Create Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << std::endl;
    //	std::cout << "Insert Time:" << std::chrono::duration_cast<std::chrono::microseconds>(tC - tB).count() << std::endl;
    //	std::cout << "------------------------------------------------------------" << std::endl;
}
void HDR_Tree::userStreamIndex(long ind) {

    User* U = new User(data->D[ind],k);
    userInsert(U);

}
void HDR_Tree::streamRandom() {

    /* Seed */
    std::random_device rd;

    /* Random number generator */
    std::default_random_engine generator(rd());

    /* Distribution on which to apply the generator */
    std::uniform_int_distribution<long unsigned> distribution(0, data->D.size() - 1);

    streamIndex(distribution(generator));
}
void HDR_Tree::userStreamRandom() {

    /* Seed */
    std::random_device rd;

    /* Random number generator */
    std::default_random_engine generator(rd());

    /* Distribution on which to apply the generator */
    std::uniform_int_distribution<long unsigned> distribution(0, data->D.size() - 1);

    userStreamIndex(distribution(generator));
}
void HDR_Tree::streamLinear() {

    streamIndex(itemIndex);
    itemIndex++;
}
void HDR_Tree::stream() {
    streamLinear();
    /*streamRandom();*/
}

void HDR_Tree::userStream() {
    /*streamLinear();*/
    userStreamRandom();
}


void HDR_Tree::syncUsers(int o, int n) {

    for (auto u : Rp) {

        if (u->R.size() < k) {
            u->computeKNN(slidingWindow);
        }
    }
}
void HDR_Tree::sync() {

//	std::cout << "Syncing " << Rp.size() << " Users. " << std::endl;

    for (auto u : Rp)
    {
        if (u->R.size() < k) {
            u->computeKNN(slidingWindow);
        }
    }

    Rp.clear();
}


void HDR_Tree::Search(VectorRef V) {
    Item* I = new Item(V);

    Rp.clear();
    Search(root, I);
}


void HDR_Tree::test_cache_effect() {
    std::cout << "\n========== Testing Cache Effect ==========\n";

    // Use actual data dimensions
    const int DIM = 784;
    const int NUM_ITEMS = 1000;
    const int NUM_USERS = 100;
    const int NUM_TESTS = 1000000;

    // Create test data
    std::vector<Eigen::VectorXf> items(NUM_ITEMS);
    std::vector<Eigen::VectorXf> users(NUM_USERS);

    // Initialize with random data
    for(int i = 0; i < NUM_ITEMS; i++) {
        items[i] = Eigen::VectorXf::Random(DIM);
    }
    for(int i = 0; i < NUM_USERS; i++) {
        users[i] = Eigen::VectorXf::Random(DIM);
    }

    // Test 1: Batch access pattern (cache friendly)
    std::cout << "Test 1: Batch access pattern (cache friendly)" << std::endl;
    auto start1 = std::chrono::high_resolution_clock::now();
    float sum1 = 0;

    // Each item accessed by multiple users consecutively
    for(int item_idx = 0; item_idx < 10; item_idx++) {  // Only use first 10 items
        for(int test = 0; test < NUM_TESTS/10; test++) {
            int user_idx = test % NUM_USERS;
            sum1 += (users[user_idx] - items[item_idx]).norm();
        }
    }

    auto end1 = std::chrono::high_resolution_clock::now();
    auto time1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - start1).count();

    // Clear cache
    std::vector<char> cache_cleaner(50*1024*1024, 0);
    volatile char tmp = 0;
    for(auto& c : cache_cleaner) tmp += c;

    // Test 2: Random access pattern (cache unfriendly)
    std::cout << "Test 2: Random access pattern (cache unfriendly)" << std::endl;
    auto start2 = std::chrono::high_resolution_clock::now();
    float sum2 = 0;

    // Random access to different items
    srand(12345);  // Fixed seed for reproducibility
    for(int test = 0; test < NUM_TESTS; test++) {
        int item_idx = rand() % NUM_ITEMS;  // Random item
        int user_idx = test % NUM_USERS;
        sum2 += (users[user_idx] - items[item_idx]).norm();
    }

    auto end2 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2).count();

    // Output results
    std::cout << "\nResults:" << std::endl;
    std::cout << "Batch pattern avg time: " << time1 / (double)NUM_TESTS << " ns/distance" << std::endl;
    std::cout << "Random pattern avg time: " << time2 / (double)NUM_TESTS << " ns/distance" << std::endl;
    std::cout << "Time difference: " << (time2 - time1) / (double)NUM_TESTS << " ns/distance" << std::endl;
    std::cout << "Performance ratio: " << (double)time2 / time1 << "x" << std::endl;

    // Prevent compiler optimization
    std::cout << "\n(Anti-optimization output: " << sum1 + sum2 << ")" << std::endl;

    // Additional info
    std::cout << "\nCache Analysis:" << std::endl;
    size_t vector_bytes = DIM * sizeof(float);
    std::cout << "Vector size: " << vector_bytes << " bytes" << std::endl;
    std::cout << "L1 cache (32KB) can hold: " << 32*1024 / vector_bytes << " vectors" << std::endl;
    std::cout << "L2 cache (256KB) can hold: " << 256*1024 / vector_bytes << " vectors" << std::endl;
}


void HDR_Tree::analyze_cache_pattern() {
    std::cout << "\n========== Cache Access Pattern Analysis ==========\n";

    std::cout << "\n[Single-point Algorithm]" << std::endl;
    int single_total_items = 0;
    int single_reused_items = 0;
    int single_max_reuse = 0;
    long single_total_accesses = 0;

    std::map<int, int> single_reuse_histogram;

    for(auto& pair : item_access_count_single) {
        single_total_items++;
        single_total_accesses += pair.second;

        if(pair.second > 1) {
            single_reused_items++;
        }
        if(pair.second > single_max_reuse) {
            single_max_reuse = pair.second;
        }

        single_reuse_histogram[pair.second]++;
    }

    std::cout << "Total unique items accessed: " << single_total_items << std::endl;
    std::cout << "Total accesses: " << single_total_accesses << std::endl;
    std::cout << "Items accessed multiple times: " << single_reused_items
              << " (" << (100.0 * single_reused_items / single_total_items) << "%)" << std::endl;
    std::cout << "Average accesses per item: " << (double)single_total_accesses / single_total_items << std::endl;
    std::cout << "Maximum reuse count: " << single_max_reuse << std::endl;

    std::cout << "\n[Batch Algorithm]" << std::endl;
    int batch_total_items = 0;
    int batch_reused_items = 0;
    int batch_max_reuse = 0;
    long batch_total_accesses = 0;

    std::map<int, int> batch_reuse_histogram;

    for(auto& pair : item_access_count_batch) {
        batch_total_items++;
        batch_total_accesses += pair.second;

        if(pair.second > 1) {
            batch_reused_items++;
        }
        if(pair.second > batch_max_reuse) {
            batch_max_reuse = pair.second;
        }

        batch_reuse_histogram[pair.second]++;
    }

    std::cout << "Total unique items accessed: " << batch_total_items << std::endl;
    std::cout << "Total accesses: " << batch_total_accesses << std::endl;
    std::cout << "Items accessed multiple times: " << batch_reused_items
              << " (" << (100.0 * batch_reused_items / batch_total_items) << "%)" << std::endl;
    std::cout << "Average accesses per item: " << (double)batch_total_accesses / batch_total_items << std::endl;
    std::cout << "Maximum reuse count: " << batch_max_reuse << std::endl;

    std::cout << "\n[Reuse Distribution - Batch Algorithm]" << std::endl;
    std::cout << "Access count -> Number of items" << std::endl;
    for(int i = 1; i <= std::min(10, batch_max_reuse); i++) {
        if(batch_reuse_histogram[i] > 0) {
            std::cout << i << " times -> " << batch_reuse_histogram[i] << " items" << std::endl;
        }
    }

    std::cout << "\n=================================================\n";
}

void HDR_Tree::save_access_pattern(const std::string& filename, bool is_batch) {
    std::ofstream file(filename);

    auto& access_map = is_batch ? item_access_count_batch : item_access_count_single;

    file << access_map.size() << std::endl;

    for(auto& pair : access_map) {
        file << pair.second << std::endl;
    }

    file.close();
    std::cout << "Saved access pattern to " << filename << std::endl;
}

void HDR_Tree::analyze_from_file(const std::string& single_file, const std::string& batch_file) {
    std::cout << "\n========== Cache Access Pattern Analysis ==========\n";

    std::ifstream file1(single_file);
    if(file1.is_open()) {
        int total_items;
        file1 >> total_items;

        long total_accesses = 0;
        int reused_items = 0;
        int max_reuse = 0;
        std::map<int, int> reuse_histogram;

        for(int i = 0; i < total_items; i++) {
            int access_count;
            file1 >> access_count;
            total_accesses += access_count;
            if(access_count > 1) reused_items++;
            if(access_count > max_reuse) max_reuse = access_count;
            reuse_histogram[access_count]++;
        }
        file1.close();

        std::cout << "\n[Single-point Algorithm]" << std::endl;
        std::cout << "Total unique items: " << total_items << std::endl;
        std::cout << "Total accesses: " << total_accesses << std::endl;
        std::cout << "Items reused: " << reused_items << " ("
                  << (100.0 * reused_items / total_items) << "%)" << std::endl;
        std::cout << "Avg accesses per item: " << (double)total_accesses / total_items << std::endl;
        std::cout << "Max reuse count: " << max_reuse << std::endl;
    } else {
        std::cout << "Cannot open file: " << single_file << std::endl;
    }

    std::ifstream file2(batch_file);
    if(file2.is_open()) {
        int total_items;
        file2 >> total_items;

        long total_accesses = 0;
        int reused_items = 0;
        int max_reuse = 0;
        std::map<int, int> reuse_histogram;

        for(int i = 0; i < total_items; i++) {
            int access_count;
            file2 >> access_count;
            total_accesses += access_count;
            if(access_count > 1) reused_items++;
            if(access_count > max_reuse) max_reuse = access_count;
            reuse_histogram[access_count]++;
        }
        file2.close();

        std::cout << "\n[Batch Algorithm]" << std::endl;
        std::cout << "Total unique items: " << total_items << std::endl;
        std::cout << "Total accesses: " << total_accesses << std::endl;
        std::cout << "Items reused: " << reused_items << " ("
                  << (100.0 * reused_items / total_items) << "%)" << std::endl;
        std::cout << "Avg accesses per item: " << (double)total_accesses / total_items << std::endl;
        std::cout << "Max reuse count: " << max_reuse << std::endl;

        std::cout << "\n[Reuse Distribution - Batch Algorithm]" << std::endl;
        std::cout << "Access count -> Number of items" << std::endl;
        for(auto& pair : reuse_histogram) {
            if(pair.first <= 10 && pair.second > 0) {
                std::cout << pair.first << " times -> " << pair.second << " items" << std::endl;
            }
        }
    } else {
        std::cout << "Cannot open file: " << batch_file << std::endl;
    }

    std::cout << "\n=================================================\n";
}



/*void HDR_Tree::clear_algorithm_data(bool is_batch) {
    if (is_batch) {
        batch_access_sequence.clear();
        batch_seq_counter = 0;
        item_access_count_batch.clear();
    } else {
        single_access_sequence.clear();
        single_seq_counter = 0;
        item_access_count_single.clear();
    }
}*/

/*void HDR_Tree::save_access_sequence(const std::string& filename, bool is_batch) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    auto& sequence = is_batch ? batch_access_sequence : single_access_sequence;

    size_t size = sequence.size();
    file.write((char*)&size, sizeof(size));

    for (const auto& record : sequence) {
        file.write((char*)&record.item_id, sizeof(record.item_id));
        file.write((char*)&record.sequence_number, sizeof(record.sequence_number));
        file.write((char*)&record.user_id, sizeof(record.user_id));
    }

    file.close();
    std::cout << "Saved " << size << " access records to " << filename << std::endl;
}*/

/*void HDR_Tree::load_access_sequence(const std::string& filename, bool is_batch) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    auto& sequence = is_batch ? batch_access_sequence : single_access_sequence;
    sequence.clear();

    size_t size;
    file.read((char*)&size, sizeof(size));

    std::map<int, Item*> id_to_item;
    for (auto item : Items) {
        id_to_item[item->ID] = item;
    }

    for (size_t i = 0; i < size; i++) {
        AccessRecord record;
        file.read((char*)&record.item_id, sizeof(record.item_id));
        file.read((char*)&record.sequence_number, sizeof(record.sequence_number));
        file.read((char*)&record.user_id, sizeof(record.user_id));

        record.item_ptr = id_to_item[record.item_id];

        sequence.push_back(record);
    }

    file.close();
    std::cout << "Loaded " << size << " access records from " << filename << std::endl;
}*/



//Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block; Spare block;