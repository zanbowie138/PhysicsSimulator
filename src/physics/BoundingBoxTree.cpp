#include "BoundingBoxTree.h"

#include <iostream>

BoundingBoxTree::BoundingBoxTree(const size_t initialCapacity)
{
	rootIndex = NULL_NODE;
	nodeCount = 0;
	nodeCapacity = 0;

	ExpandCapacity(initialCapacity);
}

void BoundingBoxTree::InsertEntity(Entity entity, BoundingBox box)
{
	size_t newNodeIndex = AllocateNode();
	mNodes[newNodeIndex].box = box;

	nodeToEntityMap.emplace(newNodeIndex, entity);
	entityToNodeMap.emplace(entity, newNodeIndex);

	InsertLeaf(newNodeIndex);
}

void BoundingBoxTree::RemoveEntity(const Entity entity)
{
	const auto enIterator = entityToNodeMap.find(entity);
	assert(enIterator != entityToNodeMap.end() && "Trying to remove entity not in map");

	const auto neIterator = nodeToEntityMap.find(enIterator->second);
	assert(neIterator != nodeToEntityMap.end() && "Trying to remove node not in map");

	size_t node = enIterator->second;
	size_t oldParent = mNodes[node].parent;
	size_t sibling;

	// Find sibling
	if (mNodes[oldParent].left == node)
		sibling = mNodes[oldParent].right;
	else sibling = mNodes[oldParent].left;

	// Make oldParent's parent reference sibling as child
	size_t grandfather = mNodes[oldParent].parent;
	if (mNodes[grandfather].left == oldParent) mNodes[grandfather].left = sibling;
	else mNodes[grandfather].right = sibling;

	// Set sibling to oldParent's parent
	mNodes[sibling].parent = grandfather;

	FreeNode(oldParent);
	FreeNode(enIterator->second);

	entityToNodeMap.erase(enIterator);
	nodeToEntityMap.erase(neIterator);
}

size_t BoundingBoxTree::AllocateNode()
{
	// If nodes array not big enough, resize
	if (mNodes[freeList].next == NULL_NODE)
	{
		ExpandCapacity(nodeCapacity * 2);
	}

	// Pull node off of free list
	size_t nodeIndex = freeList;
	freeList = mNodes[nodeIndex].next;

	// Set all data members to NULL_NODE
	ResetNodeData(nodeIndex);

	// Increment node count
	nodeCount++;

	return nodeIndex;
}

void BoundingBoxTree::FreeNode(const size_t nodeIndex)
{
	// Reset data
	ResetNodeData(nodeIndex);

	// Add node to freeList
	mNodes[nodeIndex].next = freeList;
	freeList = nodeIndex;

	nodeCount--;
}

void BoundingBoxTree::InsertLeaf(const size_t leafIndex)
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

	// TODO: Check this logic
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


		mNodes[iter].height = 1 + std::max(mNodes[left].height, mNodes[right].height);
		mNodes[iter].box.Merge(mNodes[left].box, mNodes[right].box);

		iter = mNodes[iter].parent;
	}
	nodeCount++;
}

size_t BoundingBoxTree::FindBestSibling(size_t leafIndex) const
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
		double costLeft;
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
			double oldArea = mNodes[left].box.surfaceArea;
			double newArea = aabb.surfaceArea;
			costLeft = (newArea - oldArea) + inheritedCost;
		}

		// Cost of descending to the right.
		double costRight;
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
			double oldArea = mNodes[right].box.surfaceArea;
			double newArea = aabb.surfaceArea;
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

void BoundingBoxTree::TreeQuery(const size_t node)
{
	const auto& n = mNodes[node];
	const auto& leftNode = mNodes[n.left];
	const auto& rightNode = mNodes[n.right];

	if (IsLeaf(n.left) && IsLeaf(n.right)) // Checks if children are leaves
	{
		if (leftNode.box.IsColliding(rightNode.box))
		{
			mCollisions.push_back(n.left);
			mCollisions.push_back(n.right);
		}
	}
	else
	{
		TreeQuery(mNodes[node].left);
		TreeQuery(mNodes[node].right);
	}
}

