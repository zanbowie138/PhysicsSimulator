#pragma once

#include <vector>
#include <algorithm>

#include "Renderable.h"
#include "../renderer/VBO.h"
#include "../renderer/EBO.h"
#include "../physics/BoundingBox.h"

extern ECSController ecsController;

class Lines : public Renderable
{
public:
	const GLuint mCapacity;
	GLuint mIndexAmount = 0;
	GLuint mVertexAmount = 0;

	VBO VBO;
	EBO EBO;

	explicit inline Lines(GLuint indiceAmt);

	void PushBack(const BoundingBox& box);
	void PushBack(const std::vector<BoundingBox>& boxes);

	void PushBack(const ::MeshData& data, const glm::mat4& modelMat);
	void PushBack(const ::ModelData& data, const glm::mat4& modelMat);

	void PushToBuffer(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);

	void Clear();
	size_t GetSize() override;
private:
	void InitVAO() override;
	void UpdateSize();

	const std::vector<GLuint> cubeIdxOffset =
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

	const std::vector<GLuint> triIdxOffset =
	{
		0,1,
		1,2,
		2,0
	};

	static std::vector<glm::vec3> GetCubeVertices(const BoundingBox& box)
	{
		return std::vector
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
	}

};

Lines::Lines(const GLuint indiceAmt): mCapacity(indiceAmt)
{
	primitiveType = GL_LINES;

	InitVAO();
}

inline void Lines::PushToBuffer(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices)
{
	mVAO.Bind();

	VBO.Bind();
	VBO.PushData(vertices);

	EBO.Bind();
	EBO.PushData(indices);

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

	mIndexAmount += static_cast<GLuint>(indices.size());
	mVertexAmount += static_cast<GLuint>(vertices.size());

	UpdateSize();
}

inline void Lines::PushBack(const BoundingBox& box)
{
	std::vector<glm::vec3> tempVertices;
	std::vector<GLuint> tempIndices;

	tempVertices.resize(8);
	tempIndices.resize(24);

	auto cubeVerts = GetCubeVertices(box);
	std::copy(cubeVerts.begin(), cubeVerts.end(), tempVertices.begin());

	for (size_t i = 0; i < 24; ++i)
	{
		tempIndices[i] = cubeIdxOffset[i] + mVertexAmount;
	}

	PushToBuffer(tempVertices, tempIndices);
}


inline void Lines::PushBack(const std::vector<BoundingBox>& boxes)
{
	size_t vIdx, iIdx;
	vIdx = iIdx = 0;
	
	std::vector<glm::vec3> tempVertices(boxes.size() * 8);
	std::vector<GLuint> tempIndices(boxes.size() * 24);

	for (const auto& box : boxes)
	{
		for (size_t i = 0; i < 24; ++i)
		{
			tempIndices[iIdx] = static_cast<GLuint>(vIdx) + cubeIdxOffset[i] + mVertexAmount;
			++iIdx;
		}

		auto cubeVerts = GetCubeVertices(box);
		std::copy(cubeVerts.begin(), cubeVerts.end(), tempVertices.begin() + vIdx);
		vIdx += 8;
	}

	PushToBuffer(tempVertices, tempIndices);
}

inline void Lines::PushBack(const MeshData& data, const glm::mat4& modelMat)
{
	std::vector<glm::vec3> positions(data.vertices.size());
	std::vector<GLuint> indices(data.indices.size() * 2);

	for (size_t i = 0; i < data.vertices.size(); ++i)
	{
		positions[i] = modelMat * glm::vec4(data.vertices[i].position, 1.0);
	}

	for (size_t i = 0; i < data.indices.size(); i+=3)
	{
		for (size_t j = 0; j < 6; j++)
		{
			indices[i * 2 + j] = data.indices[i + triIdxOffset[j]];
		}
	}

	PushToBuffer(positions, indices);
}

inline void Lines::PushBack(const ModelData& data, const glm::mat4& modelMat)
{
	std::vector<glm::vec3> positions(data.vertices.size());
	std::vector<GLuint> indices;

	for (size_t i = 0; i < data.vertices.size(); ++i)
	{
		positions[i] = modelMat * glm::vec4(data.vertices[i].position, 1.0);
	}

	for (size_t i = 0; i < data.indices.size(); ++i)
	{
		indices.emplace_back(data.indices[i] + mVertexAmount);
	}

	PushToBuffer(positions, indices);
}

inline void Lines::Clear()
{
	VBO.ClearData();
	EBO.ClearData();

	mIndexAmount = 0;
	mVertexAmount = 0;

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
	return mIndexAmount;
}