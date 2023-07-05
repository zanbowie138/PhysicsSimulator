#pragma once
#include "Renderable.h"
#include "../renderer/VBO.h"
#include "../renderer/EBO.h"
#include "../physics/BoundingBox.h"

class Points : public Renderable
{
public:
	const GLuint mCapacity;

	GLuint mVertexAmount = 0;

	VBO VBO;

	Points(GLuint capacity);

	void PushBack(const std::vector<glm::vec3>& vertices, const glm::mat4& modelMat);
	void PushBack(const std::vector<MeshPt>& vertices, const glm::mat4& modelMat);
	void PushBack(const std::vector<ModelPt>& vertices, const glm::mat4& modelMat);

	void PushToBuffer(const std::vector<glm::vec3>& points);

	void UpdateSize();

	void Clear();

private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Points::Points(const GLuint capacity): mCapacity(capacity)
{
	primitiveType = GL_POINTS;

	InitVAO();
}

inline void Points::PushBack(const std::vector<glm::vec3>& vertices, const glm::mat4& modelMat)
{
	std::vector<glm::vec3> temp(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		temp[i] = modelMat * glm::vec4(vertices[i], 1.0);
	}

	PushToBuffer(temp);
}

inline void Points::PushBack(const std::vector<MeshPt>& vertices, const glm::mat4& modelMat)
{
	std::vector<glm::vec3> temp(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		temp[i] = modelMat * glm::vec4(vertices[i].position, 1.0);
	}

	PushToBuffer(temp);
}

inline void Points::PushBack(const std::vector<ModelPt>& vertices, const glm::mat4& modelMat)
{
	std::vector<glm::vec3> temp(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		temp[i] = modelMat * glm::vec4(vertices[i].position, 1.0);
	}

	PushToBuffer(temp);
}

inline void Points::PushToBuffer(const std::vector<glm::vec3>& points)
{
	mVertexAmount += points.size();

	VBO.Bind();
	VBO.PushData(points);
	VBO.Unbind();

	UpdateSize();
}

inline void Points::Clear()
{
	VBO.ClearData();

	mVertexAmount = 0;

	UpdateSize();
}

inline void Points::UpdateSize()
{
	ecsController.GetComponent<Components::RenderInfo>(mEntityID).size = GetSize();
}

inline void Points::InitVAO()
{
	mVAO.Bind();

	VBO.AllocBuffer(mCapacity * sizeof(glm::vec3), GL_DYNAMIC_DRAW);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), nullptr);

	mVAO.Unbind();
	VBO.Unbind();
}

inline size_t Points::GetSize()
{
	return mVertexAmount;
}
