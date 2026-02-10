#pragma once
#include "math/Ray.h"


namespace Physics {
	constexpr size_t NULL_NODE = 0xffffffff;

	// Algorithm adapted from Box2D's dynamic tree
	class DynamicBBTree
	{
		struct Node
		{
			BoundingBox box;
			size_t height, parent, left, right;
		};

	public:
		std::vector<Node> mNodes;

		size_t nodeCapacity, nodeCount, rootIndex;

		// Queried to get entity
		std::unordered_map<size_t, Entity> nodeIdxToEntityMap;
		// Queried to get node
		std::unordered_map<Entity, size_t> entityToNodeIdxMap;

		// Stack of free nodes
		std::stack<size_t> mFreeList;

		explicit DynamicBBTree(size_t initialCapacity = 1);

		void InsertEntity(Entity entity, BoundingBox box);
		void RemoveEntity(Entity entity);
		void UpdateEntity(Entity entity, BoundingBox box);
		void UpdateEntity(Entity entity, glm::vec3 transform);

		// Uses TreeQuery to compute all box pairs
		std::vector<Entity> ComputeCollisionPairs();
		std::pair<std::vector<BoundingBox>, bool> QueryRayCollisions(Ray ray) const;
		std::pair<Entity, bool> QueryRay(Ray ray) const;

		// Returns reference to object's bounding box
		BoundingBox GetBoundingBox(Entity object) const;

		// Returns a vector of all active bounding boxes
		// Bool decides whether non-leaf boxes are added
		std::vector<BoundingBox> GetAllBoxes(const bool onlyLeaf) const;

	private:
		Node& GetNode(Entity entity);
		
		// Allocates a space for a new node
		// Returns the index position of the allocated node
		size_t AllocateNode();
		// Frees a space for a new node
		void FreeNode(size_t nodeIndex);

		// Expand capacity
		void ExpandCapacity(size_t newNodeCapacity);

		// Inserts an allocated node into the tree
		void InsertLeaf(size_t leafIndex);

		// Returns object given node index
		Entity GetObject(size_t nodeIndex) const;

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

