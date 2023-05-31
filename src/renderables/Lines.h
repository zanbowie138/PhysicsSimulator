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
	inline void ResizeArrays(const size_t boxAmt);
	inline void Clear();
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
	mVAO.Bind();

	VBO.Bind();
	vertices.insert(vertices.end(), verts.begin(), verts.end());
	VBO.PushData(verts);

	EBO.Bind();
	indices.insert(indices.end(), inds.begin(), inds.end());
	EBO.PushData(inds);

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

	UpdateSize();
}

inline void Lines::PushBack(const BoundingBox& box)
{
	mVAO.Bind();
	VBO.Bind();
	std::vector<glm::vec3> verts =
	{
		glm::vec3(box.max),
		glm::vec3(box.max.x, box.max.y, box.min.z),
		glm::vec3(box.min.x, box.max.y, box.min.z),
		glm::vec3(box.min.x, box.max.y, box.max.z),
		glm::vec3(box.max.x, box.min.y, box.max.z),
		glm::vec3(box.max.x, box.min.y, box.min.z),
		glm::vec3(box.min),
		glm::vec3(box.min.x, box.min.y, box.max.z)
	};

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
	inds.resize(12);
	for (const auto& i : offset)
	{
		inds.push_back(static_cast<GLuint>(vertices.size()) - 8 + i);
	}

	indices.insert(indices.end(), inds.begin(), inds.end());
	EBO.PushData(inds);

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

	UpdateSize();
}

inline void Lines::ResizeArrays(const size_t boxAmt)
{
	vertices.reserve(boxAmt * 8);
	indices.reserve(boxAmt * 12);
}

inline void Lines::Clear()
{
	vertices.clear();
	indices.clear();

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