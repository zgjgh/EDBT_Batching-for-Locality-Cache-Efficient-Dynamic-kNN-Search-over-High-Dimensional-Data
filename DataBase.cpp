#include "DataBase.h"
#include "Item.h"

DataBase::DataBase() {
	valid = false;
}

DataBase::~DataBase() {

	data.clear();
	data.shrink_to_fit();
}

void DataBase::load(std::string FName) {

	printf("Reading Data set from file %s\n", FName.c_str());

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	valid = false;

	FILE* f = fopen(FName.c_str(), "r");							// open the file

	if (f == NULL) {												// go back if file was not opened
		printf("Unable to open data file for reading\n");
		return;
	}

	int ret = 1;
	float x[16];
	int i = 0;

	// read until you reach end of file
	while (ret > 0) {

		// read a value from file
		ret = fscanf(f, 
			"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
			&x[0], &x[1], &x[2], &x[3], &x[4], &x[5], &x[6], &x[7], 
			&x[8], &x[9], &x[10], &x[11], &x[12], &x[13], &x[14], &x[15]);

		// If value was read successfully
		if (ret > 0) {

			for (int n = 0; n < ret; n++) {
				data.emplace_back(x[n]);
				i++;
			}
		}
	}

	fclose(f);														// close the file

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	diff = diff / 1000.0f;

	printf("Read Data set in %i minuites, %.3f seconds \n", int(diff/60), diff - 60.0f * int(diff / 60.0f));
}

void DataBase::generateUserRandom(size_t N) {

	size_t Limit = D.size();

	if (N > Limit) {
	    std::cout<<"The size of user set set is larger than the cell of the number of users that the data set can produced"<<std::endl;
		return;
	}

	/* Seed */
	std::random_device rd;

	/* Random number generator */
	std::default_random_engine generator(rd());

	/* Distribution on which to apply the generator */
	std::uniform_int_distribution<long unsigned> distribution(0, Limit - 1);

	std::set<long unsigned> userList;

	while (userList.size() < N)
	{
		userList.insert(distribution(generator));
	}

	U.resize(N, D[0].size());

	// Iterate over all elements of set
	// using range based for loop
	int k = 0;
	for (auto index : userList)
	{
		U.row(k) = D[index];
		k = k + 1;
	}
}

void DataBase::generate_general_set_random(){
    size_t Limit = D.size();

    if (general_num > Limit) {
        std::cout<<"The size of the basic general user set plus general item set is larger than the cell dataset size that can be produced by the raw dataset"<<std::endl;
        return;
    }

    /* Seed */
    std::random_device rd;

    /* Random number generator */
    std::default_random_engine generator(rd());

    /* Distribution on which to apply the generator */
    std::uniform_int_distribution<long unsigned> distribution(0, Limit - 1);

    std::set<long unsigned> general_data_List;

    while (general_data_List.size() <general_num)
    {
        general_data_List.insert(distribution(generator));
    }



    // Iterate over all elements of set
    // using range based for loop

    for (auto index : general_data_List)
    {
        general_set.emplace_back(D[index]);

    }
}



void DataBase::generateUserFixed(size_t N) {

    size_t Limit = D.size();

    if (N > Limit) {
        std::cout<<"The size of user set set is larger than the cell of the number of users that the data set can produced"<<std::endl;
        return;
    }

	U.resize(N, D[0].size());

	int k = 0;
	for (long index = 0; index < N; index++)
	{

		U.row(k) = D[index];
		k = k + 1;
	}
}


void DataBase::generate_general_set_fix(){
    size_t Limit = D.size();

    if ( general_num> Limit) {
        std::cout<<"The size of user set set is larger than the cell of the number of users that the data set can produced"<<std::endl;
        return;
    }

    long i;
    for(i=0; i<=general_num-1; i++){
        general_set.emplace_back(D[i]);
    }

}

void DataBase::generate_general_set(){
    if (random_bit==0){
        generate_general_set_fix();
    }
    else{
        generate_general_set_random();
    }
}

void DataBase::generate_user_set(){

    int i;

    U.resize(general_num_user,D[0].size());

    for(i=0;i<=general_num_user-1;i++){
        U.row(i) = general_set[i];
    }
}

void DataBase::generate_pca_matrix(){
    matrix_user_item.resize(num_user+num_item,D[0].size());
    int i;
    for(i=0;i<=num_user-1;i++){
        matrix_user_item.row(i)=general_set[i];
    }
    for(i=general_num_user;i<=general_num_user+num_item-1;i++){
        matrix_user_item.row(i-(general_num_user-num_user))=general_set[i];
    }
};



