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

class itemDataBase
{
public:
	itemDataBase();
	~itemDataBase();

	void load(std::string FName);
	void generateItem(size_t N);
	void computePCA();
	void transformData(long dimension, bool checkDuplicates);

	void calcTransform(long level, long L);


	std::vector<std::tuple<float, Eigen::VectorXf>> PrincipalComponents;

	std::vector<Eigen::MatrixXf> T;

	std::vector<Eigen::VectorXf> D;

    std::vector<Eigen::VectorXf> general_set;

    Eigen::MatrixXf I;

    Eigen::MatrixXf matrix_user_item;

	bool verbose;

	void initTransform(long L);


    long general_num;

    long general_num_user;

    long general_num_item;

    long num_user;

    long num_item;

    long random_bit;

    void generate_general_set();

    void generate_general_set_random();

    void generate_general_set_fix();

    void generate_item_set();

    void generate_pca_matrix();

    long computeDimensionReverse(long level, long L);
    long computeDimensionForward(long level, long L);
    long computeDimension(long level, long L);

    std::vector<Eigen::MatrixXf> T_all;

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

	void generateItemRandom(size_t N);
	void generateItemFixed(size_t N);

	long dim;
};



