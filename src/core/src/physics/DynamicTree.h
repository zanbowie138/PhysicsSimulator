#pragma once
#include <vector>
#include <unordered_map>
#include <stack>

#include "../core/GlobalTypes.h"
#include "BoundingBox.h"

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

		// Returns reference to object's bounding box
		const BoundingBox& GetBoundingBox(Entity object) const;

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
		Entity GetObject(size_t nodeIndex);

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


	inline DynamicBBTree::DynamicBBTree(const size_t initialCapacity)
	{
		rootIndex = NULL_NODE;
		nodeCount = 0;
		nodeCapacity = 0;

		ExpandCapacity(initialCapacity);
	}


	inline void DynamicBBTree::InsertEntity(Entity entity, BoundingBox box)
	{
		size_t newNodeIndex = AllocateNode();

		mNodes[newNodeIndex].box = box;
		mNodes[newNodeIndex].height = 0;

		nodeIdxToEntityMap.emplace(newNodeIndex, entity);
		entityToNodeIdxMap.emplace(entity, newNodeIndex);

		InsertLeaf(newNodeIndex);
	}


	inline void DynamicBBTree::RemoveEntity(const Entity entity)
	{
		const auto enIterator = entityToNodeIdxMap.find(entity);
		assert(enIterator != entityToNodeIdxMap.end() && "Trying to remove entity not in map");

		const auto neIterator = nodeIdxToEntityMap.find(enIterator->second);
		assert(neIterator != nodeIdxToEntityMap.end() && "Trying to remove node not in map");

		size_t node = enIterator->second;

		entityToNodeIdxMap.erase(enIterator);
		nodeIdxToEntityMap.erase(neIterator);

		if (node == rootIndex)
		{
			FreeNode(node);
			rootIndex = NULL_NODE;
			return;
		}

		size_t oldParent = mNodes[node].parent;
		size_t sibling = GetSibling(node);

		if (oldParent != rootIndex) // If oldParent isn't root
		{
			// Make oldParent's parent reference sibling as child
			size_t grandfather = mNodes[oldParent].parent;
			if (mNodes[grandfather].left == oldParent) mNodes[grandfather].left = sibling;
			else mNodes[grandfather].right = sibling;

			// Set sibling to oldParent's parent
			mNodes[sibling].parent = grandfather;
		}
		else // If oldParent is root
		{
			// Make sibling the root
			rootIndex = sibling;
			mNodes[sibling].parent = NULL_NODE;
		}

		FreeNode(oldParent);
		FreeNode(node);
	}


	inline void DynamicBBTree::UpdateEntity(Entity entity, BoundingBox box)
	{
		RemoveEntity(entity);
		InsertEntity(entity, box);
	}

	inline void DynamicBBTree::UpdateEntity(Entity entity, glm::vec3 transform)
	{
		BoundingBox box = GetNode(entity).box;
		box.max += transform;
		box.min += transform;
		RemoveEntity(entity);
		InsertEntity(entity, box);
	}


	inline size_t DynamicBBTree::AllocateNode()
	{
		size_t nodeIndex;

		// If there are no holes, either resize capacity or set nodeIndex to next free node.
		if (nodeCount == nodeCapacity)
			ExpandCapacity(nodeCapacity * 2);

		// If nodes array not big enough, resize
		if (mFreeList.empty())
		{
			nodeIndex = nodeCount;
		}
		else
		{
			// If there are "holes" in the mNodes vector caused by removing nodes, fill it.
			// Pulls node off of free list
			nodeIndex = mFreeList.top();
			mFreeList.pop();
		}

		// Set all data members to NULL_NODE
		ResetNodeData(nodeIndex);

		// Increment node count
		nodeCount++;

		return nodeIndex;
	}


	inline void DynamicBBTree::FreeNode(const size_t nodeIndex)
	{
		// Reset data
		ResetNodeData(nodeIndex);

		// Add node to freeList
		mFreeList.push(nodeIndex);

		nodeCount--;
	}


	inline void DynamicBBTree::InsertLeaf(const size_t leafIndex)
	{
		if (rootIndex == NULL_NODE)
		{
			// Makes root node
			rootIndex = leafIndex;
			mNodes[leafIndex].parent = NULL_NODE;
			return;
		}

		// Find best sibling
		size_t sibling = FindBestSibling(leafIndex);

		// Create new parent
		size_t oldParent = mNodes[sibling].parent;
		size_t newParent = AllocateNode();

		// Initialize new parent
		mNodes[newParent].parent = oldParent;
		mNodes[newParent].box.Merge(mNodes[leafIndex].box, mNodes[sibling].box);
		mNodes[newParent].height = mNodes[sibling].height + 1;

		mNodes[newParent].left = sibling;
		mNodes[newParent].right = leafIndex;

		// Set sibling and leaf to point to new parent
		mNodes[sibling].parent = newParent;
		mNodes[leafIndex].parent = newParent;

		// The sibling was not the root.
		if (oldParent != NULL_NODE)
		{
			if (mNodes[oldParent].left == sibling) mNodes[oldParent].left = newParent;
			else mNodes[oldParent].right = newParent;
		}
		// The sibling was the root.
		else
		{
			rootIndex = newParent;
		}

		// Walk back up tree refitting boxes
		size_t iter = mNodes[leafIndex].parent;
		while (iter != NULL_NODE)
		{
			iter = Balance(iter);

			size_t left = mNodes[iter].left;
			size_t right = mNodes[iter].right;


			mNodes[iter].height = std::max(mNodes[left].height, mNodes[right].height);
			mNodes[iter].box.Merge(mNodes[left].box, mNodes[right].box);

			iter = mNodes[iter].parent;
		}
	}


	inline Entity DynamicBBTree::GetObject(size_t nodeIndex)
	{
		const auto iterator = nodeIdxToEntityMap.find(nodeIndex);
		assert(iterator != nodeIdxToEntityMap.end() && "Trying to get node not in map");

		return iterator->second;
	}


	inline size_t DynamicBBTree::GetSibling(size_t nodeIndex)
	{
		const auto& parentNode = mNodes[mNodes[nodeIndex].parent];
		size_t sibling;
		if (parentNode.left == nodeIndex) sibling = parentNode.right;
		else
		{
			sibling = parentNode.left;
			assert(parentNode.right == nodeIndex && "Sibling not found!");
		}
		return sibling;
	}


	inline size_t DynamicBBTree::FindBestSibling(size_t leafIndex) const
	{
		size_t sibling = rootIndex;
		BoundingBox leafBox = mNodes[leafIndex].box;
		while (!IsLeaf(sibling))
		{
			// Surface area of sibling box
			float surfaceArea = mNodes[sibling].box.surfaceArea;

			// Create combined bounding box from inserted box and proposed sibling
			BoundingBox combinedBox;
			combinedBox.Merge(leafBox, mNodes[sibling].box);

			// Surface area of combined box
			float combinedSurfaceArea = combinedBox.surfaceArea;

			// Cost of creating parent and leaf
			float cost = 2.0f * combinedSurfaceArea;

			// Minimum cost of pushing leaf further down the tree
			float inheritedCost = 2.0f * (combinedSurfaceArea - surfaceArea);

			// Get indexes of sibling's children bounding boxes
			size_t left = mNodes[sibling].left;
			size_t right = mNodes[sibling].right;

			// Cost of descending to the left.
			float costLeft;
			if (IsLeaf(left))
			{
				BoundingBox aabb;
				aabb.Merge(leafBox, mNodes[left].box);
				costLeft = aabb.surfaceArea + inheritedCost;
			}
			else
			{
				BoundingBox aabb;
				aabb.Merge(leafBox, mNodes[left].box);
				float oldArea = mNodes[left].box.surfaceArea;
				float newArea = aabb.surfaceArea;
				costLeft = (newArea - oldArea) + inheritedCost;
			}

			// Cost of descending to the right.
			float costRight;
			if (IsLeaf(right))
			{
				BoundingBox aabb;
				aabb.Merge(leafBox, mNodes[right].box);
				costRight = aabb.surfaceArea + inheritedCost;
			}
			else
			{
				BoundingBox aabb;
				aabb.Merge(leafBox, mNodes[right].box);
				float oldArea = mNodes[right].box.surfaceArea;
				float newArea = aabb.surfaceArea;
				costRight = (newArea - oldArea) + inheritedCost;
			}

			// Descend according to the minimum cost.
			if ((cost < costLeft) && (cost < costRight)) break;

			// Descend.
			if (costLeft < costRight) sibling = left;
			else sibling = right;
		}
		return sibling;
	}


	inline std::vector<Entity> DynamicBBTree::ComputeCollisionPairs()
	{
		std::vector<Entity> output;
		std::stack<std::pair<size_t, size_t>> stack;

		if (nodeCount <= 1) return output;

		stack.emplace(mNodes[rootIndex].left, mNodes[rootIndex].right);

		while (!stack.empty())
		{
			size_t n1_idx = stack.top().first;
			size_t n2_idx = stack.top().second;

			const auto& n1 = mNodes[n1_idx];
			const auto& n2 = mNodes[n2_idx];
			stack.pop();

			if (IsInternal(n1_idx) && IsInternal(n2_idx))
			{
				stack.emplace(n1.left, n1.right);
				stack.emplace(n2.left, n2.right);

				if (n1.box.IsColliding(n2.box))
				{
					stack.emplace(n1.left, n2.left);
					stack.emplace(n1.left, n2.right);
					stack.emplace(n1.right, n2.left);
					stack.emplace(n1.right, n2.right);
				}
			}
			else if (IsInternal(n1_idx))
			{
				stack.emplace(n1.left, n1.right);
				if (n1.box.IsColliding(n2.box))
				{
					stack.emplace(n1.left, n2_idx);
					stack.emplace(n1.right, n2_idx);
				}
			}
			else if (IsInternal(n2_idx))
			{
				stack.emplace(n2.left, n2.right);
				if (n1.box.IsColliding(n2.box))
				{
					stack.emplace(n1_idx, n2.left);
					stack.emplace(n1_idx, n2.right);
				}
			}
			else if (n1.box.IsColliding(n2.box))
			{
				output.emplace_back(GetObject(n1_idx));
				output.emplace_back(GetObject(n2_idx));
			}
		}
		return output;
	}


	inline void DynamicBBTree::ExpandCapacity(const size_t newNodeCapacity)
	{
		assert(newNodeCapacity > nodeCapacity);

		nodeCapacity = newNodeCapacity;

		// Resize nodes vector
		mNodes.resize(nodeCapacity);
	}


	inline bool DynamicBBTree::IsLeaf(const size_t index) const
	{
		if (mNodes[index].left == NULL_NODE && mNodes[index].right == NULL_NODE) assert(mNodes[index].height == 0 && "Leaf");
		return mNodes[index].left == NULL_NODE && mNodes[index].right == NULL_NODE;
	}


	inline bool DynamicBBTree::IsInternal(size_t nodeIndex) const
	{
		return !IsLeaf(nodeIndex);
	}


	inline size_t DynamicBBTree::Balance(const size_t node)
	{
		// If node is a leaf or height = 0
		if (IsLeaf(node))
			return node;

		size_t left = mNodes[node].left;
		size_t right = mNodes[node].right;

		int currentBalance = static_cast<int>(mNodes[right].height - mNodes[left].height);

		// Rotate right branch up.
		if (currentBalance > 1)
		{
			// Store these for later
			size_t rightLeft = mNodes[right].left;
			size_t rightRight = mNodes[right].right;

			// Swap node and its right-hand child.
			mNodes[right].left = node;
			mNodes[right].parent = mNodes[node].parent;
			mNodes[node].parent = right;

			size_t nodeOldParent = mNodes[right].parent;

			// Make node's old parent point to its right-hand child.
			if (nodeOldParent != NULL_NODE)
			{
				if (mNodes[nodeOldParent].left == node) mNodes[nodeOldParent].left = right;
				else
				{
					mNodes[nodeOldParent].right = right;
				}
			}
			else rootIndex = right;

			// Rotate.
			if (mNodes[rightLeft].height > mNodes[rightRight].height)
			{
				mNodes[right].right = rightLeft;
				mNodes[node].right = rightRight;

				mNodes[rightRight].parent = node;

				mNodes[node].box.Merge(mNodes[left].box, mNodes[rightRight].box);
				mNodes[right].box.Merge(mNodes[node].box, mNodes[rightLeft].box);

				mNodes[node].height = std::max(mNodes[left].height, mNodes[rightRight].height);
				mNodes[right].height = std::max(mNodes[node].height, mNodes[rightLeft].height);
			}
			else
			{
				mNodes[right].right = rightRight;
				mNodes[node].right = rightLeft;

				mNodes[rightLeft].parent = node;

				mNodes[node].box.Merge(mNodes[left].box, mNodes[rightLeft].box);
				mNodes[right].box.Merge(mNodes[node].box, mNodes[rightRight].box);

				mNodes[node].height = std::max(mNodes[left].height, mNodes[rightLeft].height);
				mNodes[right].height = std::max(mNodes[node].height, mNodes[rightRight].height);
			}
			return right;
		}

		// Rotate left branch up.
		if (currentBalance < -1)
		{
			size_t leftLeft = mNodes[left].left;
			size_t leftRight = mNodes[left].right;

			assert(leftLeft < nodeCapacity);
			assert(leftRight < nodeCapacity);

			// Swap node and its left-hand child.
			mNodes[left].left = node;
			mNodes[left].parent = mNodes[node].parent;
			mNodes[node].parent = left;

			// The node's old parent should now point to its left-hand child.
			if (mNodes[left].parent != NULL_NODE)
			{
				if (mNodes[mNodes[left].parent].left == node) mNodes[mNodes[left].parent].left = left;
				else
				{
					assert(mNodes[mNodes[left].parent].right == node);
					mNodes[mNodes[left].parent].right = left;
				}
			}
			else rootIndex = left;

			// Rotate.
			if (mNodes[leftLeft].height > mNodes[leftRight].height)
			{
				mNodes[left].right = leftLeft;
				mNodes[node].left = leftRight;
				mNodes[leftRight].parent = node;
				mNodes[node].box.Merge(mNodes[right].box, mNodes[leftRight].box);
				mNodes[left].box.Merge(mNodes[node].box, mNodes[leftLeft].box);

				mNodes[node].height = std::max(mNodes[right].height, mNodes[leftRight].height);
				mNodes[left].height = std::max(mNodes[node].height, mNodes[leftLeft].height);
			}
			else
			{
				mNodes[left].right = leftRight;
				mNodes[node].left = leftLeft;
				mNodes[leftLeft].parent = node;
				mNodes[node].box.Merge(mNodes[right].box, mNodes[leftLeft].box);
				mNodes[left].box.Merge(mNodes[node].box, mNodes[leftRight].box);

				mNodes[node].height = std::max(mNodes[right].height, mNodes[leftLeft].height);
				mNodes[left].height = std::max(mNodes[node].height, mNodes[leftRight].height);
			}

			return left;
		}

		return node;
	}


	inline const BoundingBox& DynamicBBTree::GetBoundingBox(const Entity object) const
	{
		const auto enIterator = entityToNodeIdxMap.find(object);
		if (enIterator != entityToNodeIdxMap.end())
		{
			LOG(LOG_ERROR) << "Trying to get object not in map\n";
			return BoundingBox{};
		}
		return mNodes[enIterator->second].box;
	}

	inline std::vector<BoundingBox> DynamicBBTree::GetAllBoxes(const bool onlyLeaf) const
	{
		std::vector<BoundingBox> output;
		for (size_t i = 0; i < nodeCount; i++)
		{
			if ((!onlyLeaf || IsLeaf(i)) && mNodes[i].height != NULL_NODE)
				output.emplace_back(mNodes[i].box);
		}
		return output;
	}

	inline DynamicBBTree::Node& DynamicBBTree::GetNode(Entity entity)
	{
		const auto enIterator = entityToNodeIdxMap.find(entity);
		if (enIterator != entityToNodeIdxMap.end())
		{
			LOG(LOG_ERROR) << "Trying to find entity not in map\n";
			return mNodes[NULL_NODE];
		}
		return mNodes[enIterator->second];
	}


	inline void DynamicBBTree::ResetNodeData(const size_t nodeIndex)
	{
		mNodes[nodeIndex].box = BoundingBox{};
		mNodes[nodeIndex].parent = NULL_NODE;
		mNodes[nodeIndex].left = NULL_NODE;
		mNodes[nodeIndex].right = NULL_NODE;
		mNodes[nodeIndex].height = NULL_NODE;
	}
}