void DataBase::generateUser(size_t N) {

	/*generateUserRandom(N);*/
	generateUserFixed(N);

	if (verbose) {

		std::cout << "Users" << std::endl;
		std::cout << U << std::endl << std::endl;
	}
}

void DataBase::transformData(long dimension, bool checkDuplicates) {

	dim = dimension;

	int N = data.size() / dimension;

	Eigen::VectorXf v;
	v.resize(dimension, 1);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < dimension; j++) {
			v(j) = data[i * dimension + j];
		}


		bool found = false;

		if (checkDuplicates) {
			for (auto item : D) {
				if (dist(item, v) < 1e-4) {
					found = true;
				}
			}

		}

		if (!found) {
			D.emplace_back(v);
		}
	}
	
	printf("Transformed %u numbers to %u records\n", data.size(), D.size());

	if (verbose) {
		std::cout << std::endl << "Data: " << std::endl;
		for (auto dat : D) {
			std::cout << dat.transpose() << std::endl;
		}
		std::cout << std::endl << std::endl;

	}

	data.clear();
	data.shrink_to_fit();

}

/*Eigen::VectorXf DataBase::solveLinearSystem(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::VectorXf Ref) {

	Eigen::VectorXd Res;

	try {
		Eigen::JacobiSVD<Eigen::MatrixXd> SVD(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
		Res = SVD.solve(-1 * B);
		return Res.cast<float>();
	}
	catch (...) {
		std::cout << "JacobiSVD failed" << std::endl;
	}

	try {
		Eigen::BDCSVD<Eigen::MatrixXd> SVD(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
		Res = SVD.solve(-1 * B);
		return Res.cast<float>();
	}
	catch (...) {
		std::cout << "BDCSVD failed" << std::endl;
	}


	return Ref;
}*/

/*Eigen::VectorXf DataBase::getEigenVector(float eigenVal, Eigen::VectorXf Ref) {

//	std::cout << "Calculating Eigen Value " << eigenVal << std::endl;

	Eigen::VectorXd result;

	Eigen::MatrixXd M = Y.cast<double>();

	Eigen::MatrixXd I = Eigen::Matrix<double, numDimensions, numDimensions>::Identity();

	Eigen::MatrixXd LI = eigenVal * I;

	Eigen::MatrixXd A_LI = M - LI;

	Eigen::MatrixXd sliceA = A_LI.block(0, 0, Y.rows(), Y.cols() - 1);
	Eigen::MatrixXd sliceB = A_LI.col(Y.cols() - 1);

	Eigen::VectorXf Res = solveLinearSystem(sliceA, sliceB, Ref);

	Res.conservativeResize(Res.rows() + 1, Res.cols());
	Res(Res.rows() - 1, 0) = 1.0;

	Res.normalize();

//	std::cout << Res.transpose() << std::endl;

	return Res.cast<float>();
}*/