void BoundingBoxTree::ComputePairs()
{
	mCollisions.clear();

	if (nodeCount <= 1) return;

	TreeQuery(rootIndex);
}

void BoundingBoxTree::ExpandCapacity(const size_t newNodeCapacity)
{
	assert(newNodeCapacity > nodeCapacity);

	nodeCapacity = newNodeCapacity;

	// Resize nodes vector
	mNodes.resize(nodeCapacity);

	// Expand linked list of free nodes
	for (size_t i = nodeCount; i < nodeCapacity; i++)
	{
		mNodes[i].next = i + 1;
		mNodes[i].height = NULL_NODE;
	}

	// Create end of list
	mNodes[nodeCapacity - 1].next = NULL_NODE;
	mNodes[nodeCapacity - 1].height = NULL_NODE;

	freeList = nodeCount;
}

bool BoundingBoxTree::IsLeaf(const size_t index) const
{
	return mNodes[index].left == NULL_NODE && mNodes[index].right == NULL_NODE;
}

size_t BoundingBoxTree::Balance(const size_t node)
{
	// If node is a leaf or height = 0
	// TODO: Check this
	if (IsLeaf(node) || (mNodes[node].height == 0))
		return node;

	size_t left = mNodes[node].left;
	size_t right = mNodes[node].right;

	int currentBalance = mNodes[right].height - mNodes[left].height;

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

			mNodes[node].height = 1 + std::max(mNodes[left].height, mNodes[rightRight].height);
			mNodes[right].height = 1 + std::max(mNodes[node].height, mNodes[rightLeft].height);
		}
		else
		{
			mNodes[right].right = rightRight;
			mNodes[node].right = rightLeft;

			mNodes[rightLeft].parent = node;

			mNodes[node].box.Merge(mNodes[left].box, mNodes[rightLeft].box);
			mNodes[right].box.Merge(mNodes[node].box, mNodes[rightRight].box);

			mNodes[node].height = 1 + std::max(mNodes[left].height, mNodes[rightLeft].height);
			mNodes[right].height = 1 + std::max(mNodes[node].height, mNodes[rightRight].height);
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

			mNodes[node].height = 1 + std::max(mNodes[right].height, mNodes[leftRight].height);
			mNodes[left].height = 1 + std::max(mNodes[node].height, mNodes[leftLeft].height);
		}
		else
		{
			mNodes[left].right = leftRight;
			mNodes[node].left = leftLeft;
			mNodes[leftLeft].parent = node;
			mNodes[node].box.Merge(mNodes[right].box, mNodes[leftLeft].box);
			mNodes[left].box.Merge(mNodes[node].box, mNodes[leftRight].box);

			mNodes[node].height = 1 + std::max(mNodes[right].height, mNodes[leftLeft].height);
			mNodes[left].height = 1 + std::max(mNodes[node].height, mNodes[leftRight].height);
		}

		return left;
	}

	return node;
}

const BoundingBox& BoundingBoxTree::GetBoundingBox(const Entity entity) const
{
	const auto iterator = entityToNodeMap.find(entity);
	assert(iterator != entityToNodeMap.end() && "Entity not added to tree");

	return mNodes[iterator->second].box;
}

std::vector<BoundingBox> BoundingBoxTree::GetAllBoxes(const bool onlyLeaf) const
{
	std::vector<BoundingBox> output;
	for (size_t i = 0; i < mNodes.size(); i++)
	{
		if (mNodes[i].box.min != glm::vec3(0.0f) && mNodes[i].box.max != glm::vec3(0.0f) && (!onlyLeaf || IsLeaf(i)))
			output.emplace_back(mNodes[i].box);
	}
	return output;
}

void BoundingBoxTree::ResetNodeData(const size_t nodeIndex)
{
	mNodes[nodeIndex].parent = NULL_NODE;
	mNodes[nodeIndex].next = NULL_NODE;
	mNodes[nodeIndex].left = NULL_NODE;
	mNodes[nodeIndex].right = NULL_NODE;
	mNodes[nodeIndex].height = 0;
}
