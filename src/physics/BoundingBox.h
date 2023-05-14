#pragma once
#include <glm/glm.hpp>
#include <memory>

class BoundingBox
{
public:
	// Corner points
	glm::vec3 max;
	glm::vec3 min;

	float surfaceArea;

	BoundingBox(): max(glm::vec3(0.0f)), min(glm::vec3(0.0f)){}
	BoundingBox(const glm::vec3 max, const glm::vec3 min);

	void Merge(const BoundingBox& box1, const BoundingBox& box2);

	bool IsColliding(const BoundingBox& other) const;
	void UpdateSurfaceArea();
};

inline BoundingBox::BoundingBox(const glm::vec3 max, const glm::vec3 min): max(max), min(min)
{
	UpdateSurfaceArea();
}

inline bool BoundingBox::IsColliding(const BoundingBox& other) const
{
	return min.x <= other.max.x &&
		max.x >= other.min.x &&
		min.y <= other.max.y &&
		max.y >= other.min.y &&
		min.z <= other.max.z &&
		max.z >= other.min.z;
}

inline void BoundingBox::Merge(const BoundingBox& box1, const BoundingBox& box2)
{
	for (unsigned int d = 0; d < 3; d++) {
		min[d] = std::min(box1.min[d], box2.min[d]);
		max[d] = std::max(box1.max[d], box2.max[d]);
	}
	UpdateSurfaceArea();
}

inline void BoundingBox::UpdateSurfaceArea()
{
	surfaceArea = 2.0f*(max.x-min.x)*(max.y-min.y) + 2.0f*(max.y-min.y)*(max.z-min.z) + 2.0f*(max.x-min.x)*(max.z-min.z);
}
