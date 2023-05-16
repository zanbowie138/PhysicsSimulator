#pragma once
#include <vector>
#include <unordered_map>

#include "../core/GlobalTypes.h"
#include "BoundingBox.h"

constexpr size_t NULL_NODE = 0xffffffff;

struct Node
{
	BoundingBox box;

	size_t height;

	size_t parent;

	size_t left;
	size_t right;

	// Used for linking to the next free node in the freeList stack
	size_t next;
};


// Algorithm adapted from Box2D's dynamic tree
class BoundingBoxTree
{
public:
	std::vector<Node> mNodes;

	size_t nodeCapacity;
	size_t nodeCount;
	size_t rootIndex;

	std::vector<size_t> mCollisions;

	// Top of the stack
	size_t freeList;
private:
	// Queried to get entity
	std::unordered_map<size_t, Entity> nodeToEntityMap;
	// Queried to get node
	std::unordered_map<Entity, size_t> entityToNodeMap;
public:
	explicit BoundingBoxTree(size_t initialCapacity = 1);

	void InsertEntity(Entity entity, BoundingBox box);
	void RemoveEntity(Entity entity);

	// Uses TreeQuery to compute all box pairs
	void ComputePairs();

	// Returns reference to entity's bounding box
	const BoundingBox& GetBoundingBox(Entity entity) const;

	// Returns a vector of all active bounding boxes
	// Bool decides whether non-leaf boxes are added
	std::vector<BoundingBox> GetAllBoxes(const bool onlyLeaf) const;
private:
	// Allocates a space for a new node
	// Returns the index position of the allocated node
	size_t AllocateNode();
	// Frees a space for a new node
	void FreeNode(size_t nodeIndex);

	// Expand capacity
	void ExpandCapacity(size_t newNodeCapacity);

	// Add all possible nodes to intersect test
	void TreeQuery(size_t node);

	// Inserts an allocated node into the tree
	void InsertLeaf(size_t leafIndex);

	// Gets sibling of node
	size_t GetSibling(size_t nodeIndex);

	// Returns the index of the best sibling
	size_t FindBestSibling(size_t leafIndex) const;

	// Balance
	size_t Balance(size_t node);

	// Returns true if the node at the given index is a leaf node
	bool IsLeaf(size_t index) const;

	// Resets the data in the node
	void ResetNodeData(size_t nodeIndex);
};
