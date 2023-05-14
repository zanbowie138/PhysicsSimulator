#include "BoundingBoxTree.h"

#include <iostream>

BoundingBoxTree::BoundingBoxTree(size_t initialCapacity)
{
	rootIndex = NULL_NODE;
	nodeCount = 0;
	nodeCapacity = 0;

	ExpandCapacity(initialCapacity);
}

void BoundingBoxTree::InsertEntity(Entity entity, BoundingBox box)
{
	size_t newNodeIndex = AllocateNode();
	nodes[newNodeIndex].box = box;

	nodeToEntityMap.emplace(newNodeIndex, entity);
	entityToNodeMap.emplace(entity, newNodeIndex);

	InsertLeaf(newNodeIndex);
}

void BoundingBoxTree::RemoveEntity(Entity entity)
{
	const auto enIterator = entityToNodeMap.find(entity);
	assert(enIterator != entityToNodeMap.end() && "Trying to remove entity not in map");

	const auto neIterator = nodeToEntityMap.find(enIterator->second);
	assert(neIterator != nodeToEntityMap.end() && "Trying to remove node not in map");

	FreeNode(enIterator->second);

	entityToNodeMap.erase(enIterator);
	nodeToEntityMap.erase(neIterator);
}

size_t BoundingBoxTree::AllocateNode()
{
	// If nodes array not big enough, resize
	if (nodes[freeList].next == NULL_NODE)
	{
		ExpandCapacity(nodeCapacity * 2);
	}

	// Pull node off of free list
	size_t nodeIndex = freeList;
	freeList = nodes[nodeIndex].next;

	// Set all data members to NULL_NODE
	ResetNodeData(nodeIndex);

	// Increment node count
	nodeCount++;

	return nodeIndex;
}

void BoundingBoxTree::FreeNode(size_t nodeIndex)
{
	// Reset data
	ResetNodeData(nodeIndex);

	// Add node to freeList
	nodes[nodeIndex].next = freeList;
	freeList = nodeIndex;

	nodeCount--;
}

void BoundingBoxTree::InsertLeaf(size_t leafIndex)
{
	if (rootIndex == NULL_NODE)
	{
		// Makes root node
		rootIndex = leafIndex;
		nodes[leafIndex].parent = NULL_NODE;
		return;
	}

	// Find best sibling
	size_t sibling = FindBestSibling(leafIndex);

	// Create new parent
	size_t oldParent = nodes[sibling].parent;
	size_t newParent = AllocateNode();

	// Initialize new parent
	nodes[newParent].parent = oldParent;
	nodes[newParent].box.Merge(nodes[leafIndex].box, nodes[sibling].box);
	nodes[newParent].height = nodes[sibling].height + 1;

	nodes[newParent].left = sibling;
	nodes[newParent].right = leafIndex;

	// Set sibling and leaf to point to new parent
	nodes[sibling].parent = newParent;
	nodes[leafIndex].parent = newParent;

	// TODO: Check this logic
	// The sibling was not the root.
	if (oldParent != NULL_NODE)
	{
		if (nodes[oldParent].left == sibling) nodes[oldParent].left = newParent;
		else nodes[oldParent].right = newParent;
	}
	// The sibling was the root.
	else
	{
		rootIndex = newParent;
	}

	// Walk back up tree refitting boxes
	size_t iter = nodes[leafIndex].parent;
	while (iter != NULL_NODE)
	{
		iter = Balance(iter);

		size_t left = nodes[iter].left;
		size_t right = nodes[iter].right;


		nodes[iter].height = 1 + std::max(nodes[left].height, nodes[right].height);
		nodes[iter].box.Merge(nodes[left].box, nodes[right].box);

		iter = nodes[iter].parent;
	}
	nodeCount++;
}

