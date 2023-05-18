#pragma once
#include <vector>
#include <unordered_map>
#include <stack>

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

public:
	// Queried to get entity
	std::unordered_map<size_t, Entity> nodeToEntityMap;
	// Queried to get node
	std::unordered_map<Entity, size_t> entityToNodeMap;

	// Stack of free nodes
	std::stack<size_t> mFreeList;
public:
	explicit BoundingBoxTree(size_t initialCapacity = 1);

	void InsertEntity(Entity entity, BoundingBox box);
	void RemoveEntity(Entity entity);
	void UpdateEntity(Entity entity, BoundingBox box);

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
	void TreeQuery(const size_t node1, const size_t node2);

	// Inserts an allocated node into the tree
	void InsertLeaf(size_t leafIndex);

	// Returns entity given node index
	Entity GetEntity(size_t nodeIndex);

	// Gets sibling of node
	size_t GetSibling(size_t nodeIndex);

	// Returns the index of the best sibling
	size_t FindBestSibling(size_t leafIndex) const;

	// Balance
	size_t Balance(size_t node);

	// Returns true if the node at the given index is a leaf node
	bool IsLeaf(size_t index) const;

	// Returns true if not a leaf node
	bool IsInternal(size_t nodeIndex) const;

	// Resets the data in the node
	void ResetNodeData(size_t nodeIndex);
};
