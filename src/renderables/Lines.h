#pragma once

#include <vector>

#include "Renderable.h"
#include "../renderer/VBO.h"
#include "../renderer/EBO.h"
#include "../physics/BoundingBox.h"

extern ECSController ecsController;

class Lines : public Renderable
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	const GLuint mCapacity;

	VBO VBO;
	EBO EBO;

	explicit inline Lines(GLuint indiceAmt);

	inline void PushBack(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds);
	inline void PushBack(const BoundingBox& box);
	inline void Erase();
	size_t GetSize() override;
private:
	void InitVAO() override;
	void UpdateSize();
};

Lines::Lines(const GLuint indiceAmt): mCapacity(indiceAmt)
{
	vertices = std::vector<glm::vec3>();
	indices = std::vector<unsigned int>();
	primitiveType = GL_LINES;

	InitVAO();
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

	UpdateSize();
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
	const std::vector<GLuint> offset =
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

	std::vector<GLuint> inds;
	for (const auto& i : offset)
	{
		inds.push_back(static_cast<GLuint>(vertices.size()) - 8 + i);
	}

	indices.insert(indices.end(), inds.begin(), inds.end());
	EBO.PushData(inds);

	VBO.Unbind();
	EBO.Unbind();

	UpdateSize();
}

inline void Lines::Erase()
{
	vertices.erase(vertices.begin(), vertices.end());
	indices.erase(indices.begin(), indices.end());

	VBO.ClearData();
	EBO.ClearData();

	UpdateSize();
}

inline void Lines::UpdateSize()
{
	ecsController.GetComponent<Components::RenderInfo>(mEntityID).size = GetSize();
}

inline void Lines::InitVAO()
{
	mVAO.Bind();

	VBO.AllocBuffer(mCapacity * 2 * sizeof(glm::vec3), GL_DYNAMIC_DRAW);
	EBO.AllocBuffer(mCapacity * sizeof(GLuint), GL_DYNAMIC_DRAW);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), nullptr);

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

inline size_t Lines::GetSize()
{
	return indices.size();
}