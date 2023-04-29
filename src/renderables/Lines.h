#pragma once

#include <vector>

#include "components/Renderable.h"
#include "Camera.h"
#include "VBO.h"
#include "EBO.h"
#include "BoundingBox.h"

class Lines : public Renderable
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	VBO VBO;
	EBO EBO;

	explicit inline Lines(GLuint indiceAmt);

	inline void PushBack(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds);
	inline void PushBack(const BoundingBox& box);

	inline void Draw(const Shader& shader) const override;
};

Lines::Lines(const GLuint indiceAmt)
{
	vertices = std::vector<glm::vec3>();
	indices = std::vector<unsigned int>();

	color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

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

inline void Lines::PushBack(const BoundingBox& box)
{
	VBO.Bind();
	std::vector<glm::vec3> verts;
	verts.emplace_back(box.max);
	verts.emplace_back(box.max.x, box.max.y, box.min.z);
	verts.emplace_back(box.min.x, box.max.y, box.min.z);
	verts.emplace_back(box.min.x, box.max.y, box.max.z);
	
	verts.emplace_back(box.max.x, box.min.y, box.max.z);
	verts.emplace_back(box.max.x, box.min.y, box.min.z);
	verts.emplace_back(box.min);
	verts.emplace_back(box.min.x, box.min.y, box.max.z);

	vertices.insert(vertices.end(), verts.begin(), verts.end());
	VBO.PushData(verts);

	EBO.Bind();
	std::vector<unsigned int> inds =
	{
		0,1,
		1,2,
		2,3,
		3,0,
		4,5,
		5,6,
		6,7,
		7,4,
		0,4,
		1,5,
		2,6,
		3,7
	};

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
