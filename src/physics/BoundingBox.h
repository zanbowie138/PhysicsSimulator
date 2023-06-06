#pragma once
#include <glm/glm.hpp>
#include <memory>

class BoundingBox
{
public:
	// Corner points
	glm::vec3 min;
	glm::vec3 max;

	float surfaceArea;

public:
	BoundingBox();
	BoundingBox(const glm::vec3 min, const glm::vec3 max);

	void Merge(const BoundingBox& box1, const BoundingBox& box2);
	void Merge(const BoundingBox& other);
	void IncludePoint(const glm::vec3 point);

	void Scale(const glm::mat4& mat);

	std::string String() const;

	void Reset();
	void SetToLimit();

	bool IsColliding(const BoundingBox& other) const;
	void UpdateSurfaceArea();
};

inline BoundingBox::BoundingBox()
{
	SetToLimit();
}

inline BoundingBox::BoundingBox(const glm::vec3 min, const glm::vec3 max): min(min), max(max)
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

inline void BoundingBox::IncludePoint(const glm::vec3 point)
{
	for (unsigned int d = 0; d < 3; d++) {
		min[d] = std::min(point[d], min[d]);
		max[d] = std::max(point[d], max[d]);
	}
}

inline void BoundingBox::Scale(const glm::mat4& mat)
{
	min = mat* glm::vec4(min, 1.0);
	max = mat * glm::vec4(min, 1.0);
}

inline std::string BoundingBox::String() const 
{
	return "min: " + glm::to_string(min) + "\n" + "max: " + glm::to_string(max) + "\n";
}

inline void BoundingBox::Reset()
{
	max = glm::vec3(0.0f);
	min = glm::vec3(0.0f);
	surfaceArea = 0.0f;
}

inline void BoundingBox::SetToLimit()
{
	max = glm::vec3(-FLT_MAX);
	min = glm::vec3(FLT_MAX);
	surfaceArea = 0.0f;
}

inline void BoundingBox::UpdateSurfaceArea()
{
	surfaceArea = (max.x-min.x)*(max.y-min.y) + (max.y-min.y)*(max.z-min.z) + (max.x-min.x)*(max.z-min.z);
}
