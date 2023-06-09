#pragma once

#include <vector>

#include "Renderable.h"
#include "../renderer/VBO.h"
#include "../renderer/EBO.h"
#include "../physics/BoundingBox.h"

class Points : public Renderable
{
public:
	std::vector<glm::vec3> points;

	VBO VBO;

	explicit inline Points(GLuint amount);

	inline void PushBack(const std::vector<glm::vec3>& pts);

private:
	void InitVAO() override;
	size_t GetSize() override;
};

Points::Points(const GLuint amount)
{
	points = std::vector<glm::vec3>();

	primitiveType = GL_POINTS;

	InitVAO();
}

void Points::PushBack(const std::vector<glm::vec3>& pts)
{
	VBO.Bind();
	points.insert(points.end(), pts.begin(), pts.end());
	VBO.PushData(pts);
	VBO.Unbind();
}

inline void Points::InitVAO
{
	mVAO.Bind();

	VBO.AllocBuffer(amount * sizeof(glm::vec3), GL_DYNAMIC_DRAW);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), nullptr);

	mVAO.Unbind();
	VBO.Unbind();
}

inline void Points::GetSize
{
	return points.size();
}
