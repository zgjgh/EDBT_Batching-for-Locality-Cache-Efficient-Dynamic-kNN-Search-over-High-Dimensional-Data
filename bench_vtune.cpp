// ============================================================================
//  bench_vtune.cpp -- benchmark entry point.
//
//  Runs one method (single | batch | model) on one dataset with controlled
//  initial-query / inserted-query / reference-set sizes, and times only the
//  kNN maintenance of the inserted queries.
//
//  Usage:   bench <dataset_key> <single|batch|model> [dataset_path]
//
//  Optional ITT task markers wrap the timed region for profiler attribution;
//  build with -DENABLE_VTUNE to activate them.
// ============================================================================

#include <cstdlib>
#include <cstdio>
#include <string>
#include <chrono>
#include <iostream>

#include "DataBase.h"
#include "HDR_Tree.h"
#include "itemDataBase.h"
#include "itemHDR_Tree.h"
#include "batch_size_model.h"

#ifdef ENABLE_VTUNE
#include <ittnotify.h>
static __itt_domain*        g_itt_dom    = __itt_domain_create("Distance_Calculation");
static __itt_string_handle* g_itt_single = __itt_string_handle_create("Single_Distance");
static __itt_string_handle* g_itt_batch  = __itt_string_handle_create("Batch_Distance");
  #define ITT_BEGIN_SINGLE() __itt_task_begin(g_itt_dom, __itt_null, __itt_null, g_itt_single)
  #define ITT_END_SINGLE()   __itt_task_end(g_itt_dom)
  #define ITT_BEGIN_BATCH()  __itt_task_begin(g_itt_dom, __itt_null, __itt_null, g_itt_batch)
  #define ITT_END_BATCH()    __itt_task_end(g_itt_dom)
#else
  #define ITT_BEGIN_SINGLE()
  #define ITT_END_SINGLE()
  #define ITT_BEGIN_BATCH()
  #define ITT_END_BATCH()
#endif

static DataBase      g_userDB;
static itemDataBase  g_itemDB;
static HDR_Tree      g_tree;

// ---------------------------------------------------------------------------
//  Per-dataset configuration.
//    general_size      = experiment universe drawn from the file
//    general_user_set  = query pool size
//    general_item_set  = reference pool size
//    initial_user_set  = initial query set size
//    insertion_number  = number of inserted queries
//    initial_item_set  = reference set size
// ---------------------------------------------------------------------------
struct DatasetCfg {
    const char* key;
    const char* path;        // default path; can be overridden by argv[3]
    long dimensions;
    long fanout;
    long threshold;
    long general_size;
    long general_user_set;
    long general_item_set;
    long initial_user_set;
    long insertion_number;
    long initial_item_set;
};

static const DatasetCfg kConfigs[] = {
    {  "trevi",   "./datasets/trevi.txt",
       4096, 8, 30,   99900, 49950, 49950, 24975, 24975, 49950 },

    {  "nuswide", "./datasets/Normalized_WT.dat",
       128, 10, 30,   269648, 134824, 67412, 67412, 67412, 67412 },

    {  "cifar",   "./datasets/cifar.txt",
       512, 20, 30,   50000, 25000, 12500, 12500, 12500, 12500 },

    {  "audio",   "./datasets/audio.txt",
       192, 20, 30,   53387, 26692, 13346, 13346, 13346, 13346 },

    {  "sun",     "./datasets/sun.txt",
       512, 10, 30,   79106, 39552, 19776, 19776, 19776, 19776 },

    {  "fashion", "./datasets/fashion-mnist-784-euclidean.txt",
       784, 15, 20,   60000, 30000, 15000, 15000, 15000, 15000 },

    // small configuration for quick functional checks
    {  "smoke",   "./datasets/Normalized_WT.dat",
       128, 10, 30,   8000, 4000, 4000, 2000, 2000, 4000 },
};

static const DatasetCfg* findConfig(const std::string& key) {
    for (const auto& c : kConfigs) if (key == c.key) return &c;
    return nullptr;
}

