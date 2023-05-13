#pragma once
#include <vector>
#include <unordered_map>

#include "../core/GlobalTypes.h"
#include "BoundingBox.h"

constexpr size_t NULL_NODE = 0xffffffff;

struct Node
{
	BoundingBox box;

	//size_t index;
	size_t height;

	size_t parent;

	size_t left;
	size_t right;

	size_t next;
};

class BoundingBoxTree
{
public:
	std::vector<Node> nodes;
	// Used to get entity
	std::unordered_map<size_t, Entity> nodeToEntityMap;
	// Used to get node
	std::unordered_map<Entity, size_t> entityToNodeMap;

	size_t nodeCapacity;
	size_t nodeCount;
	size_t rootIndex;

	size_t freeList;

	explicit BoundingBoxTree(size_t initialCapacity = 1);

	void InsertEntity(Entity entity, BoundingBox box);
	void RemoveEntity(Entity entity);

	// Add all possible nodes to intersect test
	void TreeQuery(size_t node);

	const BoundingBox& GetBoundingBox(Entity entity) const;

private:
	// Allocates a space for a new node
	size_t AllocateNode();
	// Frees a space for a new node
	void FreeNode(size_t nodeIndex);
	// Expand capacity
	void ExpandCapacity(size_t newNodeCapacity);

	// Inserts a node into the tree
	void InsertLeaf(size_t leafIndex);

	size_t FindBestSibling(size_t leafIndex) const;

	// Balance
	size_t Balance(size_t node);

	bool IsLeaf(size_t index) const;
	void ResetNodeData(size_t nodeIndex);
};
