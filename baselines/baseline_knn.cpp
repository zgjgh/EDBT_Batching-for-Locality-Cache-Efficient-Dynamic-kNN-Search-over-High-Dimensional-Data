// ============================================================================
//  baseline_knn.cpp -- exact kNN baselines (nanoflann KD-tree + FAISS
//  IndexFlatL2), run on the data exported by the bench tool (BENCH_DUMP=1):
//      <key>_I.txt : reference set     (one point per line, space floats)
//      <key>_U.txt : inserted query set
//  k = 10, Euclidean. Index construction on I is not timed; the query phase
//  over U is timed (one-by-one and, for FAISS, also batch).
//  Prints the sum over all queries of the k-th NN distance for verification.
//
//  BUILD
//    nanoflann only (header-only):
//      g++ -O3 -std=c++17 -fopenmp baseline_knn.cpp -o baseline_nano \
//          -I/path/to/nanoflann
//    with FAISS:
//      g++ -O3 -std=c++17 -fopenmp -DUSE_FAISS baseline_knn.cpp -o baseline_all \
//          -I/path/to/nanoflann -I$FAISS_PREFIX/include \
//          -L$FAISS_PREFIX/lib -lfaiss
//
//  RUN
//      baseline_nano <key>      # expects <key>_I.txt and <key>_U.txt
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <limits>
#include <iostream>

#include "nanoflann.hpp"

#ifdef USE_FAISS
#include <faiss/IndexFlat.h>
#include <omp.h>
#endif

using Clock = std::chrono::steady_clock;
static double secsSince(Clock::time_point t0) {
    return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - t0).count() / 1e6;
}

static const int K = 10;

// --------------------------------------------------------------------------
//  Plain-text loader: one point per line, space-separated floats.
//  Returns flat row-major array; sets n (rows) and dim (cols).
//  Reads the whole file with one fread() and scans it in place with strtof().
// --------------------------------------------------------------------------
static std::vector<float> loadMatrix(const std::string& fn, long& n, long& dim) {
    FILE* f = fopen(fn.c_str(), "rb");                 // binary: no CRLF translation
    if (!f) { fprintf(stderr, "cannot open %s\n", fn.c_str()); exit(1); }

    // ---- file size ----
    fseek(f, 0, SEEK_END);
    long long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsize <= 0) { fprintf(stderr, "empty/bad file %s\n", fn.c_str()); exit(1); }

    // ---- slurp whole file into one buffer (+1 for NUL terminator) ----
    std::vector<char> buf((size_t)fsize + 1);
    size_t got = fread(buf.data(), 1, (size_t)fsize, f);
    fclose(f);
    buf[got] = '\0';

    // ---- determine dim from the first line ----
    //  Count whitespace-separated tokens on the first line ONLY. We must stop
    //  at '\n' manually: strtof() treats '\n' as leading whitespace and would
    //  happily run past the first line into the rest of the file.
    dim = 0;
    {
        const char* p = buf.data();
        bool in_tok = false;
        while (*p && *p != '\n') {
            char ch = *p;
            bool sp = (ch == ' ' || ch == '\t' || ch == '\r');
            if (!sp && !in_tok) { ++dim; in_tok = true; }
            else if (sp)        { in_tok = false; }
            ++p;
        }
    }
    if (dim <= 0) { fprintf(stderr, "bad dim in %s\n", fn.c_str()); exit(1); }

    // ---- scan the whole buffer for floats (single pass, no realloc) ----
    std::vector<float> data;
    data.reserve((size_t)(fsize / 3));                 // safe over-reserve
    const char* p = buf.data();
    char* e;
    while (*p) {
        float v = strtof(p, &e);
        if (e == p) { ++p; continue; }                 // skip non-numeric byte
        data.push_back(v);
        p = e;
    }

    n = (long)data.size() / dim;
    return data;
}

