#include "BPlusTree.h"
#include <queue>
using namespace std;
#include <iostream>


BPlusTree::BPlusTree()
{
	m_pRoot = nullptr;
	m_pFirst = nullptr;
	m_pLast = nullptr;
}


BPlusTree::~BPlusTree()
= default;

bool BPlusTree::Insert(double nKey, Item* number)   //To change the value in the key value pair from double to item pointer
{
	int i = 0;
	int m = 0;

	if (m_pRoot==nullptr)
	{
        auto * pNew = new bpLeafNode();
        pNew->m_bLeaf = true;
		pNew->m_nKeyNum = 1;
        pNew->m_pParent = nullptr;
		pNew->leafNodeData->m_Keys[0]=nKey;
        pNew->leafNodeData->m_number[0]=number;
        m_pRoot = pNew;
		m_pFirst = pNew;
		return true;
	}

	bpNode * pTmp = m_pRoot;

	while (!pTmp->m_bLeaf)
	{
		for (i = 0; i < pTmp->m_nKeyNum; i++)
		{

			if (nKey <  ((InternalNode*)pTmp)->internalNodeData->m_Keys[i])
				break;
		}
		if (i == 0)
			pTmp = (bpNode *) (((InternalNode*)pTmp)->internalNodeData->m_Pointers[0]);
		else
			pTmp = (bpNode *)(((InternalNode*)pTmp)->internalNodeData->m_Pointers[i]);
	}

	for (i = 0; i < ((bpLeafNode *)pTmp)->m_nKeyNum; i++)
	{
		if (nKey == ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[i])    //Here, should not use element in m_number to make comparison with mkey, but in mkeys
		{
		    std::cout<<"The key has existed"<<std::endl;
			return false;
		}
	}

	if (((bpLeafNode *)pTmp)->m_nKeyNum < LEAF_ORDER)
	{
		for (i = 0; i < ((bpLeafNode *)pTmp)->m_nKeyNum; i++)
		{
			if (nKey < ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[i])     //Here, should not use element in m_number to make comparison with mkey, but in mkeys
            {
                break;
            }
		}


		for (m = ((bpLeafNode *)pTmp)->m_nKeyNum - 1; m >= i; m--)
		{
            ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[m + 1] = ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[m];
            ((bpLeafNode *)pTmp)->leafNodeData->m_number[m + 1] = ((bpLeafNode *)pTmp)->leafNodeData->m_number[m];
		}
        ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[i] = nKey;
        ((bpLeafNode *)pTmp)->leafNodeData->m_number[i] = number;
        Item* x1 = ((bpLeafNode *)pTmp)->leafNodeData->m_number[i];
        ((bpLeafNode *)pTmp)->m_nKeyNum++;
		return true;
	}

	int nMid = (LEAF_ORDER + 1) / 2;

	double * pTmpKeys = new double[LEAF_ORDER + 1];    //The temporary list of keys should be float instead of integer
	auto * pTmpnumbers = new Item*[LEAF_ORDER + 1];


	for (i = 0; i < pTmp->m_nKeyNum; i++)
	{
		if (((bpLeafNode *)pTmp)->leafNodeData->m_Keys[i] > nKey)  //Here the value should not be used for comparison but the key
			break;
	}
	for (m = pTmp->m_nKeyNum - 1; m >= i; m--)
	{
		pTmpKeys[m + 1] = ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[m];
        pTmpnumbers[m + 1] = ((bpLeafNode *)pTmp)->leafNodeData->m_number[m];
	}

	for (m = 0; m < i; m++)
	{
		pTmpKeys[m] = ((bpLeafNode *)pTmp)->leafNodeData->m_Keys[m];
        pTmpnumbers[m] = ((bpLeafNode *)pTmp)->leafNodeData->m_number[m];
        Item* x = ((bpLeafNode *)pTmp)->leafNodeData->m_number[m];
        Item* y = pTmpnumbers[m];
	}
	pTmpKeys[i] = nKey;
    pTmpnumbers[i] = number;


    auto * pNew = new bpLeafNode;

	for (m = 0, i = nMid; i < LEAF_ORDER + 1; i++,m++)
	{
		pNew->leafNodeData->m_Keys[m] =pTmpKeys[i];
		pNew->leafNodeData->m_number[m] = pTmpnumbers[i];
		pNew->m_nKeyNum++;
	}
    pNew->m_pParent = pTmp->m_pParent;

	pNew->m_bLeaf = pTmp->m_bLeaf;

	for (i = 0; i < pTmp->m_nKeyNum; i++)
	{
        ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i] = 0;
        ((bpLeafNode*)pTmp)->leafNodeData->m_number[i] = nullptr;       //Maybe here the elements should be void pointer
	}

	pTmp->m_nKeyNum = 0;

	for (i = 0; i < nMid; i++)
	{
        ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i] = pTmpKeys[i];
        ((bpLeafNode*)pTmp)->leafNodeData->m_number[i] = pTmpnumbers[i];
		pTmp->m_nKeyNum++;
	}
	pNew->leafNodeData->m_next = ((bpLeafNode*)pTmp)->leafNodeData->m_next;
    ((bpLeafNode*)pTmp)->leafNodeData->m_next= pNew;


	if (!InsertKeyAndPointer(pTmp->m_pParent, pTmp, pTmpKeys[nMid], pNew))
	{
	    std::cout<<"Insertion of an internode fails"<<std::endl;
		return false;
	}
	return true;
}