void DataBase::computePCA() {

	calculateCovariance();

    Eigen::EigenSolver<Eigen::MatrixXf> solver;

    solver.compute(Y, true);

    Eigen::MatrixXcf ev_c=solver.eigenvalues();

    Eigen::MatrixXf ev=ev_c.real();

    Eigen::MatrixXcf evt_c=solver.eigenvectors();

    Eigen::MatrixXf evt=evt_c.real();

    for(int i=0; i<Y.rows();i++){
        std::tuple<float, Eigen::VectorXf> tuple_ev_evt=std::make_tuple(ev(i), evt.col(i));
        PrincipalComponents.emplace_back(tuple_ev_evt);
    }
    std::sort(PrincipalComponents.begin(), PrincipalComponents.end(),
              [&](const std::tuple<float, Eigen::VectorXf>& a, const std::tuple<float, Eigen::VectorXf >& b) -> bool {
                  return std::get<0>(a) >= std::get<0>(b);
              });



    SumEigenValues = 0.0f;
    for (auto E : PrincipalComponents) {
        SumEigenValues = SumEigenValues + std::get<0>(E);
    }

	/*if (eigensolver.info() != Eigen::Success) {
		return;
	}*/

//	if (verbose) {
//		std::cout << "Eigen Values: " << std::endl;
//		std::cout << eigensolver.eigenvalues() << std::endl << std::endl;

//		std::cout << "Eigen Vector: " << std::endl;
//		std::cout << eigensolver.eigenvectors() << std::endl << std::endl;
//	}


	/*Eigen::VectorXf eigen_values = (eigensolver.eigenvalues()).cast<float>();
	Eigen::MatrixXf eigen_vectors = (eigensolver.eigenvectors()).cast<float>();

	for (int i = 0; i < eigen_values.size(); i++) {
		auto eVec = getEigenVector(eigen_values[i], eigen_vectors.row(i));
		std::tuple<float, Eigen::VectorXf> vec_and_val(eigen_values[i], eVec);
		PrincipalComponents.push_back(vec_and_val);
	}

	std::sort(PrincipalComponents.begin(), PrincipalComponents.end(),
		[&](const std::tuple<float, Eigen::VectorXf> & a, const std::tuple<float, Eigen::VectorXf> & b) -> bool {
			return std::get<0>(a) > std::get<0>(b);
		});*/





	/*if (verbose) {
		for (auto E : PrincipalComponents) {

			auto prodMat = Y * std::get<1>(E);
			auto vectorDivide = prodMat.array() / std::get<1>(E).array();

			std::cout << "Eigen Value: " << std::get<0>(E) << std::endl
				<< " Vector: " << std::get<1>(E).transpose() << std::endl
				<< " Product: " << prodMat.transpose() << std::endl
				<< " Av/v: " << vectorDivide.transpose() << std::endl << std::endl;
		}
	}
	else {
		int i = 0;
		for (auto E : PrincipalComponents) {

			float error = 0;

			auto prodMat = Y * std::get<1>(E);
			auto vectorDivide = prodMat.array() / std::get<1>(E).array();

			float eVal = std::get<0>(E);

			for (int i = 0; i < vectorDivide.rows(); i++) {
				error = error + abs(eVal - vectorDivide(i,0));
			}

			error = 100 * error / eVal;

			std::cout << "Eigen Value [" << i << "] = " << std::get<0>(E) << ", Error = " << error << std::endl;


			i++;
		}
	}*/

	/*Eigen::EigenSolver< Eigen::MatrixXf> eigensolver(Y);

	if (eigensolver.info() != Eigen::Success) {
		return;
	}

	Eigen::VectorXf eigen_values = eigensolver.eigenvalues().real();
	Eigen::MatrixXf eigen_vectors = eigensolver.eigenvectors().real();

	for (int i = 0; i < eigen_values.size(); i++) {
		std::tuple<float, Eigen::VectorXf> vec_and_val(eigen_values[i], eigen_vectors.row(i));
		PrincipalComponents.push_back(vec_and_val);
	}

	std::sort(PrincipalComponents.begin(), PrincipalComponents.end(),
		[&](const std::tuple<float, Eigen::VectorXf>& a, const std::tuple<float, Eigen::VectorXf>& b) -> bool {
			return std::get<0>(a) > std::get<0>(b);
		});

	SumEigenValues = 0.0f;
	for (auto E : PrincipalComponents) {
		SumEigenValues = SumEigenValues + std::get<0>(E);
	}

	if (verbose) {
		for (auto E : PrincipalComponents) {
			std::cout << "Eigen Value: " << std::get<0>(E)
				<< " Vector: " << std::get<1>(E).transpose() << std::endl;
		}
	}
	else {
		int i = 0;
		for (auto E : PrincipalComponents) {
			std::cout << "Eigen Value [" << i << "] = " << std::get<0>(E) << std::endl;
			i++;
		}
	}*/



//	std::cout << std::endl << "P0: "<< std::get<1>(PrincipalComponents[0]).transpose() << std::endl << std::endl;
//	std::cout << std::endl << "P1: " << std::get<1>(PrincipalComponents[1]).transpose() << std::endl << std::endl;

//	float MaxV = 0;
//	for (int i = 0; i < std::get<1>(E).size(); i++) {
//		MaxV = std::max(MaxV, std::abs(std::get<1>(E)(i)));
//	}

//	std::cout << "Max: " << MaxV << std::endl;

}

void DataBase::calculateCovariance() {


	Eigen::MatrixXf centered = matrix_user_item.rowwise() - matrix_user_item.colwise().mean();
	Y = (centered.adjoint() * centered) / float(matrix_user_item.rows());

	if (verbose) {
		/*std::cout << "U " << U.rows() << "x" << U.cols() << std::endl;
		
		std::cout << "Average " << U.colwise().mean() << std::endl;

		std::cout << "Co-variance" << std::endl;
		std::cout << Y << std::endl << std::endl;*/

//		Y = (centered.transpose() * centered) / float(U.rows() - 1);

//		std::cout << "Co-variance" << std::endl;
//		std::cout << Y << std::endl << std::endl;
	}

}

