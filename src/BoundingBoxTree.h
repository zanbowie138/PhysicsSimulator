#pragma once
#include "BoundingBox.h"
class BoundingBoxTree
{
public:
	BoundingBox** rootNode;

	unsigned int nodeCount;

	void BoundingBoxTree() : rootNode(nullptr), nodeCount(0){}

	void InsertNode(BoundingBox* box);
};

inline void BoundingBoxTree::InsertNode(BoundingBox* box)
{
	if (nodeCount == 0) rootNode = &box;
}
