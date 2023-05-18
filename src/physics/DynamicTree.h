#pragma once
#include <vector>
#include <unordered_map>
#include <stack>

#include "../core/GlobalTypes.h"
#include "BoundingBox.h"

namespace Physics {
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
	template <typename T>
	class DynamicBBTree
	{
	public:
		std::vector<Node> mNodes;

		size_t nodeCapacity;
		size_t nodeCount;
		size_t rootIndex;

		std::vector<size_t> mCollisions;

	public:
		// Queried to get object
		std::unordered_map<size_t, T> nodeToObjectMap;
		// Queried to get node
		std::unordered_map<T, size_t> objectToNodeMap;

		// Stack of free nodes
		std::stack<size_t> mFreeList;
	public:
		explicit DynamicBBTree(size_t initialCapacity = 1);

		void InsertEntity(T object, BoundingBox box);
		void RemoveEntity(T object);
		void UpdateEntity(T object, BoundingBox box);

		// Uses TreeQuery to compute all box pairs
		void ComputePairs();

		// Returns reference to object's bounding box
		const BoundingBox& GetBoundingBox(T object) const;

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

		// Returns object given node index
		T GetEntity(size_t nodeIndex);

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
}
