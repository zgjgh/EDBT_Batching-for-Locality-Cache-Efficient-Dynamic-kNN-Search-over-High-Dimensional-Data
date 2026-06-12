#pragma once

#include<string>

#include<vector>
#include <set>

#include <chrono>
#include <random>

//#include "Eigen//SVD"

#include "Eigen//Dense"

#include <iostream>

typedef const Eigen::Ref<const Eigen::VectorXf>& VectorRef;

class DataBase
{
public:
	DataBase();
	~DataBase();

	void load(std::string FName);
	void generateUser(size_t N);
	void computePCA();
	void transformData(long dimension, bool checkDuplicates);

	void calcTransform(long level, long L);

	std::vector<std::tuple<float, Eigen::VectorXf>> PrincipalComponents;

	std::vector<Eigen::MatrixXf> T;

	std::vector<Eigen::MatrixXf> T_all;

	std::vector<Eigen::VectorXf> D;

    std::vector<Eigen::VectorXf> general_set;

    Eigen::MatrixXf U;

    Eigen::MatrixXf matrix_user_item;

	bool verbose;

	void initTransform(long L);

    long dim;

    long general_num;

    long general_num_user;

    long num_user;

    long num_item;

    long random_bit;

    void generate_general_set();

    void generate_general_set_random();

    void generate_general_set_fix();

    void generate_user_set();

    void generate_pca_matrix();

    void generate_low_pcamatrix(long l_d);

    Eigen::MatrixXf Mat_low_d;


    std::vector<float> propartion_variance;

    long computeDimensionReverse(long level, long L);
    long computeDimensionForward(long level, long L);
    long computeDimension(long level, long L);

    void calculate_transform_all();

private:
	std::vector<float> data;

	float SumEigenValues;

	bool valid;

	Eigen::MatrixXf Y;



	Eigen::VectorXf solveLinearSystem(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::VectorXf Ref);
	Eigen::VectorXf getEigenVector(float eigenVal, Eigen::VectorXf Ref);

	float dist(VectorRef A, VectorRef B);

	void calculateCovariance();

	void generateUserRandom(size_t N);
	void generateUserFixed(size_t N);


};

