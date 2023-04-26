#pragma once
#include <vector>

#include "BoundingBox.h"

constexpr int NULL_NODE = 0xffffffff;

struct Node {
	BoundingBox box;
	unsigned int objectIndex;
	unsigned int parentIndex;
	unsigned int height;
	unsigned int next;
	unsigned int left;
	unsigned int right;
};

class BoundingBoxTree
{
public:
	std::vector<Node> nodeStorage;

	unsigned int nodeCapacity;
	unsigned int nodeCount;
	unsigned int rootIndex;

	unsigned int freeList;

	BoundingBoxTree(unsigned int initialCapacity = 1);
	~BoundingBoxTree();

	// Inserts a node into the tree
	void InsertLeaf(unsigned int objectIndex, BoundingBox box);

	// Allocates a space for a new node
	unsigned int AllocateNode();

	// Frees a space for a new node
	void FreeNode(unsigned int nodeIndex);

	// Add all possible 
	void TreeQuery(BoundingBox* box);

	// Expand capacity
	void ExpandCapacity(unsigned int newNodeCapacity);
};

inline BoundingBoxTree::BoundingBoxTree(unsigned int initialCapacity)
{
	rootIndex = NULL_NODE;
	nodeCount = 0;
	nodeCapacity = 0;

	ExpandCapacity(initialCapacity);
}

inline unsigned int BoundingBoxTree::AllocateNode()
{
	if (nodeStorage[freeList].next == NULL_NODE)
	{
		ExpandCapacity(nodeCapacity * 2);
	}

	unsigned int nodeIndex = freeList;
	freeList = nodeStorage[nodeIndex].next;
	nodeStorage[nodeIndex].parentIndex = NULL_NODE;
	nodeStorage[nodeIndex].left = NULL_NODE;
	nodeStorage[nodeIndex].right = NULL_NODE;
	nodeStorage[nodeIndex].height = 0;
	nodeCount++;

	return nodeIndex;
}

inline void BoundingBoxTree::FreeNode(unsigned int nodeIndex)
{
	nodeStorage[nodeIndex].next = freeList;
	nodeStorage[nodeIndex].height = NULL_NODE;
	freeList = nodeIndex;
	nodeCount--;
}

inline void BoundingBoxTree::InsertLeaf(unsigned int objectIndex, BoundingBox box)
{
	unsigned int newNode = AllocateNode();
	if (rootIndex = NULL_NODE)
	{
		rootIndex = 0;
	}

	// Find best sibling
	unsigned int bestSibling = NULL_NODE;
	for (int i = 0; i < nodeCount; i++)
	{
		// TODO: Get index of best sibling
	}

	// Create new parent
	
	// Walk back up tree refitting boxes
	
	nodeCount++;
}

inline void BoundingBoxTree::TreeQuery(BoundingBox* box)
{
	if (rootNode == NULL) return;

	if (rootNode->IsColliding(*box)) 
	{
		if (rootNode->child1 == NULL &&& rootNode->child2 == NULL) // ISLEAF
		{
			// TODO: ADD TO LIST
		}
		else 
		{
			TreeQuery(rootNode->child1);
			TreeQuery(rootNode->child2);
		}
	}
}

inline void BoundingBoxTree::ExpandCapacity(unsigned int newNodeCapacity) 
{
	if (newNodeCapacity <= nodeCapacity)
	{
		throw std::invalid_argument("newNodeCapacity parameter must be greater than or equal to current nodeCapacity.");
	}

	nodeCapacity = newNodeCapacity;
	nodeStorage.resize(nodeCapacity);
	for (unsigned int i = nodeCount; i < nodeCapacity; i++) {
		nodeStorage[i].next = i+1;
		nodeStorage[i].height = NULL_NODE;
	}
	nodeStorage[nodeCapacity-1].next = NULL_NODE;
    nodeStorage[nodeCapacity-1].height = NULL_NODE;
	freeList = nodeCount;
}