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

public:
	BoundingBox(): max(glm::vec3(0.0f)), min(glm::vec3(0.0f)), surfaceArea(0.0f){}
	BoundingBox(const glm::vec3 max, const glm::vec3 min);

	void Merge(const BoundingBox& box1, const BoundingBox& box2);
	void Merge(const BoundingBox& other);
	void IncludePoint(const glm::vec3& point);

	std::string String() const;

	void SetToLimit();

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

inline void BoundingBox::Merge(const BoundingBox& other)
{
	for (unsigned int d = 0; d < 3; d++) {
		min[d] = std::min(min[d], other.min[d]);
		max[d] = std::max(max[d], other.max[d]);
	}
	UpdateSurfaceArea();
}

inline void BoundingBox::IncludePoint(const glm::vec3& point)
{
	for (unsigned int d = 0; d < 3; d++) {
		min[d] = std::min(point[d], min[d]);
		max[d] = std::max(point[d], max[d]);
	}
}

inline std::string BoundingBox::String() const 
{
	return "min: " + glm::to_string(min) + "\n" + "max: " + glm::to_string(max) + "\n";
}

inline void BoundingBox::SetToLimit()
{
	max = glm::vec3(-FLT_MAX);
	min = glm::vec3(FLT_MAX);
}

inline void BoundingBox::UpdateSurfaceArea()
{
	surfaceArea = 2.0f*(max.x-min.x)*(max.y-min.y) + 2.0f*(max.y-min.y)*(max.z-min.z) + 2.0f*(max.x-min.x)*(max.z-min.z);
}
