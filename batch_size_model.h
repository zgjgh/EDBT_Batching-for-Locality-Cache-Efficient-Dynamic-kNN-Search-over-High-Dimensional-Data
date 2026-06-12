// batch_size_model.h -- model-based selection of the batch (cluster) capacity.
#pragma once

#include <string>
#include <vector>

class HDR_Tree;

struct BatchSizeModelParams {
    double E_Nq;
    double E_RHk;
    long   N1, N2;
    double R1, R2;
    long   D;
    long   W;
    long   I;
    long   Lambda;
    std::vector<long>   dl;
    std::vector<double> Upsilon;
    double fanout;

    double Tdist_hot_D;
    double Tdist_cold_D;
    std::vector<double> Tdist_cold_dl;
    double Tdist_hot_dLambda;
    double Tminz_per_elem;
};

struct BatchSizeResult {
    long   Nc_opt;
    long   clusters_opt;
    double Eclk_opt;
    double Eclk_heuristic;
    double Eclk_capacity1;
    BatchSizeModelParams params;
};

double batch_model_cost(const BatchSizeModelParams& P, double Nc);

BatchSizeResult compute_optimal_batch_size(HDR_Tree& tree,
                                           long insertN,
                                           long fanout,
                                           const std::string& csvPath);
