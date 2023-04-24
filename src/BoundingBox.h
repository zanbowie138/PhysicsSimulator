#pragma once
#include <glm/glm.hpp>
#include <memory>

class BoundingBox
{
public:
	// Corner points
	glm::vec3 max;
	glm::vec3 min;

	BoundingBox* parent;
	BoundingBox* child1;
	BoundingBox* child2;

	bool isLeaf;

	BoundingBox(const glm::vec3 max, const glm::vec3 min): max(max), min(min){}

	bool IsColliding(const BoundingBox& other) const;
};

inline bool BoundingBox::IsColliding(const BoundingBox& other) const
{
	return min.x <= other.max.x &&
		max.x >= other.min.x &&
		min.y <= other.max.y &&
		max.y >= other.min.y &&
		min.z <= other.max.z &&
		max.z >= other.min.z;
}