// ---------------------------------------------------------------------------
//  Build both trees + the batch-clustering setup (not timed).
// ---------------------------------------------------------------------------
static long buildTrees(const DatasetCfg& cfg, const char* path) {
    // ---------- reference side : the delta-tree ----------
    g_itemDB.load(path);
    g_itemDB.verbose = false;

    g_tree.numFeatures_i     = cfg.dimensions;
    g_tree.checkDuplicates_i = false;
    g_tree.numItems_i        = cfg.initial_item_set;
    g_tree.verbose_i         = false;
    g_tree.setData_i(&g_itemDB);

    g_tree.data_i->general_num      = cfg.general_size;
    g_tree.data_i->general_num_item = cfg.general_item_set;
    g_tree.data_i->general_num_user = cfg.general_user_set;
    g_tree.data_i->num_user         = cfg.initial_user_set;
    g_tree.data_i->num_item         = cfg.initial_item_set;
    g_tree.data_i->random_bit       = 1;

    long delta_dim = g_tree.construct_i(cfg.fanout, cfg.threshold);
    std::cout << "\n - - - Finished constructing Item (delta) tree - - - \n" << std::endl;

    // ---------- query side : the HDR-tree ----------
    g_userDB.load(path);
    g_userDB.verbose = false;

    g_tree.k               = 10;
    g_tree.windowSize      = cfg.initial_item_set;
    g_tree.numFeatures     = cfg.dimensions;
    g_tree.checkDuplicates = false;
    g_tree.numUsers        = cfg.initial_user_set;
    g_tree.verbose         = false;
    g_tree.setData(&g_userDB);

    g_tree.data->general_num      = cfg.general_size;
    g_tree.data->general_num_user = cfg.general_user_set;
    g_tree.data->num_user         = cfg.initial_user_set;
    g_tree.data->num_item         = cfg.initial_item_set;
    g_tree.data->random_bit       = 1;

    g_tree.construct(10, 50);
    std::cout << "\n - - - Finished constructing User (HDR) tree - - - \n" << std::endl;

    // ---------- batch-clustering setup ----------
    // cluster count: default 150, override with BENCH_CLUSTERS
    long nClusters = 150;
    if (const char* e = std::getenv("BENCH_CLUSTERS")) {
        long v = std::atol(e);
        if (v > 0) nClusters = v;
    }
    std::cout << "form_clusters_user: " << nClusters << " clusters" << std::endl;
    g_tree.form_clusters_user(delta_dim, nClusters);
    g_tree.calculate_distance_itree_ucluster();
    g_tree.userclusters_false();
    g_tree.usercluster_radius_zero();
    g_tree.usercluster_updated_user_clear();
    g_tree.clear_clusters_haveuserknn();

    g_tree.data->generate_low_pcamatrix(delta_dim);
    for (auto user : g_tree.Users)         user->center_low = g_tree.data->Mat_low_d * user->center;
    for (auto item : g_tree.slidingWindow) item->v_low      = g_tree.data->Mat_low_d * item->v;

    std::cout << "\n - - - Finished batch-clustering setup - - - \n" << std::endl;
    return delta_dim;
}

// sum of dknn over all queries (initial + inserted)
static double sumDknnAll() {
    double s = 0.0;
    for (auto u : g_tree.Users) s += u->dknn;
    return s;
}

// sum of dknn over the inserted queries only
static double sumDknn() {
    double s = 0.0;
    for (size_t i = (size_t)g_tree.numUsers; i < g_tree.Users.size(); ++i)
        s += g_tree.Users[i]->dknn;
    return s;
}

// ---------------------------------------------------------------------------
//  Optional data export (BENCH_DUMP=1): writes the exact reference set I and
//  inserted query set U as plain text, for running the external baselines on
//  identical data.
//    <key>_I.txt : reference set
//    <key>_U.txt : inserted query set
// ---------------------------------------------------------------------------
static void dumpIfRequested(const DatasetCfg& cfg) {
    const char* flag = std::getenv("BENCH_DUMP");
    if (!flag || std::string(flag) != "1") return;

    const long dim = cfg.dimensions;

    {
        std::string fn = std::string(cfg.key) + "_I.txt";
        FILE* f = fopen(fn.c_str(), "w");
        if (!f) { printf("DUMP: cannot open %s\n", fn.c_str()); return; }
        long n = 0;
        for (auto it : g_tree.slidingWindow) {
            const Eigen::VectorXf& v = it->v;
            for (long j = 0; j < dim; ++j)
                fprintf(f, j + 1 < dim ? "%.9g " : "%.9g\n", v(j));
            ++n;
        }
        fclose(f);
        printf("DUMP: wrote %ld reference points (dim %ld) to %s\n", n, dim, fn.c_str());
    }

    {
        std::string fn = std::string(cfg.key) + "_U.txt";
        FILE* f = fopen(fn.c_str(), "w");
        if (!f) { printf("DUMP: cannot open %s\n", fn.c_str()); return; }
        const long start = g_tree.numUsers;
        const long end   = start + cfg.insertion_number;
        for (long i = start; i < end; ++i) {
            for (long j = 0; j < dim; ++j)
                fprintf(f, j + 1 < dim ? "%.9g " : "%.9g\n", g_tree.data->U(i, j));
        }
        fclose(f);
        printf("DUMP: wrote %ld inserted queries (dim %ld) to %s\n",
               cfg.insertion_number, dim, fn.c_str());
    }
}

static const char* kMethods[] = {
    "single", "batch", "parallel", "scan", "idistance", "model"
};