long DataBase::computeDimensionReverse(long level, long L) {
	return computeDimensionForward(L - level, L);
}
long DataBase::computeDimensionForward(long level, long L) {

	auto D1 = 1;
	auto SumD1 = 0.0f;
	for (int j = 1; j <= D1; j++) {
		auto EigenValue = std::get<0>(PrincipalComponents[j - 1]);
		SumD1 = SumD1 + EigenValue;
	}

	float factor = (level - 1.0f) / (L - 1.0f);
	float RHS = factor * (SumEigenValues - SumD1) + SumD1;

	std::cout << "SumEigenValues " << SumEigenValues
		<< " SumD1 " << SumD1 << std::endl;

		std::cout << "RHS " << RHS
			<< " Factor " << factor << std::endl;

	int Dl;
	for (Dl = D1; Dl < dim; Dl++) {

		auto SumDl = 0.0f;
		for (int j = 1; j <= Dl; j++) {
			auto EigenValue = std::get<0>(PrincipalComponents[j - 1]);
			SumDl = SumDl + EigenValue;
		}



				std::cout << "Dimension " << Dl <<
					" EigneVal " << std::get<0>(PrincipalComponents[Dl])
					<< " Sum " << SumDl <<
					" < " << RHS << std::endl;

		if (SumDl >= RHS) {
			break;
		}
	}

	std::cout << "Level " << level << " Dimension " << Dl << std::endl << std::endl;


	if(level==L) {

        int Dl_to_full;
        float SumDl_to_full;

        for (Dl_to_full = 1; Dl_to_full <= dim; Dl_to_full++) {
            SumDl_to_full = 0.0f;
            for (int j = 1; j <= Dl_to_full; j++) {
                auto EigenValue = std::get<0>(PrincipalComponents[j - 1]);
                SumDl_to_full = SumDl_to_full + EigenValue;
            }
            std::cout<<"The sum of variance of the front "<<Dl_to_full<<" dimensions is "<<SumDl_to_full<<"; ";
            std::cout<<"The proportion of the sum of the variance of the front "<<Dl_to_full<<" dimensions in the total "<<dim<<" dimensions is "<<float(SumDl_to_full/RHS)<<std::endl;
            propartion_variance.emplace_back(float(SumDl_to_full/RHS));
        }
    }


	return Dl;
}
long DataBase::computeDimension(long level, long L) {
	return computeDimensionForward(level, L);
//	return computeDimensionReverse(level, L);
}

void DataBase::calcTransform(long level, long L) {
	auto dim = computeDimension(level, L);

//	std::cout << "Level " << level << " Dimension " << dim << " Features " << D[0].size() << std::endl;
//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;

	Eigen::MatrixXf Mat;

	Mat.resize(dim, U.cols());

	for (int i = 0; i < dim; i++) {
		Mat.row(i) = std::get<1>(PrincipalComponents[i]);
	}

	T.emplace_back(Mat);

//	std::cout << "Transform " << transform.rows() << "x" << transform.cols() << std::endl;
}

void DataBase::calculate_transform_all(){

    Eigen::MatrixXf Mat;

    int i;

    int j;

    for(i=1; i<=U.cols(); i++){
        Mat.resize(i, U.cols());
        for (j = 0; j <=i-1 ; j++) {
            Mat.row(j) = std::get<1>(PrincipalComponents[j]);
        }
        T_all.emplace_back(Mat);
    }
};

float DataBase::dist(VectorRef A, VectorRef B) {

	if ((A.rows() != B.rows()) ||
		(A.cols() != B.cols())) {
		std::cout << "Error : float DataBase::dist(VectorRef A, VectorRef B) : Dimension Mismatch" << std::endl;
		std::cout << " A : " << A.rows() << "x" << A.cols() << std::endl;
		std::cout << " B : " << B.rows() << "x" << B.cols() << std::endl;
	}

	float res = (A - B).norm();
	return res;
}

void DataBase::initTransform(long L) {
	for (long i = 1; i <= L; i++) {
		calcTransform(i, L);
	}
}


void DataBase::generate_low_pcamatrix(long l_d){
    Mat_low_d.resize(l_d, U.cols());
    for (int i = 0; i < l_d; i++) {
        Mat_low_d.row(i) = std::get<1>(PrincipalComponents[i]);
    }
};