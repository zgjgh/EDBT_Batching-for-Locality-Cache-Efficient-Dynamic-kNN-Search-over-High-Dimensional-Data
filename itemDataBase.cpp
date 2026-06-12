#include "itemDataBase.h"
#include "Item.h"

itemDataBase::itemDataBase() {
	valid = false;
}

itemDataBase::~itemDataBase() {

	data.clear();
	data.shrink_to_fit();
}

void itemDataBase::load(std::string FName) {

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

	printf("Read Data set in %i minuites, %.3f seconds \n", int(diff / 60), diff - 60.0f * int(diff / 60.0f));
}

void itemDataBase::generateItemRandom(size_t N) {
    std::cout<<"Start generate items randomly"<<std::endl;
	size_t Limit = D.size();

	if (N > Limit) {
		return;
	}

	/* Seed */
	std::random_device rd;

	/* Random number generator */
	std::default_random_engine generator(rd());

	/* Distribution on which to apply the generator */
	std::uniform_int_distribution<long unsigned> distribution(0, Limit - 1);

	std::set<long unsigned> itemList;

	while (itemList.size() < N)
	{
		itemList.insert(distribution(generator));
	}
    std::cout<<"Start resize I(random mode)"<<std::endl;
	I.resize(N, D[0].size());
    std::cout<<"Successfully complete resizing I(random mode)"<<std::endl;
	// Iterate over all elements of set
	// using range based for loop
	int k = 0;
	for (auto index : itemList)
	{
		I.row(k) = D[index];
		k = k + 1;
	}
}

void itemDataBase::generate_general_set_random(){
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


void itemDataBase::generateItemFixed(size_t N) {

	I.resize(N, D[0].size());

	int k = 0;
	for (long index = 0; index < N; index++)
	{

		I.row(k) = D[index];
		k = k + 1;
	}
}

void itemDataBase::generate_general_set_fix(){
    size_t Limit = D.size();

    if ( general_num> Limit) {
        std::cout<<"The size of user set set is larger than the cell of the number of users that the data set can produced"<<std::endl;
        return;
    }

    int i;
    for(i=0; i<=general_num-1; i++){
        general_set.emplace_back(D[i]);
    }

}


void itemDataBase::generateItem(size_t N) {

	generateItemRandom(N);
	/*generateItemFixed(N);*/

	if (verbose) {

		std::cout << "Items" << std::endl;
		std::cout << I << std::endl << std::endl;
	}
    std::cout<<"Successful generation of items"<<std::endl;
}

void itemDataBase::generate_general_set(){
    if (random_bit==0){
        generate_general_set_fix();
    }
    else{

        //std::cout<<"Check_0"<<std::endl;

        generate_general_set_random();
    }
}

void itemDataBase::generate_item_set(){

    int i;

    I.resize(general_num_item,D[0].size());

    for(i=general_num_user;i<=general_num_user+general_num_item-1;i++){
        I.row(i-general_num_user) = general_set[i];
    }
}

void itemDataBase::generate_pca_matrix(){
    matrix_user_item.resize(num_user+num_item,D[0].size());
    int i;
    for(i=0;i<=num_user-1;i++){
        matrix_user_item.row(i)=general_set[i];
    }
    for(i=general_num_user;i<=general_num_user+num_item-1;i++){
        matrix_user_item.row(i-(general_num_user-num_user))=general_set[i];
    }
};

void itemDataBase::transformData(long dimension, bool checkDuplicates) {

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



	if (verbose) {
		std::cout << std::endl << "Data: " << std::endl;
		for (auto dat : D) {
			std::cout << dat.transpose() << std::endl;
		}
		std::cout << std::endl << std::endl;

	}

	data.clear();
	data.shrink_to_fit();
    printf("Transformed %u numbers to %u records\n", data.size(), D.size());
}

/*Eigen::VectorXf itemDataBase::solveLinearSystem(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::VectorXf Ref) {

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

/*Eigen::VectorXf itemDataBase::getEigenVector(float eigenVal, Eigen::VectorXf Ref) {

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

void itemDataBase::computePCA() {

	calculateCovariance();

	std::cout<<"Successful generation of corvariance matrix"<<std::endl;

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
		[&](const std::tuple<float, Eigen::VectorXf>& a, const std::tuple<float, Eigen::VectorXf>& b) -> bool {
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
				error = error + abs(eVal - vectorDivide(i, 0));
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

void itemDataBase::calculate_transform_all(){

    Eigen::MatrixXf Mat;

    int i;

    int j;

    for(i=1; i<=I.cols(); i++){
        Mat.resize(i, I.cols());
        for (j = 0; j <=i-1 ; j++) {
            Mat.row(j) = std::get<1>(PrincipalComponents[j]);
        }
        T_all.emplace_back(Mat);
    }
};

void itemDataBase::calculateCovariance() {


	Eigen::MatrixXf centered = matrix_user_item.rowwise() - matrix_user_item.colwise().mean();
	Y = (centered.adjoint() * centered) / float(matrix_user_item.rows());

	if (verbose) {
		std::cout << "U " << I.rows() << "x" << I.cols() << std::endl;

		std::cout << "Average " << I.colwise().mean() << std::endl;

		std::cout << "Co-variance" << std::endl;
		std::cout << Y << std::endl << std::endl;

		//		Y = (centered.transpose() * centered) / float(U.rows() - 1);

		//		std::cout << "Co-variance" << std::endl;
		//		std::cout << Y << std::endl << std::endl;
	}

}

long itemDataBase::computeDimensionReverse(long level, long L) {
	return computeDimensionForward(L - level, L);
}
long itemDataBase::computeDimensionForward(long level, long L) {

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

	return Dl;
}
long itemDataBase::computeDimension(long level, long L) {
	return computeDimensionForward(level, L);
	//	return computeDimensionReverse(level, L);
}

void itemDataBase::calcTransform(long level, long L) {
	auto dim = computeDimension(level, L);

	//	std::cout << "Level " << level << " Dimension " << dim << " Features " << D[0].size() << std::endl;
	//	std::cout << "T " << T.rows() << "x" << T.cols() << std::endl;

	Eigen::MatrixXf Mat;

	Mat.resize(dim, D[0].size());

	for (int i = 0; i < dim; i++) {
		Mat.row(i) = std::get<1>(PrincipalComponents[i]);
	}

	T.emplace_back(Mat);

	//	std::cout << "Transform " << transform.rows() << "x" << transform.cols() << std::endl;
}

float itemDataBase::dist(VectorRef A, VectorRef B) {

	if ((A.rows() != B.rows()) ||
		(A.cols() != B.cols())) {
		std::cout << "Error : float itemDataBase::dist(VectorRef A, VectorRef B) : Dimension Mismatch" << std::endl;
		std::cout << " A : " << A.rows() << "x" << A.cols() << std::endl;
		std::cout << " B : " << B.rows() << "x" << B.cols() << std::endl;
	}

	float res = (A - B).norm();
	return res;
}

void itemDataBase::initTransform(long L) {
	for (long i = 1; i <= L; i++) {
		calcTransform(i, L);
	}
}
