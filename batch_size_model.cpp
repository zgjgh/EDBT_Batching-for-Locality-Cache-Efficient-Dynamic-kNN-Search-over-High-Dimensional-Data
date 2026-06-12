// batch_size_model.cpp
#include "batch_size_model.h"

#include "HDR_Tree.h"
#include "DataBase.h"
#include "itemDataBase.h"
#include "User.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <limits>
#include <random>
#include <numeric>
#include <iostream>

static long env_long(const char* name, long dflt) {
    const char* e = std::getenv(name);
    if (!e || !*e) return dflt;
    long v = std::atol(e);
    return v > 0 ? v : dflt;
}

// ---------------------------------------------------------------------------
//  Hardware parameters (fill in for the target machine, per CPU documentation)
// ---------------------------------------------------------------------------
struct HwParams {
    // element size (bytes) and SIMD width (lanes)
    double S;
    double V;
    // instruction costs (cycles)
    double T_vsub;
    double T_vfma;
    double T_vadd;
    double T_vmin;
    double T_perm;
    double T_sqrt;
    // memory hierarchy links: 1 = L1, 2 = L2, 3 = L3, 4 = Mem
    // tau[j]: startup latency of link j (cycles); BW[j]: bandwidth (bytes/cycle)
    double tau[5];
    double BW[5];
};

static HwParams hw_default() {
    HwParams h;
    h.S = 4.0;  h.V = 16.0;
    h.T_vsub = 1.0;  h.T_vfma = 1.0;  h.T_vadd = 1.0;
    h.T_vmin = 1.0;  h.T_perm = 3.0;  h.T_sqrt = 12.0;
    h.tau[1] = 5.0;   h.tau[2] = 14.0;  h.tau[3] = 50.0;  h.tau[4] = 220.0;
    h.BW[1]  = 128.0; h.BW[2]  = 64.0;  h.BW[3]  = 32.0;  h.BW[4]  = 8.0;
    return h;
}

// T_trans(D, L_k): move a D-dimensional vector from tier k to the registers
static double hw_trans(const HwParams& h, double D, int k) {
    double worst = 0.0;
    for (int j = 1; j <= k; ++j) {
        double lat = 0.0;
        for (int i = j; i <= k; ++i) lat += h.tau[i];
        double t = D * h.S / h.BW[j] + lat;
        if (t > worst) worst = t;
    }
    return worst;
}

// T_dist(D, Loc1, Loc2)
static double hw_dist(const HwParams& h, double D, int loc1, int loc2) {
    double N = std::ceil(D / h.V);
    double red = std::ceil(std::log2(h.V)) * (h.T_perm + h.T_vadd);
    return hw_trans(h, D, loc1) + hw_trans(h, D, loc2)
         + N * (h.T_vsub + h.T_vfma) + red + h.T_sqrt;
}

// T_minz(M, Loc) per element (linear coefficient in M)
static double hw_minz_per_elem(const HwParams& h, int loc) {
    return h.S / h.BW[loc] + h.T_vmin / h.V;
}

// regularized incomplete beta function
static double bsm_betacf(double a, double b, double x) {
    const int    MAXIT = 300;
    const double EPS   = 3.0e-14;
    const double FPMIN = 1.0e-300;

    double qab = a + b, qap = a + 1.0, qam = a - 1.0;
    double c = 1.0;
    double d = 1.0 - qab * x / qap;
    if (std::fabs(d) < FPMIN) d = FPMIN;
    d = 1.0 / d;
    double h = d;
    for (int m = 1; m <= MAXIT; m++) {
        int m2 = 2 * m;
        double aa = m * (b - m) * x / ((qam + m2) * (a + m2));
        d = 1.0 + aa * d;  if (std::fabs(d) < FPMIN) d = FPMIN;
        c = 1.0 + aa / c;  if (std::fabs(c) < FPMIN) c = FPMIN;
        d = 1.0 / d;
        h *= d * c;
        aa = -(a + m) * (qab + m) * x / ((a + m2) * (qap + m2));
        d = 1.0 + aa * d;  if (std::fabs(d) < FPMIN) d = FPMIN;
        c = 1.0 + aa / c;  if (std::fabs(c) < FPMIN) c = FPMIN;
        d = 1.0 / d;
        double del = d * c;
        h *= del;
        if (std::fabs(del - 1.0) < EPS) break;
    }
    return h;
}

