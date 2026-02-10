#pragma once
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
	const Texture& diffuseTex = {};
	const Texture& specularTex = {};

	bool hasDiffuse = false;
	bool hasSpecular = false;

	// Initializes the mesh
	Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const Texture& diffuseTex,
	      const Texture& specularTex);
	Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const Texture& diffuseTex);
	Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices);


	Model(const ModelData& data);
	Model(const ModelData& data, const Texture& diffuseTex, const Texture& specularTex);

	void AddToECS();
	BoundingBox CalcBoundingBox();
	BoundingBox CalcBoundingBox(const glm::mat4& modelMat) const;
private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Model::Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const Texture& diffuseTex, const Texture& specularTex): vertices(vertices), indices(indices), diffuseTex(diffuseTex), specularTex(specularTex)
{
    mSize = indices.size();
	hasDiffuse = true;
	hasSpecular = true;
    InitVAO();
}

inline Model::Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices, const Texture& diffuseTex):
	vertices(vertices), indices(indices), diffuseTex(diffuseTex)
{
	mSize = indices.size();
	hasDiffuse = true;
	InitVAO();
}

inline Model::Model(const std::vector<ModelPt>& vertices, const std::vector<GLuint>& indices): vertices(vertices),
indices(indices)
{
	mSize = indices.size();
	InitVAO();
}

inline Model::Model(const ModelData& data): vertices(data.vertices), indices(data.indices)
{
	mSize = indices.size();
	InitVAO();
}

inline Model::Model(const ModelData& data, const Texture& diffuseTex, const Texture& specularTex): vertices(data.vertices), indices(data.indices), diffuseTex(diffuseTex), specularTex(specularTex)
{
    mSize = indices.size();
    hasDiffuse = true;
    hasSpecular = true;
    InitVAO();
}

inline void Model::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), reinterpret_cast<void*>(3 * sizeof(float)));
	mVAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(ModelPt), reinterpret_cast<void*>(6 * sizeof(float)));

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

inline void Model::AddToECS()
{
	// Initialize entity
	mEntityID = world.CreateEntity();

	// Add components
	world.AddComponent(mEntityID, transform);
	world.AddComponent(mEntityID, Components::RenderInfo{ GL_TRIANGLES,mVAO.ID, ShaderID, indices.size(), mColor});
	if (hasDiffuse)
		world.AddComponent(mEntityID, Components::DiffuseTextureInfo( { diffuseTex.ID }));
	if (hasSpecular)
        world.AddComponent(mEntityID, Components::SpecularTextureInfo( { specularTex.ID }));
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