// --------------------------------------------------------------------------
//  nanoflann dataset adaptor over a flat row-major float array.
// --------------------------------------------------------------------------
struct MatAdaptor {
    const float* pts; long n; long dim;
    inline size_t kdtree_get_point_count() const { return (size_t)n; }
    inline float  kdtree_get_pt(const size_t idx, const size_t d) const { return pts[idx * dim + d]; }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dataset_key>\n  expects <key>_I.txt and <key>_U.txt\n", argv[0]);
        return 1;
    }
    std::string key = argv[1];

    long nI, dimI, nU, dimU;
    std::vector<float> I = loadMatrix(key + "_I.txt", nI, dimI);
    std::vector<float> U = loadMatrix(key + "_U.txt", nU, dimU);
    if (dimI != dimU) { fprintf(stderr, "dim mismatch I=%ld U=%ld\n", dimI, dimU); return 1; }
    long dim = dimI;
    printf("=== baseline_knn ===\n");
    printf("dataset = %s   dim = %ld\n", key.c_str(), dim);
    printf("reference I = %ld points,  query U = %ld points,  k = %d\n\n", nI, nU, K);

    // ======================================================================
    //  nanoflann KD-tree (static; I is fixed so build the tree ONCE)
    // ======================================================================
    {
        using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
            nanoflann::L2_Simple_Adaptor<float, MatAdaptor>, MatAdaptor, -1, long>;
        MatAdaptor adIa{ I.data(), nI, dim };

        auto tb = Clock::now();
        KDTree tree((int)dim, adIa, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        tree.buildIndex();
        double t_build = secsSince(tb);

        std::vector<long>  idx(K);
        std::vector<float> d2(K);   // squared distances (L2_Simple returns squared)

        // ---- query U one by one ----
        double sum_dknn = 0.0;
        auto t1 = Clock::now();
        for (long q = 0; q < nU; ++q) {
            nanoflann::KNNResultSet<float, long> rs(K);
            rs.init(idx.data(), d2.data());
            tree.findNeighbors(rs, &U[q * dim], nanoflann::SearchParameters());
            sum_dknn += std::sqrt(d2[K - 1]);   // k-th NN Euclidean dist (sqrt of squared)
        }
        double t_query = secsSince(t1);

        printf("[nanoflann KD-tree]\n");
        printf("  build I tree (not counted) : %.4f s\n", t_build);
        printf("  query U one-by-one         : %.4f s   (%.2f us/query)\n",
               t_query, t_query * 1e6 / nU);
        printf("  sum of dknn (Euclidean)    : %.6g\n\n", sum_dknn);
    }

    // ======================================================================
    //  FAISS IndexFlatL2 (brute force exact; build once, batch + per-query)
    // ======================================================================
#ifdef USE_FAISS
    {
        omp_set_num_threads(1);   // single-thread
        faiss::IndexFlatL2 index((int)dim);

        auto tb = Clock::now();
        index.add(nI, I.data());          // "build" = add all reference points
        double t_build = secsSince(tb);

        std::vector<faiss::idx_t> labels((size_t)nU * K);
        std::vector<float>        dist2((size_t)nU * K);  // squared L2

        // ---- batch search all U at once ----
        auto t3 = Clock::now();
        index.search(nU, U.data(), K, dist2.data(), labels.data());
        double t_batch = secsSince(t3);

        double sum_dknn = 0.0;
        for (long q = 0; q < nU; ++q)
            sum_dknn += std::sqrt(dist2[(size_t)q * K + (K - 1)]);

        // ---- query U one by one ----
        auto t1 = Clock::now();
        std::vector<faiss::idx_t> lab1(K);
        std::vector<float>        d1(K);
        for (long q = 0; q < nU; ++q)
            index.search(1, &U[q * dim], K, d1.data(), lab1.data());
        double t_query = secsSince(t1);

        printf("[FAISS IndexFlatL2  (1 thread)]\n");
        printf("  build I index (not counted): %.4f s\n", t_build);
        printf("  batch search all U         : %.4f s   (%.2f us/query)\n",
               t_batch, t_batch * 1e6 / nU);
        printf("  query U one-by-one         : %.4f s   (%.2f us/query)\n",
               t_query, t_query * 1e6 / nU);
        printf("  sum of dknn (Euclidean)    : %.6g\n\n", sum_dknn);
    }
#else
    printf("[FAISS] skipped (compile with -DUSE_FAISS on the Linux server)\n");
#endif

    return 0;
}
