#pragma once
#include "BoundingBox.h"

constexpr int NULL_NODE = 0xffffffff;
/*
struct Node {
	BoundingBox box;
	unsigned int objectIndex;
	unsigned int parentIndex;
	unsigned int left;
	unsigned int right;
	bool isLeaf;
};
struct BBTree {
};

class BoundingBoxTree
{
public:
	Node* nodes;

	unsigned int nodeCapacity;
	unsigned int nodeCount;
	unsigned int rootIndex;

	BoundingBoxTree(): rootNode(nullptr), nodeCount(0){}
	~BoundingBoxTree();

	// Inserts a node into the tree
	void InsertNode(unsigned int objectIndex, BoundingBox box);

	// 
	Node* AllocateNode(unsigned int objectIndex, BoundingBox box);

	// Add all possible 
	void TreeQuery(BoundingBox* box);

	// Expand capacity
	void ExpandCapacity();
};

inline Node* BoundingBoxTree::AllocateNode(unsigned int objectIndex, BoundingBox box)
{
	Node* newNode = new Node{box, objectIndex, 0, 0, 0, false};
}

inline void BoundingBoxTree::InsertNode(unsigned int objectIndex, BoundingBox box)
{
	if (tree.nodeCount == 0)
	{
		tree.rootIndex = ;
	}

	// Find best sibling
	BoundingBox* bestSibling = nullptr;
	for (int i = 0; i < nodeCount; i++)
	{
		// TODO: Get pointer to best sibling
	}
	// Create new parent
	bestSibling.parent->
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

inline void BoundingBoxTree::ExpandCapacity() 
{
	void* newMemory = malloc();
}*/