#pragma once

#include <vector>

#include "components/Renderable.h"
#include "Camera.h"
#include "VBO.h"

class Points : public Renderable
{
public:
	std::vector<glm::vec3> points;

	VBO VBO;

	explicit inline Points(GLuint amount);

	inline void PushBack(const std::vector<glm::vec3>& pts);
	inline void Draw(const Shader& shader) const override;
};

Points::Points(const GLuint amount)
{
	points = std::vector<glm::vec3>();

	VAO.Bind();

	VBO.AllocBuffer(amount * sizeof(glm::vec3), GL_DYNAMIC_DRAW);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), nullptr);

	VAO.Unbind();
	VBO.Unbind();
}

void Points::PushBack(const std::vector<glm::vec3>& pts)
{
	VBO.Bind();
	points.insert(points.end(), pts.begin(), pts.end());
	VBO.PushData(pts);
	VBO.Unbind();
}

void Points::Draw(const Shader& shader) const
{
	shader.Activate();
	VAO.Bind();

	glEnable(GL_PROGRAM_POINT_SIZE);

	glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, value_ptr(modelMatrix));

	glDrawArrays(GL_POINTS, 0, points.size());

	VAO.Unbind();
}