static double bsm_reg_inc_beta(double a, double b, double x) {
    if (x <= 0.0) return 0.0;
    if (x >= 1.0) return 1.0;
    double lnBt = std::lgamma(a + b) - std::lgamma(a) - std::lgamma(b)
                + a * std::log(x) + b * std::log1p(-x);
    double bt = std::exp(lnBt);
    if (x < (a + 1.0) / (a + b + 2.0))
        return bt * bsm_betacf(a, b, x) / a;
    else
        return 1.0 - bt * bsm_betacf(b, a, 1.0 - x) / b;
}

// cluster radius as a function of capacity, from the two calibration points
static double bsm_radius(const BatchSizeModelParams& P, double Nc) {
    if (P.R1 <= 0.0 || P.R2 <= 0.0) return 0.0;
    double gamma = std::log(P.R2 / P.R1) / std::log((double)P.N2 / (double)P.N1);
    if (gamma < 0.0) gamma = 0.0;
    return P.R1 * std::pow(Nc / (double)P.N1, gamma);
}

double batch_model_cost(const BatchSizeModelParams& P, double Nc) {
    if (Nc < 1.0) Nc = 1.0;

    double Rc    = bsm_radius(P, Nc);
    double ratio = (std::sqrt(2.0) * Rc) / (2.0 * P.E_RHk);
    double Ibeta = 0.0;
    if (ratio < 1.0) {
        double z = 1.0 - ratio * ratio;
        Ibeta = bsm_reg_inc_beta(((double)P.D + 1.0) / 2.0, 0.5, z);
    }

    double E_new_C, Pr;
    if (Ibeta < 1e-14) {
        E_new_C = P.E_Nq * Nc;
        Pr      = 0.0;
    } else {
        double one_minus_pow = -std::expm1(Nc * std::log1p(-Ibeta));
        E_new_C = P.E_Nq * one_minus_pow / Ibeta;
        Pr      = 1.0 - one_minus_pow / (Ibeta * Nc);
        if (Pr < 0.0) Pr = 0.0;
        if (Pr > 1.0) Pr = 1.0;
    }

    double Tp2p = P.Tdist_hot_D  * Nc * P.E_Nq * Pr
                + P.Tdist_cold_D * Nc * P.E_Nq * (1.0 - Pr);

    double sumU = 0.0;
    for (double u : P.Upsilon) sumU += u;
    double prunedTotal = (double)P.I - E_new_C;
    if (prunedTotal < 0.0) prunedTotal = 0.0;

    double Tc2c = 0.0, prunedSoFar = 0.0;
    for (long l = 1; l <= P.Lambda; ++l) {
        double remaining = (double)P.I - prunedSoFar;
        if (remaining < 0.0) remaining = 0.0;
        double nodeSz = (double)P.I / std::pow(P.fanout, (double)l);
        if (nodeSz < 1.0) nodeSz = 1.0;
        Tc2c += P.Tdist_cold_dl[l - 1] * (remaining / nodeSz);
        prunedSoFar += (P.Upsilon[l - 1] / sumU) * prunedTotal;
    }

    double leafSz = (double)P.I / std::pow(P.fanout, (double)P.Lambda);
    if (leafSz < 1.0) leafSz = 1.0;
    double Tp2c = Nc * (E_new_C / leafSz) * P.Tdist_hot_dLambda;

    double Tclus = ((double)P.W * (double)P.W / Nc) * P.Tdist_hot_dLambda
                 + (double)P.W * P.Tminz_per_elem * ((double)P.W / Nc);

    return ((double)P.W / Nc) * (Tp2p + Tc2c + Tp2c) + Tclus;
}

