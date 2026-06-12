#pragma once
#define LEAF_ORDER 300
#define ORDER 300
#define MAX_NUM 301

#include <string>
using namespace std;
#include "Item.h"
#include <vector>


struct InternalNodeData{
    double m_Keys[ORDER];      //1. Change the keys of the internodes from integer to float to fit the data type of the distances between points
    void * m_Pointers[MAX_NUM];
};

struct LeafNodeData{
    double m_Keys[LEAF_ORDER];
    Item* m_number[LEAF_ORDER];      //To change the data type of the elements in m_number
    void* m_next;
};

class bpNode{
public:
    int m_nKeyNum;
    bool m_bLeaf;
    bpNode *m_pParent{};
    bpNode()
    {
        m_nKeyNum = 0;
        m_bLeaf = false;
    }
};

class InternalNode:public bpNode{
public:
    InternalNodeData* internalNodeData;
    InternalNode(){
        internalNodeData = new InternalNodeData;
        for(double & m_Key : internalNodeData->m_Keys){
            m_Key=0;
        }
        for(int i=0;i<LEAF_ORDER;i++){
            internalNodeData->m_Pointers[i]=nullptr;
        }
    };
};

class bpLeafNode:public bpNode{
public:
    LeafNodeData* leafNodeData;
    bpLeafNode(){
        leafNodeData = new LeafNodeData;
        for(int i=0;i<ORDER;i++){
            leafNodeData->m_Keys[i]=0;
        }
        for(int i=0;i<MAX_NUM;i++){
            leafNodeData->m_number[i]= nullptr;     //To change the data type of the elements in m_number
        }
        leafNodeData->m_next = nullptr;
    };
};
class BPlusTree
{
public:
	BPlusTree();
	~BPlusTree();
private:
	bpNode * m_pRoot;
	bpNode * m_pFirst;
	bpNode * m_pLast;
public:
	bool Insert(double nKey, Item* number);     //To change the value in the key-value pair from double to item pointer
	bool Remove(double nKey);
	bool InsertKeyAndPointer(bpNode * pParent, bpNode * pOld, double nKey, bpNode * pNew);
	void PrintLeaves();
    bpLeafNode * search(double nKey);
    void PrintLayerTree();
    void get_range_item(std::vector<Item*>* pointer,  double a, double b, bool left_bool, bool right_bool, bool print_if );

    //std::vector<std::tuple<float, Item*>> Items;



};

