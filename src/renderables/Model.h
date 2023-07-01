#pragma once
#include <string>
#include <vector>

#include "../core/GlobalTypes.h"

#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "../renderer/Texture.h"

#include "Renderable.h"

class Model: public Renderable
{
public:
	const std::vector<ModelPt>& vertices;
	const std::vector<GLuint>& indices;
	const std::vector<Texture>& textures;

	bool hasTextures = false;

	// Initializes the mesh
	Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices);


	Model(const ModelData& data);
	Model(const ModelData& data, const std::vector<Texture>& textures);

	void InitECS();
	BoundingBox CalcBoundingBox();
	BoundingBox CalcBoundingBox(const glm::mat4& modelMat) const;
private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Model::Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures): vertices(vertices), indices(indices), textures(textures)
{
	mSize = indices.size();
	hasTextures = true;
	InitVAO();
}

inline Model::Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices): vertices(vertices), indices(indices), textures(std::vector<Texture>{})
{
	mSize = indices.size();
	InitVAO();
}

inline Model::Model(const ModelData& data): vertices(data.vertices), indices(data.indices), textures(std::vector<Texture>{})
{
	mSize = indices.size();
	InitVAO();
}

inline Model::Model(const ModelData& data, const std::vector<Texture>& textures): vertices(data.vertices), indices(data.indices), textures(textures)
{
	mSize = indices.size();
	hasTextures = true;
	InitVAO();
}

inline void Model::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));
	mVAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(ModelPt), (void*)(6 * sizeof(float)));

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

inline void Model::InitECS()
{
	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ GL_TRIANGLES,mVAO.ID, ShaderID, indices.size(), mColor});
	if (hasTextures)
		ecsController.AddComponent(mEntityID, Components::TextureInfo{ textures[0].ID, textures[1].ID });
}

inline size_t Model::GetSize()
{
	return mSize;
}

inline BoundingBox Model::CalcBoundingBox()
{
	transform.CalculateModelMat();
	return CalcBoundingBox(transform.modelMat);
}

inline BoundingBox Model::CalcBoundingBox(const glm::mat4& modelMat) const
{
	BoundingBox box;
	box.min = modelMat * glm::vec4(vertices[0].position, 1.0f);
	box.max = modelMat * glm::vec4(vertices[0].position, 1.0f);
	for (const auto& pt : vertices)
	{
		auto point = modelMat * glm::vec4(pt.position, 1.0f);
		for (unsigned int i = 0; i < 3; i++)
		{
			box.max[i] = std::max(box.max[i], point[i]);
			box.min[i] = std::min(box.min[i], point[i]);
		}
	}
	box.UpdateSurfaceArea();
	return box;
}