static bool isValidMethod(const std::string& m) {
    for (const char* s : kMethods) if (m == s) return true;
    return false;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <dataset_key> <single|batch|parallel|scan|idistance|model> [dataset_path]\n"
                  << "  dataset_key in { trevi , nuswide , cifar , audio , sun , fashion , smoke }\n"
                  << "  single    : point-wise delta-tree search\n"
                  << "  batch     : batch search (cluster count: 150 or BENCH_CLUSTERS)\n"
                  << "  parallel  : batch search with parallel cluster processing\n"
                  << "  scan      : sequential scan over the reference set\n"
                  << "  idistance : iDistance index search\n"
                  << "  model     : batch-size cost model (writes <key>_model_curve.csv)\n"
                  << "  dataset_path (optional) overrides the built-in default path\n";
        return 1;
    }
    std::string dsKey  = argv[1];
    std::string method = argv[2];

    const DatasetCfg* cfg = findConfig(dsKey);
    if (!cfg) { std::cerr << "Unknown dataset_key '" << dsKey << "'.\n"; return 1; }
    if (!isValidMethod(method)) {
        std::cerr << "Unknown method '" << method << "'.\n"; return 1;
    }
    const char* path = (argc >= 4) ? argv[3] : cfg->path;

    std::cout << "=== bench ===\n"
              << "dataset = " << cfg->key  << "  (dim " << cfg->dimensions << ")\n"
              << "path    = " << path      << "\n"
              << "method  = " << method    << "\n"
              << "initialU= " << cfg->initial_user_set
              << "  insertU= " << cfg->insertion_number
              << "  fixedI= "  << cfg->initial_item_set << "\n" << std::endl;

    buildTrees(*cfg, path);

    dumpIfRequested(*cfg);

    // ---- model mode: select the batch capacity ----
    if (method == "model") {
        BatchSizeResult r = compute_optimal_batch_size(
            g_tree, cfg->insertion_number, cfg->fanout,
            std::string(cfg->key) + "_model_curve.csv");

        std::cout << "\n============ BATCH-SIZE MODEL RESULT ============\n"
                  << "dataset                : " << cfg->key << "\n"
                  << "optimal batch capacity : " << r.Nc_opt << "\n"
                  << "  -> cluster count     : " << r.clusters_opt << "\n"
                  << "E(opt)      : " << r.Eclk_opt << "\n"
                  << "E(|W|/150)  : " << r.Eclk_heuristic << "\n"
                  << "E(1)        : " << r.Eclk_capacity1 << "\n"
                  << "=================================================\n";
        return 0;
    }

    // ---- iDistance setup (index construction, not timed) ----
    if (method == "idistance") {
        g_tree.form_item_clusters(60);
        g_tree.complete_tree();
    }

    printf(">>> START UPDATE  [%s]  dataset=%s  inserting %ld queries ...\n",
           method.c_str(), cfg->key, cfg->insertion_number);
    fflush(stdout);

    // ---- time only the inserted-query kNN computation ----
    auto t0 = std::chrono::steady_clock::now();
    if (method == "single") {
        ITT_BEGIN_SINGLE();
        for (long i = 0; i < cfg->insertion_number; ++i) g_tree.update_user_add_deltatree();
        ITT_END_SINGLE();
    } else if (method == "batch") {
        ITT_BEGIN_BATCH();
        g_tree.update_user_add_batch_bycluster(cfg->insertion_number);
        ITT_END_BATCH();
    } else if (method == "parallel") {
        g_tree.update_user_add_batch_bycluster_paralell(cfg->insertion_number);
    } else if (method == "scan") {
        for (long i = 0; i < cfg->insertion_number; ++i) {
            auto* u = new User(g_tree.data->U.row(g_tree.numUsers + g_tree.add_user_num), g_tree.k);
            u->computeKNN(g_tree.slidingWindow);
            g_tree.Users.emplace_back(u);
            g_tree.add_user_num += 1;
        }
    } else { // idistance
        for (long i = 0; i < cfg->insertion_number; ++i) {
            auto* u = new User(g_tree.data->U.row(g_tree.numUsers + g_tree.add_user_num), g_tree.k);
            g_tree.user_computeknn_id(u, 0.01, 0.01, false);
            g_tree.Users.emplace_back(u);
            g_tree.add_user_num += 1;
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    double sec = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1e6;

    std::cout << "\n================ RESULT ================\n"
              << "dataset       : " << cfg->key << "\n"
              << "method        : " << method   << "\n"
              << "inserted U    : " << cfg->insertion_number << "\n"
              << "update time   : " << sec << " s\n"
              << "sum of dknn (inserted U)    : " << sumDknn()    << "\n"
              << "sum of dknn (all U)         : " << sumDknnAll() << "\n"
              << "========================================\n";

    return 0;
}
