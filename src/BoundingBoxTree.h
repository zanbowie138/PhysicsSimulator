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

	// Inserts a node into the tree
	void InsertLeaf(unsigned int leafIndex);

	// Allocates a space for a new node
	unsigned int AllocateNode();

	// Frees a space for a new node
	void FreeNode(unsigned int nodeIndex);

	// Add all possible 
	void TreeQuery(BoundingBox* box);

	// Expand capacity
	void ExpandCapacity(unsigned int newNodeCapacity);
private:
	bool IsLeaf(unsigned int index);
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
	if (nodes[freeList].next == NULL_NODE)
	{
		ExpandCapacity(nodeCapacity * 2);
	}

	unsigned int nodeIndex = freeList;
	freeList = nodes[nodeIndex].next;
	nodes[nodeIndex].parentIndex = NULL_NODE;
	nodes[nodeIndex].left = NULL_NODE;
	nodes[nodeIndex].right = NULL_NODE;
	nodes[nodeIndex].height = 0;
	nodeCount++;

	return nodeIndex;
}

inline void BoundingBoxTree::FreeNode(unsigned int nodeIndex)
{
	nodes[nodeIndex].next = freeList;
	nodes[nodeIndex].height = NULL_NODE;
	freeList = nodeIndex;
	nodeCount--;
}

inline void BoundingBoxTree::InsertLeaf(unsigned int leafIndex)
{
	unsigned int newNode = AllocateNode();
	if (rootIndex = NULL_NODE)
	{
		rootIndex = newNode;
		nodes[newNode].parentIndex = NULL_NODE;
		return;
	}

	// Find best sibling
	unsigned int sibling = rootIndex;
	BoundingBox leafBox = nodes[leafIndex].box;
	while (IsLeaf(sibling))
	{
		unsigned int left = nodes[sibling].left;
		unsigned int right = nodes[sibling].right;

		float surfaceArea = nodes[sibling].box.surfaceArea;

		BoundingBox combinedBox;
		combinedBox.Merge(leafBox, nodes[sibling].box);
		float combinedSurfaceArea = combinedBox.surfaceArea;

		// Cost of creating parent and leaf
		float cost = combinedSurfaceArea;

		// Minimum cost of pushing leaf further down the tree
		float inheritanceCost = combinedSurfaceArea - surfaceArea;

		// Cost of descending to the left.
		double costLeft;
		if (IsLeaf(left))
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[left].box);
			costLeft = aabb.surfaceArea + inheritanceCost;
		}
		else
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[left].box);
			double oldArea = nodes[left].box.surfaceArea;
			double newArea = aabb.surfaceArea;
			costLeft = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending to the right.
		double costRight;
		if (IsLeaf(right))
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[right].box);
			costRight = aabb.surfaceArea + inheritanceCost;
		}
		else
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[right].box);
			double oldArea = nodes[right].box.surfaceArea;
			double newArea = aabb.surfaceArea;
			costRight = (newArea - oldArea) + inheritanceCost;
		}

		// Descend according to the minimum cost.
        if ((cost < costLeft) && (cost < costRight)) break;

		// Descend.
		if (costLeft < costRight) sibling = left;
		else                      sibling = right;
	}

	// Create new parent
	unsigned int oldParent = nodes[sibling].parent;
	unsigned int newParent = AllocateNode();

	nodes[newParent].parent = oldParent;
	nodes[newParent].box.Merge(leafBox, nodes[sibling].box);
	nodes[newParent].height = nodes[sibling].height + 1;

	// The sibling was not the root.
	if (oldParent != NULL_NODE)
	{
		if (nodes[oldParent].left == sibling) nodes[oldParent].left = newParent;
		else                                  nodes[oldParent].right = newParent;

		nodes[newParent].left = sibling;
		nodes[newParent].right = leafIndex;
		nodes[sibling].parent = newParent;
		nodes[leafIndex].parent = newParent;
	}
	// The sibling was the root.
	else
	{
		nodes[newParent].left = sibling;
		nodes[newParent].right = leafIndex;
		nodes[sibling].parent = newParent;
		nodes[leafIndex].parent = newParent;
		rootIndex = newParent;
	}

	// Walk back up tree refitting boxes
	unsigned int index = nodes[leafIndex].parent;
	while (index != NULL_NODE)
	{
		index = balance(index);

		unsigned int left = nodes[index].left;
		unsigned int right = nodes[index].right;

		nodes[index].height = 1 + std::max(nodes[left].height, nodes[right].height);
		nodes[index].box.Merge(nodes[left].box, nodes[right].box);

		index = nodes[index].parent;
	}
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
	nodes.resize(nodeCapacity);
	for (unsigned int i = nodeCount; i < nodeCapacity; i++) {
		nodes[i].next = i+1;
		nodes[i].height = NULL_NODE;
	}
	nodes[nodeCapacity-1].next = NULL_NODE;
    nodes[nodeCapacity-1].height = NULL_NODE;
	freeList = nodeCount;
}

inline bool BoundingBox::IsLeaf(unsigned int index)
{
	return !(nodes[index].left == NULL_NODE && nodes[index].right == NULL_NODE);
}