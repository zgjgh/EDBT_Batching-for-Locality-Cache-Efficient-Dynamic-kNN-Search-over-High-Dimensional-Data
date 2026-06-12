# batchkNN — Build & Run (Linux)

C++ implementation of batch kNN Search over high-dimensional dynamic data,
including the batch-size cost model and exact-search baselines.

---

## 1) Build

### CMake (out-of-source)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
```

This produces two executables in `build/`:
- `batch_knn` — the original entry point
- `bench` — the benchmark entry point (controlled splits, batch-size model)

### Direct g++ (bench only)

```bash
g++ -O3 -std=c++14 -I. \
    bench_vtune.cpp batch_size_model.cpp \
    Clusters.cpp DataBase.cpp HDR_Tree.cpp Item.cpp \
    itemClusters.cpp itemDataBase.cpp itemHDR_Tree.cpp itemLeafNode.cpp \
    itemNode.cpp itemNonLeafNode.cpp LeafNode.cpp Node.cpp NonLeafNode.cpp \
    User.cpp Clustersi.cpp BPlusTree.cpp Clustersu.cpp \
    userNode.cpp userLeafNode.cpp userNonLeafNode.cpp \
    Littletree.cpp Cluster_origin.cpp \
    -o bench
```

Requirements: Linux, CMake ≥ 3.12 (for the CMake route), a C++14 compiler.
Eigen and nanoflann are bundled; no external dependencies.

---

## 2) Datasets

Plain text, one vector per line, space-separated floats. Place dataset files
under `./datasets/`, or pass an explicit file path on the command line.
The datasets (NUS-WIDE, Audio, CIFAR, SUN, Fashion-MNIST, Trevi, Enron, ...)
are available at:
https://github.com/DBAIWangGroup/nns_benchmark

---

## 3) Usage — batch_knn

```bash
./batch_knn <dataset_path> <single|batch>
```

Example:
```bash
./batch_knn ../datasets/Normalized_WT.dat batch
```

---

## 4) Usage — bench

```bash
./bench <dataset_key> <single|batch|model> [dataset_path]
```

- `dataset_key` ∈ { nuswide, audio, cifar, sun, fashion, trevi, smoke } —
  selects a built-in configuration (dimension, tree parameters, and the
  initial-query / inserted-query / reference-set split).
- `single` — point-wise Δ-Tree search over the inserted queries.
- `batch`  — batch search. The cluster count defaults to 150
  (capacity |W|/150); override it with the `BENCH_CLUSTERS` environment
  variable.
- `model`  — batch-size cost model: estimates the model parameters, selects
  the batch capacity minimizing the predicted cost, prints the recommended
  cluster count, and writes the predicted cost curve to
  `<key>_model_curve.csv`.
- `dataset_path` overrides the built-in default `./datasets/<file>`.

Typical workflow:

```bash
./bench nuswide model ./datasets/Normalized_WT.dat
# -> prints: optimal batch capacity, recommended cluster count (e.g. 180)

BENCH_CLUSTERS=180 ./bench nuswide batch ./datasets/Normalized_WT.dat
./bench nuswide single ./datasets/Normalized_WT.dat
```

Each timed run reports the wall-clock time of the inserted-query kNN
maintenance and the sum of k-th NN distances over the inserted queries
("sum of dknn"), which can be compared across methods to verify the results.

### Cost-model parameters

- **Hardware parameters** (SIMD instruction costs, cache/memory link
  latencies and bandwidths) are set in `hw_default()` in
  `batch_size_model.cpp`. Edit them for the target machine per its CPU
  documentation; the provided values are examples.
- **Data-dependent parameters** are estimated automatically by sampling:
  the expected number of reference points visited per query and the radius
  of the visited region are measured on a random sample of the initial
  query set, and the cluster-radius/capacity relation is calibrated by
  clustering the initial query set at two capacities.
- Environment variables: `BSM_SAMPLES` (probe sample size, default 256),
  `BSM_N1`, `BSM_N2` (calibration capacities, default 64 and 512).

---

## 5) Baselines (nanoflann KD-tree, FAISS IndexFlatL2)

Export the exact reference/query data, then run the baselines on it:

```bash
BENCH_DUMP=1 ./bench nuswide single ./datasets/Normalized_WT.dat
# writes nuswide_I.txt and nuswide_U.txt

# nanoflann only (no dependencies)
g++ -O3 -std=c++17 baselines/baseline_knn.cpp -o baseline_nano -Ibaselines/nanoflann
./baseline_nano nuswide

# nanoflann + FAISS (FAISS installed, e.g. conda install -c pytorch faiss-cpu)
g++ -O3 -std=c++17 -fopenmp -DUSE_FAISS baselines/baseline_knn.cpp -o baseline_all \
    -Ibaselines/nanoflann -I"$CONDA_PREFIX/include" -L"$CONDA_PREFIX/lib" -lfaiss \
    -Wl,-rpath,"$CONDA_PREFIX/lib"
OMP_NUM_THREADS=1 ./baseline_all nuswide
```

The baselines print the same "sum of dknn" metric for verification.