void BPlusTree::PrintLeaves()
{
    int num=0;
	int i = 0;
	if (!m_pFirst)
		return;
	bpLeafNode * pCur =(bpLeafNode*)m_pFirst;
	printf("------print leave-------\n");
	while(1){
        for(int i=0;i<pCur->m_nKeyNum;i++){
            printf("<key:%f, number:%lf>\n", pCur->leafNodeData->m_Keys[i], pCur->leafNodeData->m_number[i]);
            num=num+1;
        }
        if(pCur->leafNodeData->m_next==NULL){
            break;
        }else{
            pCur = static_cast<bpLeafNode *>(pCur->leafNodeData->m_next);
        }
        printf("\n------------------------------------------------------\n");
	}
	std::cout<<num<<std::endl;
}

bool BPlusTree::InsertKeyAndPointer(bpNode * pParent, bpNode * pOld, double nKey, bpNode * pNew)
{
	if (!pOld)
		return false;

	if (!pNew)
		return false;

	int i = 0;

	int m = 0;

	int k = 0;

	if (pParent == nullptr)
	{
		auto * pNewRoot = new InternalNode;
        pNewRoot->m_bLeaf = false;



		pNewRoot->internalNodeData->m_Keys[0] = nKey;
		pNewRoot->m_nKeyNum = 1;
        pNewRoot->internalNodeData->m_Pointers[0] = pOld;
        pNewRoot->internalNodeData->m_Pointers[1] = pNew;
		pNewRoot->m_pParent = nullptr;
		pOld->m_pParent = pNewRoot;
        pNew->m_pParent = pNewRoot;
		m_pRoot = pNewRoot;






		return true;
	}

	for (i = 0; i < pParent->m_nKeyNum; i++)
	{
		if (nKey == ((InternalNode*)pParent)->internalNodeData->m_Keys[i]) {
		    std::cout<<"Trying to inserting an existing key"<<std::endl;
            return false;
        }
		if (nKey < ((InternalNode*)pParent)->internalNodeData->m_Keys[i]) {
            break;
        }
	}

	if (pParent->m_nKeyNum < ORDER)
	{
		for (m = pParent->m_nKeyNum - 1; m >= i; m--)
		{
            ((InternalNode*)pParent)->internalNodeData->m_Keys[m + 1] = ((InternalNode*)pParent)->internalNodeData->m_Keys[m];
		}

		for (m = pParent->m_nKeyNum; m > i; m--)
		{
            ((InternalNode*)pParent)->internalNodeData->m_Pointers[m + 1] = ((InternalNode*)pParent)->internalNodeData->m_Pointers[m];
		}

        ((InternalNode*)pParent)->internalNodeData->m_Keys[i] = nKey;
        ((InternalNode*)pParent)->internalNodeData->m_Pointers[i + 1] = pNew;
		pParent->m_nKeyNum++;

		pNew->m_pParent = pParent;
		return true;
	}

	double * pTmpKeys = new double[ORDER + 1];   //The temporary list of keys should contain elements of data type of float instead of integer

	void ** pTmpPointers = new void *[ORDER + 2];

	for (i = 0; i < pParent->m_nKeyNum; i++)
	{
		if (nKey < ((InternalNode*)pParent)->internalNodeData->m_Keys[i])
			break;
	}

	for (m = 0; m < i; m++)
	{
		pTmpKeys[m] = ((InternalNode*)pParent)->internalNodeData->m_Keys[m];
	}

	pTmpKeys[m] = nKey;
	m++;

	for (k = i; k < pParent->m_nKeyNum; k++,m++)
	{
		pTmpKeys[m] = ((InternalNode*)pParent)->internalNodeData->m_Keys[k];
	}

	for (m = 0; m <= i; m++)
	{
		pTmpPointers[m] = ((InternalNode*)pParent)->internalNodeData->m_Pointers[m];
	}

	pTmpPointers[m] = pNew;
	m++;

	for (k = i + 1; k <= pParent->m_nKeyNum; k++,m++)
	{
		pTmpPointers[m] = ((InternalNode*)pParent)->internalNodeData->m_Pointers[k];
	}

	bpNode * pNewSplit = new InternalNode;

	pNewSplit->m_bLeaf = pParent->m_bLeaf;

	pNewSplit->m_pParent = pParent->m_pParent;

	int nMid = (ORDER + 1) / 2;

	double nMidKey = pTmpKeys[nMid]; //The key obtained from key in the middle position of the temporary list should be float instead of integer

	for (m = 0, i = nMid + 1; i < ORDER + 1; i++, m++)
	{
        ((InternalNode*)pNewSplit)->internalNodeData->m_Keys[m] = pTmpKeys[i];
        ((InternalNode*)pNewSplit)->internalNodeData->m_Pointers[m] = pTmpPointers[i];
		if (!pNewSplit->m_bLeaf)
		{
			bpNode * pCur = static_cast<bpNode *>(((InternalNode*)pNewSplit)->internalNodeData->m_Pointers[m]);
			if (pCur)
			{
				pCur->m_pParent = pNewSplit;
			}
		}
		pNewSplit->m_nKeyNum++;
	}

    ((InternalNode*)pNewSplit)->internalNodeData->m_Pointers[m] = pTmpPointers[i];

	if (!pNewSplit->m_bLeaf)
	{
		bpNode * pCur = static_cast<bpNode *>(((InternalNode*)pNewSplit)->internalNodeData->m_Pointers[m]);
		if (pCur)
		{
			pCur->m_pParent = pNewSplit;
		}
	}

	pParent->m_nKeyNum = 0;


	for (i = 0; i < nMid; i++)
	{
        ((InternalNode*)pParent)->internalNodeData->m_Keys[i] = pTmpKeys[i];
        ((InternalNode*)pParent)->m_nKeyNum++;
	}

	for (i = 0; i <= nMid; i++)
	{
        ((InternalNode*)pParent)->internalNodeData->m_Pointers[i] = pTmpPointers[i];
	}

    delete[] pTmpKeys;

    delete[] pTmpPointers;

	return InsertKeyAndPointer(pParent->m_pParent, pParent, nMidKey, pNewSplit);
}


