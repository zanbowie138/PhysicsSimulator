#pragma once
#include <vector>

#include "BoundingBox.h"

constexpr int NULL_NODE = 0xffffffff;

struct Node {
	BoundingBox box;
	unsigned int objectIndex;
	unsigned int parent;
	unsigned int height;
	unsigned int next;
	unsigned int left;
	unsigned int right;
};

class BoundingBoxTree
{
public:
	std::vector<Node> nodes;

	unsigned int nodeCapacity;
	unsigned int nodeCount;
	unsigned int rootIndex;

	unsigned int freeList;

	BoundingBoxTree(unsigned int initialCapacity = 1);
	~BoundingBoxTree();

	// Allocates a space for a new node
	unsigned int AllocateNode();
	// Frees a space for a new node
	void FreeNode(unsigned int nodeIndex);
	// Expand capacity
	void ExpandCapacity(unsigned int newNodeCapacity);

	// Inserts a node into the tree
	void InsertLeaf(unsigned int leafIndex);

	// Add all possible 
	void TreeQuery(unsigned int node);

	// Balance
	unsigned int Balance(unsigned int node);
private:
	bool IsLeaf(unsigned int index);
	void ResetNodeData(unsigned int node);
};