static double calib_mean_radius(HDR_Tree& tree, long Ncap, std::mt19937& rng) {
    const long numU = tree.numUsers;
    long SC = std::min(numU, 20000L);

    std::vector<long> idx((size_t)numU);
    std::iota(idx.begin(), idx.end(), 0L);
    std::shuffle(idx.begin(), idx.end(), rng);
    idx.resize((size_t)SC);

    long kc = (long)std::llround((double)numU / (double)Ncap);
    if (kc < 2) kc = 2;
    if (kc > SC / 2) kc = SC / 2;

    const long ld = (long)tree.Users[0]->center_low.size();

    Eigen::MatrixXf X(ld, SC);
    for (long i = 0; i < SC; ++i)
        X.col(i) = tree.Users[(size_t)idx[(size_t)i]]->center_low;

    Eigen::MatrixXf C = X.leftCols(kc);
    std::vector<long> assign((size_t)SC, 0L);

    for (int it = 0; it < 6; ++it) {
        long moved = 0;
        for (long i = 0; i < SC; ++i) {
            float bd = std::numeric_limits<float>::max();
            long  bj = 0;
            for (long j = 0; j < kc; ++j) {
                float d = (X.col(i) - C.col(j)).squaredNorm();
                if (d < bd) { bd = d; bj = j; }
            }
            if (assign[(size_t)i] != bj) ++moved;
            assign[(size_t)i] = bj;
        }
        Eigen::MatrixXf S = Eigen::MatrixXf::Zero(ld, kc);
        std::vector<long> cnt((size_t)kc, 0L);
        for (long i = 0; i < SC; ++i) {
            S.col(assign[(size_t)i]) += X.col(i);
            ++cnt[(size_t)assign[(size_t)i]];
        }
        for (long j = 0; j < kc; ++j)
            if (cnt[(size_t)j] > 0) C.col(j) = S.col(j) / (float)cnt[(size_t)j];
        if (moved < SC / 100) break;
    }

    const long D = (long)tree.Users[0]->center.size();
    std::vector<Eigen::VectorXf> cent((size_t)kc, Eigen::VectorXf::Zero(D));
    std::vector<long> cnt((size_t)kc, 0L);
    for (long i = 0; i < SC; ++i) {
        cent[(size_t)assign[(size_t)i]] += tree.Users[(size_t)idx[(size_t)i]]->center;
        ++cnt[(size_t)assign[(size_t)i]];
    }
    for (long j = 0; j < kc; ++j)
        if (cnt[(size_t)j] > 0) cent[(size_t)j] /= (float)cnt[(size_t)j];

    std::vector<float> rad((size_t)kc, 0.0f);
    for (long i = 0; i < SC; ++i) {
        long j = assign[(size_t)i];
        float d = (tree.Users[(size_t)idx[(size_t)i]]->center - cent[(size_t)j]).norm();
        if (d > rad[(size_t)j]) rad[(size_t)j] = d;
    }

    double sum = 0.0; long n = 0;
    for (long j = 0; j < kc; ++j)
        if (cnt[(size_t)j] >= 2) { sum += rad[(size_t)j]; ++n; }
    return n > 0 ? sum / (double)n : 0.0;
}