bpLeafNode* BPlusTree::search(double nKey){    //The data type of the variable should be float instead of integer

    int i=0;

    if (!m_pRoot){
        std::cout<<"The tree is empty"<<std::endl;
        return nullptr;
    }


    bpNode * pTmp = m_pRoot;
    while (!pTmp->m_bLeaf)
    {
        for (i = 0; i < pTmp->m_nKeyNum; i++)
        {
            if (nKey < ((InternalNode*)pTmp)->internalNodeData->m_Keys[i])
            {
                break;
            }
        }

        pTmp = (bpNode*)(((InternalNode*)pTmp)->internalNodeData->m_Pointers[i]);
    }

    auto* leafNode = (bpLeafNode *)pTmp;
    /*for(int i=0;i<leafNode->m_nKeyNum;i++){
        if(nKey == leafNode->leafNodeData->m_Keys[i]){
            std::cout<<nKey<<" Exist"<<std::endl;
            return leafNode;
        }
    }
    std::cout<<nKey<<"The leafnode that includes the key does not exist"<<std::endl;*/
    return leafNode;
}








void BPlusTree::PrintLayerTree()
{
    int i = 0;

    queue<bpNode *> q;

    if (m_pRoot == NULL)
    {
        printf("b+tree is null.\n");
        return;
    }

    bpNode* node;

    node = m_pRoot;
    q.push(node);

    double test_layer=0;      // A variable to check if the node is the first node of a new line
    int test_leaf=0;       // A variable to Check if the node is the first node of the leaf nodes



    while (q.empty() == false)
    {
        bpNode * nodeTmp = q.front();

        if (nodeTmp->m_bLeaf == false)
        {
            if(((InternalNode*)nodeTmp) ->internalNodeData->m_Keys[0]<test_layer){        //if we need to change a new line
                std::cout<<" "<<std::endl;
            }
            for (i = 0;
            i < nodeTmp->m_nKeyNum; i++)
            {
                //printf("%d",((InternalNode*)nodeTmp) ->internalNodeData->m_Keys[i]);

                std::cout<<((InternalNode*)nodeTmp) ->internalNodeData->m_Keys[i]<<" ";   // A substitute one of output

                q.push((bpNode*)((InternalNode*)nodeTmp) ->internalNodeData->m_Pointers[i]);

                //printf(" ");


            }
            q.push((bpNode*)((InternalNode*)nodeTmp) ->internalNodeData->m_Pointers[i]);
            std::cout<<"|| ";           // A substitute one of output
            test_layer=((InternalNode*)nodeTmp) ->internalNodeData->m_Keys[nodeTmp->m_nKeyNum-1];
        }
        else
        {
            if(test_leaf==0){                  //Check if it is the first node of the leaf nodes
                std::cout<<" "<<std::endl;
            }
            for (i = 0; i < nodeTmp->m_nKeyNum; i++)
            {
                //printf("%d", ((LeafNode*)nodeTmp)->leafNodeData->m_Keys[i]);
                //printf(" ");

                std::cout<<((bpLeafNode*)nodeTmp) ->leafNodeData->m_Keys[i]<<" ";   // A substitute one of output


            }
            test_leaf=test_leaf+1;
            std::cout<<"|| ";           // A substitute one of output
        }

        //printf("\n");

        //printf("-------------------------------------------------\n");

        q.pop();
    }

    std::cout<<" "<<std::endl;

    return;
}

