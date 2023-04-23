#pragma once

#include <vector>

#include "components/Renderable.h"
#include "Camera.h"
#include "VBO.h"
#include "EBO.h"

class Lines : public Renderable
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	VBO VBO;
	EBO EBO;

	explicit inline Lines(GLuint indiceAmt);

	inline void PushBack(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds);

	inline void Draw(const Shader& shader) const override;
};

Lines::Lines(const GLuint indiceAmt)
{
	vertices = std::vector<glm::vec3>();
	indices = std::vector<unsigned int>();

	VAO.Bind();

	VBO.AllocBuffer(indiceAmt * 2 * sizeof(glm::vec3), GL_DYNAMIC_DRAW);
	EBO.AllocBuffer(indiceAmt * sizeof(GLuint), GL_DYNAMIC_DRAW);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), nullptr);

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Lines::PushBack(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds)
{
	VBO.Bind();
	vertices.insert(vertices.end(), verts.begin(), verts.end());
	VBO.PushData(verts);

	EBO.Bind();
	indices.insert(indices.end(), inds.begin(), inds.end());
	EBO.PushData(inds);

	VBO.Unbind();
	EBO.Unbind();
}

void Lines::Draw(const Shader& shader) const
{
	shader.Activate();
	VAO.Bind();

	glEnable(GL_PROGRAM_POINT_SIZE);

	glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, value_ptr(modelMatrix));

	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

	VAO.Unbind();
}