BatchSizeResult compute_optimal_batch_size(HDR_Tree& tree,
                                           long insertN,
                                           long fanout,
                                           const std::string& csvPath) {
    BatchSizeModelParams P;
    P.D      = (long)tree.Users[0]->center.size();
    P.W      = insertN;
    P.I      = (long)tree.slidingWindow.size();
    P.fanout = (double)fanout;

    P.Lambda = (long)tree.data_i->T.size();
    P.dl.resize((size_t)P.Lambda);
    P.Upsilon.resize((size_t)P.Lambda);
    for (long l = 0; l < P.Lambda; ++l) {
        P.dl[(size_t)l] = (long)tree.data_i->T[(size_t)l].rows();
        double s = 0.0;
        for (long j = 0; j < P.dl[(size_t)l]; ++j)
            s += (double)std::get<0>(tree.data_i->PrincipalComponents[(size_t)j]);
        P.Upsilon[(size_t)l] = s;
    }

    std::printf("\n[model] D=%ld  |W|=%ld  |I|=%ld  L=%ld\n", P.D, P.W, P.I, P.Lambda);

    // primitive costs from the hardware parameter table
    HwParams hw = hw_default();
    P.Tdist_hot_D  = hw_dist(hw, (double)P.D, 2, 1);
    P.Tdist_cold_D = hw_dist(hw, (double)P.D, 2, 4);
    P.Tdist_cold_dl.resize((size_t)P.Lambda);
    for (long l = 0; l < P.Lambda; ++l)
        P.Tdist_cold_dl[(size_t)l] = hw_dist(hw, (double)P.dl[(size_t)l], 2, 4);
    P.Tdist_hot_dLambda = hw_dist(hw, (double)P.dl[(size_t)P.Lambda - 1], 2, 1);
    P.Tminz_per_elem    = hw_minz_per_elem(hw, 2);

    long S = env_long("BSM_SAMPLES", 256);
    if (S > tree.numUsers) S = tree.numUsers;

    std::mt19937 rng(2024);
    std::vector<long> sidx((size_t)tree.numUsers);
    std::iota(sidx.begin(), sidx.end(), 0L);
    std::shuffle(sidx.begin(), sidx.end(), rng);

    double sumN = 0.0, sumR = 0.0;
    for (long i = 0; i < S; ++i) {
        auto* probe = new User(tree.data->U.row(sidx[(size_t)i]), tree.k);
        long nv = 0; float md = 0.0f;
        tree.probe_user_leaf_stats(probe, nv, md);
        sumN += (double)nv;
        sumR += (double)md;
        delete probe;
    }
    P.E_Nq  = sumN / (double)S;
    P.E_RHk = sumR / (double)S;

    P.N1 = env_long("BSM_N1", 64);
    P.N2 = env_long("BSM_N2", 512);
    if (P.N2 > insertN / 2) P.N2 = std::max(16L, insertN / 2);
    if (P.N1 >= P.N2)       P.N1 = std::max(8L,  P.N2 / 8);
    P.R1 = calib_mean_radius(tree, P.N1, rng);
    P.R2 = calib_mean_radius(tree, P.N2, rng);

    long NcMax = std::min(insertN, 50000L);
    std::vector<long> grid;
    for (double x = 2.0; x <= (double)NcMax; x *= 1.06) {
        long v = (long)std::llround(x);
        if (grid.empty() || v > grid.back()) grid.push_back(v);
    }
    if (grid.back() != NcMax) grid.push_back(NcMax);

    FILE* csv = nullptr;
    if (!csvPath.empty()) {
        csv = std::fopen(csvPath.c_str(), "w");
        if (csv) std::fprintf(csv, "Nc,Eclk\n");
    }

    long   bestNc = grid.front();
    double bestC  = std::numeric_limits<double>::max();
    for (long nc : grid) {
        double c = batch_model_cost(P, (double)nc);
        if (csv) std::fprintf(csv, "%ld,%.6g\n", nc, c);
        if (c < bestC) { bestC = c; bestNc = nc; }
    }
    if (csv) std::fclose(csv);

    {
        long lo = std::max(2L, (long)(bestNc * 0.94));
        long hi = std::min(NcMax, (long)(bestNc * 1.06) + 1);
        for (long nc = lo; nc <= hi; ++nc) {
            double c = batch_model_cost(P, (double)nc);
            if (c < bestC) { bestC = c; bestNc = nc; }
        }
    }

    BatchSizeResult R;
    R.params         = P;
    R.Nc_opt         = bestNc;
    R.clusters_opt   = std::max(1L, (long)std::llround((double)insertN / (double)bestNc));
    R.Eclk_opt       = bestC;
    R.Eclk_heuristic = batch_model_cost(P, std::max(1.0, (double)insertN / 150.0));
    R.Eclk_capacity1 = batch_model_cost(P, 1.0);
    return R;
}