bool BPlusTree::Remove(double nKey)    //Here, the data type of the variable should be float instead of integer
{
	if (!m_pRoot)
		return false;

	int i = 0;

	int j=-1;

	int m = 0;

	bpNode * pTmp = m_pRoot;

	while (!pTmp->m_bLeaf)
	{
	    bool flag=false;
		for (i = 0; i < pTmp->m_nKeyNum; i++)
		{
            if (nKey < ((InternalNode*)pTmp)->internalNodeData->m_Keys[i])
            {
                flag = true;
                j=i;
                break;
            }
		}
		if(flag==false){
		    j = pTmp->m_nKeyNum;
		}

        pTmp = (bpNode*)(((InternalNode*)pTmp)->internalNodeData->m_Pointers[i]);
	}

	for (i = 0; i < pTmp->m_nKeyNum; i++)
	{
		if (nKey == ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i]) {
            break;
        }
	}

	if (i == pTmp->m_nKeyNum) {
	    printf("Wrong! %d node not exists",nKey);
        return false;
    }

	auto * pCur = (bpLeafNode *)pTmp;


	for (m = i + 1; m < pTmp->m_nKeyNum; m++)
	{
        pCur->leafNodeData->m_Keys[m-1] = pCur->leafNodeData->m_Keys[m];
        pCur->leafNodeData->m_number[m-1] = pCur->leafNodeData->m_number[m];
	}

	pTmp->m_nKeyNum--;

	int nLowNum = (LEAF_ORDER+1)  / 2;

	if (pTmp->m_nKeyNum >= nLowNum)
	{
		return true;
	}


	bpNode * pParent = pTmp->m_pParent;

	if (!pParent) {
        if (pTmp->m_nKeyNum < 1) {
            m_pRoot = NULL;
            delete pTmp;
            m_pFirst = m_pLast = NULL;
        }
        return true;
    }

	bpNode * pNeighbor = NULL;
	int nNeighbor = -1;
	int nIndex = -1;

	if (j == 0)
	{
		pNeighbor = (bpNode*)(((InternalNode *)pParent)->internalNodeData->m_Pointers[1]);
		nNeighbor = 1;
		nIndex = 0;
	}
	else
	{
        pNeighbor = (bpNode*)(((InternalNode *)pParent)->internalNodeData->m_Pointers[j-1]);
		nNeighbor = j - 1;
		nIndex = j;
	}

	if (pNeighbor->m_nKeyNum > nLowNum)
	{
		if (nNeighbor < nIndex)
		{
            ((InternalNode*)pParent)->internalNodeData->m_Keys[nNeighbor] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[pNeighbor->m_nKeyNum - 1];
			for (i = pTmp->m_nKeyNum - 1; i >= 0; i--)
			{
                ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i + 1] = ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i];
                ((bpLeafNode*)pTmp)->leafNodeData->m_number[i + 1] = ((bpLeafNode*)pTmp)->leafNodeData->m_number[i];
			}
            ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i + 1] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[pNeighbor->m_nKeyNum - 1];
            ((bpLeafNode*)pTmp)->leafNodeData->m_number[0] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[pNeighbor->m_nKeyNum - 1];
			pTmp->m_nKeyNum++;
			pNeighbor->m_nKeyNum--;
		}
		else
		{
            ((InternalNode*)pParent)->internalNodeData->m_Keys[nIndex] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[1];
            ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[pTmp->m_nKeyNum] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[0];   //Here, the key borrowed should be the first key from the right brother node instead of from the node itself
            ((bpLeafNode*)pTmp)->leafNodeData->m_number[pTmp->m_nKeyNum] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[0];   //Here, the number borrowed should be the first number from the right brother node instead of from the node itself
			pTmp->m_nKeyNum++;
			for (i = 1; i <= pNeighbor->m_nKeyNum - 1; i++)
			{
                ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[i - 1] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[i];
                ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[i - 1] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[i];
			}
			pNeighbor->m_nKeyNum--;
		}
		return true;
	}

	else
	{
		if (nNeighbor < nIndex)
            {
			for (i = 0; i < pTmp->m_nKeyNum; i++)
			{
                ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[pNeighbor->m_nKeyNum] = ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[i];
                ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[pNeighbor->m_nKeyNum] = ((bpLeafNode*)pTmp)->leafNodeData->m_number[i];
				pNeighbor->m_nKeyNum++;
			}

			for (i = nIndex; i < pParent->m_nKeyNum; i++)
			{
                ((InternalNode*)pParent)->internalNodeData->m_Keys[i - 1] = ((InternalNode*)pParent)->internalNodeData->m_Keys[i];
			}

			for (i = nIndex + 1; i <= pParent->m_nKeyNum; i++)
			{
                ((InternalNode*)pParent)->internalNodeData->m_Pointers[i - 1] = ((InternalNode*)pParent)->internalNodeData->m_Pointers[i];
			}

			pParent->m_nKeyNum--;

            ((bpLeafNode*)pNeighbor)->leafNodeData->m_next = ((bpLeafNode*)pTmp)->leafNodeData->m_next;

			delete pTmp;

		}
		else
		{
			for (i = 0; i < pNeighbor->m_nKeyNum; i++)
			{
                ((bpLeafNode*)pTmp)->leafNodeData->m_Keys[pTmp->m_nKeyNum] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_Keys[i];
                ((bpLeafNode*)pTmp)->leafNodeData->m_number[pTmp->m_nKeyNum] = ((bpLeafNode*)pNeighbor)->leafNodeData->m_number[i];
				pTmp->m_nKeyNum++;
			}

			for (i = nNeighbor; i < pParent->m_nKeyNum; i++)
			{
                ((InternalNode*)pParent)->internalNodeData->m_Keys[i - 1] = ((InternalNode*)pParent)->internalNodeData->m_Keys[i];
			}
			for (i = nNeighbor + 1; i <= pParent->m_nKeyNum; i++)
			{
                ((InternalNode*)pParent)->internalNodeData->m_Pointers[i - 1] = ((InternalNode*)pParent)->internalNodeData->m_Pointers[i];
			}

			pParent->m_nKeyNum--;
            ((bpLeafNode*)pTmp)->leafNodeData->m_next = ((bpLeafNode*)pNeighbor)->leafNodeData->m_next;
			delete pNeighbor;
		}

		bpNode * pCurTmp = pParent;
		int nInternalLowNum = (ORDER + 1) / 2;

		while (pCurTmp)
		{
			if (pCurTmp->m_nKeyNum >= nInternalLowNum)
			{
				break;
			}

			bpNode * pCurParent = pCurTmp->m_pParent;

			bpNode * pCurNeighbor = NULL;

			int nCurIndex = 0;

			int nNeighborIndex = 0;

			double nTmp = 0;    //It is a key which should be changed to float

			if (!pCurParent)
			{
				if (pCurTmp->m_nKeyNum < 1)
				{
                    ((bpNode*)(((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[0]))->m_pParent= NULL;
					m_pRoot = (bpNode *)(((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[0]);
					delete pCurTmp;
				}
				break;
			}
			else
			{
				for (i = 0; i <= pCurParent->m_nKeyNum; i++)
				{
					if (pCurTmp == ((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i])
					{
						break;
					}
				}

				if (i == 0)
				{
					pCurNeighbor = (bpNode *)((InternalNode*)pCurParent)->internalNodeData->m_Pointers[1];
					nCurIndex = 0;
					nNeighborIndex = 1;
				}
				else
				{
					pCurNeighbor = (bpNode *)((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i-1];
					nCurIndex = i;
					nNeighborIndex = i-1;
				}

				if (pCurNeighbor->m_nKeyNum > nInternalLowNum)
				{
					if (nNeighborIndex < nCurIndex)
					{
						nTmp = ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nNeighborIndex];
                        ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nNeighborIndex] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[pCurNeighbor->m_nKeyNum - 1];
						for (i = pCurTmp->m_nKeyNum - 1; i >= 0; i--)
						{
                            ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[i + 1] = ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[i];
						}
						for (i = pCurTmp->m_nKeyNum; i >= 0; i--)
						{
						    ((InternalNode *) pCurTmp)->internalNodeData->m_Pointers[i + 1] = ((InternalNode *) pCurTmp)->internalNodeData->m_Pointers[i];
						}
                        ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[0] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[pCurNeighbor->m_nKeyNum-1];   //It seems that the nTmp could not mean the last key of the neighbour
						if (((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum])
						{

                            static_cast<bpNode *>(((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum])->m_pParent = pCurTmp;
						}
                        ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[0] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum];
						pCurTmp->m_nKeyNum++;
						pCurNeighbor->m_nKeyNum--;
					}
					else
					{
						nTmp =  ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nCurIndex];
                        ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nCurIndex] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[1];  //Here, it seems that the key at the right of the parent in the grandparent should be the second key of the uncle
                        ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[pCurTmp->m_nKeyNum] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[0];  //Here, it seems that the new key for the parent should be the first key of the uncle

						if (((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[0])
						{
							static_cast<bpNode *>(((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[0])->m_pParent = pCurTmp;
						}

                        ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum + 1] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[0];
						pCurTmp->m_nKeyNum++;

						for (i = 1; i < pCurNeighbor->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[i - 1] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[i];
						}
						for (i = 1; i <= pCurNeighbor->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[i-1] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[i];
						}

						pCurNeighbor->m_nKeyNum--;
					}
					break;
				}
				else
				{
					if (nNeighborIndex < nCurIndex)
					{

                        ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[pCurNeighbor->m_nKeyNum] = ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nNeighborIndex];
						pCurNeighbor->m_nKeyNum++;
						for (i = 0; i < pCurTmp->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[pCurNeighbor->m_nKeyNum] = ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[i];
                            //pCurNeighbor->m_nKeyNum++;
                            ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum] = ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[i];
							bpNode * pChild = (bpNode *)((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum];
							if (pChild)
								pChild->m_pParent = pCurNeighbor;
							pCurNeighbor->m_nKeyNum++;
						}

                        ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum] = ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[i];

						if ( ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum])
						{
							static_cast<bpNode *>( ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[pCurNeighbor->m_nKeyNum])->m_pParent = pCurNeighbor;
						}

						for (i = nNeighborIndex + 1; i < pCurParent->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurParent)->internalNodeData->m_Keys[i - 1] = ((InternalNode*)pCurParent)->internalNodeData->m_Keys[i];
						}

						for (i = nCurIndex + 1; i <= pCurParent->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i - 1] = ((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i];
						}

						pCurParent->m_nKeyNum--;

						delete pCurTmp;
					}
					else
					{
                        ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[pCurTmp->m_nKeyNum] = ((InternalNode*)pCurParent)->internalNodeData->m_Keys[nCurIndex];
						pCurTmp->m_nKeyNum++;

						for (i = 0; i < pCurNeighbor->m_nKeyNum; i++)
						{
                            ((InternalNode*)pCurTmp)->internalNodeData->m_Keys[pCurTmp->m_nKeyNum] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Keys[i];
                            ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[i];
							bpNode * pChild = (bpNode *)(((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum]);
							if (pChild)
								pChild->m_pParent = pCurTmp;
							pCurTmp->m_nKeyNum++;
						}

                        ((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum] = ((InternalNode*)pCurNeighbor)->internalNodeData->m_Pointers[i];
						if (((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum])
						{
							((bpNode *)(((InternalNode*)pCurTmp)->internalNodeData->m_Pointers[pCurTmp->m_nKeyNum]))->m_pParent = pCurTmp;
						}

						for (i = nCurIndex + 1; i < pCurParent->m_nKeyNum; i++)
						{
							((InternalNode*)pCurParent)->internalNodeData->m_Keys[i - 1] = ((InternalNode*)pCurParent)->internalNodeData->m_Keys[i];
						}

						for (i = nNeighborIndex + 1; i <= pCurParent->m_nKeyNum; i++)
						{
							((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i - 1] = ((InternalNode*)pCurParent)->internalNodeData->m_Pointers[i];
						}

						pCurParent->m_nKeyNum--;

						delete pCurNeighbor;
					}

					pCurTmp = pCurParent;
				}
			}
		}

		return true;
	}
}


void BPlusTree::get_range_item(std::vector<Item*>* pointer,  double a, double b, bool left_bool, bool right_bool, bool print_if){
    if(print_if==false){
        if((left_bool==true)&&(right_bool==true)){
            if(a>b){
                //std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>=a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<=b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
            }
        }
        if((left_bool==true)&&(right_bool==false)){
            if(a>=b){
                //std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>=a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
            }
        }
        if((left_bool==false)&&(right_bool==false)){
            if(a>=b){
                //std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
            }
        }
        if((left_bool==false)&&(right_bool==true)){
            if(a>=b){
                //std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<=b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
            }
        }





    }else{
        if((left_bool==true)&&(right_bool==true)){
            if(a>b){
                std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                int count_get=0;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>=a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<=b)){
                            std::cout<<pointer_dynamic->leafNodeData->m_Keys[i]<<" ";
                            //printf("<key:%f>\n", pointer_dynamic->leafNodeData->m_Keys[i]);
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                            count_get=count_get+1;
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
                std::cout<<'\n'<<std::endl;
                std::cout<<"Get "<<count_get<<" keys"<<std::endl;
            }
        }
        if((left_bool==true)&&(right_bool==false)){
            if(a>=b){
                std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                int count_get=0;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>=a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                            std::cout<<pointer_dynamic->leafNodeData->m_Keys[i]<<" ";
                            //printf("<key:%f>\n", pointer_dynamic->leafNodeData->m_Keys[i]);
                            count_get=count_get+1;
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
                std::cout<<'\n'<<std::endl;
                std::cout<<"Get "<<count_get<<" keys"<<std::endl;
            }
        }
        if((left_bool==false)&&(right_bool==false)){
            if(a>=b){
                std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                int count_get=0;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                            std::cout<<pointer_dynamic->leafNodeData->m_Keys[i]<<" ";
                            //printf("<key:%f>\n", pointer_dynamic->leafNodeData->m_Keys[i]);
                            count_get=count_get+1;
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
                std::cout<<'\n'<<std::endl;
                std::cout<<"Get "<<count_get<<" keys"<<std::endl;
            }
        }
        if((left_bool==false)&&(right_bool==true)){
            if(a>=b){
                std::cout<<"The floor of the range should be smaller than the cell"<<std::endl;
            }else{
                bpLeafNode* left=search(a);
                bpLeafNode* right=search(b);
                bpLeafNode* pointer_dynamic=left;
                int i;
                int count_get=0;
                while(1==1){
                    for(i=0; i<pointer_dynamic->m_nKeyNum;i++){
                        if((pointer_dynamic->leafNodeData->m_Keys[i]>a)&&(pointer_dynamic->leafNodeData->m_Keys[i]<=b)){
                            (*pointer).emplace_back(pointer_dynamic->leafNodeData->m_number[i]);
                            std::cout<<pointer_dynamic->leafNodeData->m_Keys[i]<<" ";
                            //printf("<key:%f>\n", pointer_dynamic->leafNodeData->m_Keys[i]);
                            count_get=count_get+1;
                        }
                    }
                    if(pointer_dynamic==right){
                        break;
                    }else{
                        pointer_dynamic=(bpLeafNode*)(pointer_dynamic->leafNodeData->m_next);
                    }
                }
                std::cout<<'\n'<<std::endl;
                std::cout<<"Get "<<count_get<<" keys"<<std::endl;
            }
        }
    }


}
