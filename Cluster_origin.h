
#pragma once

enum class ClusterType {uCluster, iCluster};

class Cluster_origin
{
public:

    ClusterType Ctype;

    virtual ~Cluster_origin();
};