size_t BoundingBoxTree::FindBestSibling(size_t leafIndex) const
{
	size_t sibling = rootIndex;
	BoundingBox leafBox = nodes[leafIndex].box;
	while (!IsLeaf(sibling))
	{
		// Surface area of sibling box
		float surfaceArea = nodes[sibling].box.surfaceArea;

		// Create combined bounding box from inserted box and proposed sibling
		BoundingBox combinedBox;
		combinedBox.Merge(leafBox, nodes[sibling].box);

		// Surface area of combined box
		float combinedSurfaceArea = combinedBox.surfaceArea;

		// Cost of creating parent and leaf
		float cost = 2.0f * combinedSurfaceArea;

		// Minimum cost of pushing leaf further down the tree
		float inheritedCost = 2.0f * (combinedSurfaceArea - surfaceArea);

		// Get indexes of sibling's children bounding boxes
		size_t left = nodes[sibling].left;
		size_t right = nodes[sibling].right;

		// Cost of descending to the left.
		double costLeft;
		if (IsLeaf(left))
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[left].box);
			costLeft = aabb.surfaceArea + inheritedCost;
		}
		else
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[left].box);
			double oldArea = nodes[left].box.surfaceArea;
			double newArea = aabb.surfaceArea;
			costLeft = (newArea - oldArea) + inheritedCost;
		}

		// Cost of descending to the right.
		double costRight;
		if (IsLeaf(right))
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[right].box);
			costRight = aabb.surfaceArea + inheritedCost;
		}
		else
		{
			BoundingBox aabb;
			aabb.Merge(leafBox, nodes[right].box);
			double oldArea = nodes[right].box.surfaceArea;
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

void BoundingBoxTree::TreeQuery(size_t node)
{
	if (nodeCount == 0) return;

	if (nodes[node].box.IsColliding(nodes[node].box))
	{
		if (IsLeaf(node))
		{
			// TODO: ADD TO STACK
		}
		else
		{
			TreeQuery(nodes[node].left);
			TreeQuery(nodes[node].right);
		}
	}
}

void BoundingBoxTree::ExpandCapacity(size_t newNodeCapacity)
{
	assert(newNodeCapacity > nodeCapacity);

	nodeCapacity = newNodeCapacity;

	// Resize nodes vector
	nodes.resize(nodeCapacity);

	// Expand linked list of free nodes
	for (size_t i = nodeCount; i < nodeCapacity; i++)
	{
		nodes[i].next = i + 1;
		nodes[i].height = NULL_NODE;
	}

	// Create end of list
	nodes[nodeCapacity - 1].next = NULL_NODE;
	nodes[nodeCapacity - 1].height = NULL_NODE;

	freeList = nodeCount;
}

bool BoundingBoxTree::IsLeaf(size_t index) const
{
	return nodes[index].left == NULL_NODE && nodes[index].right == NULL_NODE;
}

size_t BoundingBoxTree::Balance(size_t node)
{
	// If node is a leaf or height = 0
	// TODO: Check this
	if (IsLeaf(node) || (nodes[node].height == 0))
		return node;

	size_t left = nodes[node].left;
	size_t right = nodes[node].right;

	int currentBalance = nodes[right].height - nodes[left].height;

	// Rotate right branch up.
	if (currentBalance > 1)
	{
		// Store these for later
		size_t rightLeft = nodes[right].left;
		size_t rightRight = nodes[right].right;

		// Swap node and its right-hand child.
		nodes[right].left = node;
		nodes[right].parent = nodes[node].parent;
		nodes[node].parent = right;

		size_t nodeOldParent = nodes[right].parent;

		// Make node's old parent point to its right-hand child.
		if (nodeOldParent != NULL_NODE)
		{
			if (nodes[nodeOldParent].left == node) nodes[nodeOldParent].left = right;
			else
			{
				nodes[nodeOldParent].right = right;
			}
		}
		else rootIndex = right;

		// Rotate.
		if (nodes[rightLeft].height > nodes[rightRight].height)
		{
			nodes[right].right = rightLeft;
			nodes[node].right = rightRight;

			nodes[rightRight].parent = node;

			nodes[node].box.Merge(nodes[left].box, nodes[rightRight].box);
			nodes[right].box.Merge(nodes[node].box, nodes[rightLeft].box);

			nodes[node].height = 1 + std::max(nodes[left].height, nodes[rightRight].height);
			nodes[right].height = 1 + std::max(nodes[node].height, nodes[rightLeft].height);
		}
		else
		{
			nodes[right].right = rightRight;
			nodes[node].right = rightLeft;

			nodes[rightLeft].parent = node;

			nodes[node].box.Merge(nodes[left].box, nodes[rightLeft].box);
			nodes[right].box.Merge(nodes[node].box, nodes[rightRight].box);

			nodes[node].height = 1 + std::max(nodes[left].height, nodes[rightLeft].height);
			nodes[right].height = 1 + std::max(nodes[node].height, nodes[rightRight].height);
		}
		return right;
	}

	// Rotate left branch up.
	if (currentBalance < -1)
	{
		size_t leftLeft = nodes[left].left;
		size_t leftRight = nodes[left].right;

		assert(leftLeft < nodeCapacity);
		assert(leftRight < nodeCapacity);

		// Swap node and its left-hand child.
		nodes[left].left = node;
		nodes[left].parent = nodes[node].parent;
		nodes[node].parent = left;

		// The node's old parent should now point to its left-hand child.
		if (nodes[left].parent != NULL_NODE)
		{
			if (nodes[nodes[left].parent].left == node) nodes[nodes[left].parent].left = left;
			else
			{
				assert(nodes[nodes[left].parent].right == node);
				nodes[nodes[left].parent].right = left;
			}
		}
		else rootIndex = left;

		// Rotate.
		if (nodes[leftLeft].height > nodes[leftRight].height)
		{
			nodes[left].right = leftLeft;
			nodes[node].left = leftRight;
			nodes[leftRight].parent = node;
			nodes[node].box.Merge(nodes[right].box, nodes[leftRight].box);
			nodes[left].box.Merge(nodes[node].box, nodes[leftLeft].box);

			nodes[node].height = 1 + std::max(nodes[right].height, nodes[leftRight].height);
			nodes[left].height = 1 + std::max(nodes[node].height, nodes[leftLeft].height);
		}
		else
		{
			nodes[left].right = leftRight;
			nodes[node].left = leftLeft;
			nodes[leftLeft].parent = node;
			nodes[node].box.Merge(nodes[right].box, nodes[leftLeft].box);
			nodes[left].box.Merge(nodes[node].box, nodes[leftRight].box);

			nodes[node].height = 1 + std::max(nodes[right].height, nodes[leftLeft].height);
			nodes[left].height = 1 + std::max(nodes[node].height, nodes[leftRight].height);
		}

		return left;
	}

	return node;
}

const BoundingBox& BoundingBoxTree::GetBoundingBox(Entity entity) const
{
	const auto iterator = entityToNodeMap.find(entity);
	assert(iterator != entityToNodeMap.end() && "Entity not added to tree");

	return nodes[iterator->second].box;
}

std::vector<BoundingBox> BoundingBoxTree::GetAllBoxes(const bool onlyLeaf) const
{
	std::vector<BoundingBox> output;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].box.min != glm::vec3(0.0f) && nodes[i].box.max != glm::vec3(0.0f) && (!onlyLeaf || IsLeaf(i)))
			output.emplace_back(nodes[i].box);
	}
	return output;
}

void BoundingBoxTree::ResetNodeData(const size_t nodeIndex)
{
	nodes[nodeIndex].parent = NULL_NODE;
	nodes[nodeIndex].next = NULL_NODE;
	nodes[nodeIndex].left = NULL_NODE;
	nodes[nodeIndex].right = NULL_NODE;
	nodes[nodeIndex].height = 0;
}
