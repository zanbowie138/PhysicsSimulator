#include "BoundingBoxTree.h"

BoundingBoxTree::BoundingBoxTree(unsigned int initialCapacity)
{
	rootIndex = NULL_NODE;
	nodeCount = 0;
	nodeCapacity = 0;

	ExpandCapacity(initialCapacity);
}

unsigned int BoundingBoxTree::AllocateNode()
{
    // If nodes array not big enough, resize
	if (nodes[freeList].next == NULL_NODE)
	{
		ExpandCapacity(nodeCapacity * 2);
	}

    // Pull node off of free list
	unsigned int nodeIndex = freeList;
	freeList = nodes[nodeIndex].next;

    // Set all data members to NULL_NODE
    ResetNodeData(nodeIndex);

    // Increment node count
	nodeCount++;

	return nodeIndex;
}

void BoundingBoxTree::FreeNode(unsigned int nodeIndex)
{
    // Reset data
	ResetNodeData(nodeIndex);

    // Add node to freeList
	nodes[nodeIndex].next = freeList;
    freeList = nodeIndex;

	nodeCount--;
}

void BoundingBoxTree::InsertLeaf(unsigned int leafIndex)
{
	if (rootIndex == NULL_NODE)
	{
        // Makes root node
		rootIndex = leafIndex;
		nodes[leafIndex].parent = NULL_NODE;
		return;
	}

	// Find best sibling
	unsigned int sibling = rootIndex;
	BoundingBox leafBox = nodes[leafIndex].box;
	while (IsLeaf(sibling))
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
		unsigned int left = nodes[sibling].left;
		unsigned int right = nodes[sibling].right;

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
		index = Balance(index);

		unsigned int left = nodes[index].left;
		unsigned int right = nodes[index].right;

		nodes[index].height = 1 + std::max(nodes[left].height, nodes[right].height);
		nodes[index].box.Merge(nodes[left].box, nodes[right].box);

		index = nodes[index].parent;
	}
	nodeCount++;
}

void BoundingBoxTree::TreeQuery(unsigned int node)
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

void BoundingBoxTree::ExpandCapacity(unsigned int newNodeCapacity) 
{
	if (newNodeCapacity <= nodeCapacity)
	{
		throw std::invalid_argument("newNodeCapacity parameter must be greater than or equal to current nodeCapacity.");
	}

	nodeCapacity = newNodeCapacity;

	// Resize nodes vector
	nodes.resize(nodeCapacity);

	// Expand linked list of free nodes
	for (unsigned int i = nodeCount; i < nodeCapacity; i++) {
		nodes[i].next = i+1;
		nodes[i].height = NULL_NODE;
	}

	// Create end of list
	nodes[nodeCapacity-1].next = NULL_NODE;
    nodes[nodeCapacity-1].height = NULL_NODE;

	freeList = nodeCount;
}

bool BoundingBoxTree::IsLeaf(unsigned int index)
{
	return !(nodes[index].left == NULL_NODE && nodes[index].right == NULL_NODE);
}

unsigned int BoundingBoxTree::Balance(unsigned int node)
    {
        if (IsLeaf(node) || (nodes[node].height <= 1))
            return node;

        unsigned int left = nodes[node].left;
        unsigned int right = nodes[node].right;

        int currentBalance = nodes[right].height - nodes[left].height;

        // Rotate right branch up.
        if (currentBalance > 1)
        {
            unsigned int rightLeft = nodes[right].left;
            unsigned int rightRight = nodes[right].right;

            // Swap node and its right-hand child.
            nodes[right].left = node;
            nodes[right].parent = nodes[node].parent;
            nodes[node].parent = right;

            // The node's old parent should now point to its right-hand child.
            if (nodes[right].parent != NULL_NODE)
            {
                if (nodes[nodes[right].parent].left == node) nodes[nodes[right].parent].left = right;
                else
                {
                    nodes[nodes[right].parent].right = right;
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
            unsigned int leftLeft = nodes[left].left;
            unsigned int leftRight = nodes[left].right;

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

    void BoundingBoxTree::ResetNodeData(unsigned int node)
    {
        nodes[node].parent = NULL_NODE;
        nodes[node].next = NULL_NODE;
	    nodes[node].left = NULL_NODE;
	    nodes[node].right = NULL_NODE;
	    nodes[node].height = NULL_NODE;
    }