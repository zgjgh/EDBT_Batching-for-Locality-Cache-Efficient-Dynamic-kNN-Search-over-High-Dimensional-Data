cmake_minimum_required(VERSION 3.12)
project(batch_knn LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


option(ENABLE_BIG_OBJECTS "Enable big object flags if needed" OFF)


option(EIGEN_SAFE_MODE "Disable Eigen static alignment to avoid crashes" OFF)

add_executable(batch_knn
        main.cpp
        Clusters.cpp DataBase.cpp HDR_Tree.cpp Item.cpp
        itemClusters.cpp itemDataBase.cpp itemHDR_Tree.cpp itemLeafNode.cpp
        itemNode.cpp itemNonLeafNode.cpp LeafNode.cpp Node.cpp NonLeafNode.cpp
        User.cpp Clustersi.cpp BPlusTree.cpp Clustersu.cpp
        userNode.cpp userLeafNode.cpp userNonLeafNode.cpp
        Littletree.cpp Cluster_origin.cpp

        # headers (for IDE indexing)
        Cluster.h Clusters.h DataBase.h HDR_Tree.h Item.h
        itemCluster.h itemClusters.h itemDataBase.h itemHDR_Tree.h
        itemLeafNode.h itemNode.h itemNonLeafNode.h LeafNode.h Node.h
        NonLeafNode.h User.h Clusteri.h Clustersi.h BPlusTree.h
        Clusteru.h Clustersu.h userNode.h userLeafNode.h userNonLeafNode.h
        Littletree.h Cluster_origin.h
        )


if(MSVC)
    target_compile_options(batch_knn PRIVATE /MP)

    target_compile_options(batch_knn PRIVATE $<$<CONFIG:Debug>:/Zi>)
    target_link_options(   batch_knn PRIVATE $<$<CONFIG:Debug>:/DEBUG>)

    if(ENABLE_BIG_OBJECTS)
        target_compile_options(batch_knn PRIVATE /bigobj)
    endif()
else()

    target_compile_options(batch_knn PRIVATE $<$<CONFIG:Debug>:-g>)


    if(ENABLE_BIG_OBJECTS AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(batch_knn PRIVATE -Wa,-mbig-obj)
    endif()
endif()


if(EIGEN_SAFE_MODE)
    target_compile_definitions(batch_knn PRIVATE
            EIGEN_MAX_ALIGN_BYTES=0
            EIGEN_DONT_ALIGN_STATICALLY=1
            )
endif()


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)


# ============================================================================
#  Second target: bench  (benchmark entry point)
#  Shares the core sources; replaces main.cpp with bench_vtune.cpp.
#  Optionally links Intel ITT for profiler task markers:
#  enable with -DBENCH_VTUNE=ON, override the path with -DVTUNE_DIR=...
# ============================================================================
add_executable(bench
        bench_vtune.cpp
        batch_size_model.cpp
        Clusters.cpp DataBase.cpp HDR_Tree.cpp Item.cpp
        itemClusters.cpp itemDataBase.cpp itemHDR_Tree.cpp itemLeafNode.cpp
        itemNode.cpp itemNonLeafNode.cpp LeafNode.cpp Node.cpp NonLeafNode.cpp
        User.cpp Clustersi.cpp BPlusTree.cpp Clustersu.cpp
        userNode.cpp userLeafNode.cpp userNonLeafNode.cpp
        Littletree.cpp Cluster_origin.cpp
        )

if(MSVC)
    target_compile_options(bench PRIVATE /MP)
    target_compile_definitions(bench PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(bench PRIVATE -g)
endif()

option(BENCH_VTUNE "Link Intel VTune ITT for the bench target" OFF)
if(NOT DEFINED VTUNE_DIR)
    set(VTUNE_DIR "C:/Program Files (x86)/Intel/oneAPI/vtune/latest")
endif()
set(ITT_INCLUDE_DIR "${VTUNE_DIR}/include")
set(ITT_LIB "${VTUNE_DIR}/lib64/libittnotify.lib")

if(BENCH_VTUNE AND EXISTS "${ITT_LIB}" AND EXISTS "${ITT_INCLUDE_DIR}/ittnotify.h")
    message(STATUS "VTune ITT found: cache markers ENABLED for target 'bench'.")
    target_compile_definitions(bench PRIVATE ENABLE_VTUNE=1)
    target_include_directories(bench PRIVATE "${ITT_INCLUDE_DIR}")
    target_link_libraries(bench PRIVATE "${ITT_LIB}")
else()
    message(STATUS "Building 'bench' WITHOUT VTune ITT markers (timing still works).")
endif()





