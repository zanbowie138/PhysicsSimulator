#pragma once

#include <vector>
#include <algorithm>

#include "Renderable.h"
#include "../renderer/VBO.h"
#include "../renderer/EBO.h"
#include "../physics/BoundingBox.h"
#include "../utils/MeshProcessing.h"

class Lines: public Renderable
{
public:
	const GLuint mCapacity;
	GLuint mIndexAmount = 0;
	GLuint mVertexAmount = 0;

	VBO VBO;
	EBO EBO;

	explicit inline Lines(GLuint indiceAmt);

	void PushBoundingBox(const BoundingBox& box);
	void PushBoundingBoxes(const std::vector<BoundingBox>& boxes);

	void PushMeshOutline(const ::MeshData& data, const glm::mat4& modelMat);
	void PushModelOutline(const ::ModelData& data, const glm::mat4& modelMat);

	void PushData(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);
	void PushShapeData(const ShapeData& shapeData);

	void Clear();
	size_t GetSize() override { return mIndexAmount; }
private:
	void InitVAO() override;
	void UpdateSize();
};

Lines::Lines(const GLuint indiceAmt): mCapacity(indiceAmt)
{
	primitiveType = GL_LINES;

	InitVAO();
}

inline void Lines::PushData(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices)
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

inline void Lines::PushShapeData(const ShapeData& shapeData)
{
	PushData(shapeData.vertices, shapeData.indices);
}

inline void Lines::PushBoundingBox(const BoundingBox& box)
{
	PushShapeData(Utils::CalculateEdges(box, mVertexAmount));
}


inline void Lines::PushBoundingBoxes(const std::vector<BoundingBox>& boxes)
{
	PushShapeData(Utils::CalculateEdges(boxes, mVertexAmount));
}

inline void Lines::PushMeshOutline(const MeshData& data, const glm::mat4& modelMat)
{
	PushShapeData(Utils::CalculateEdges(data, modelMat));
}

inline void Lines::PushModelOutline(const ModelData& data, const glm::mat4& modelMat)
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

	PushData(positions, indices);
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