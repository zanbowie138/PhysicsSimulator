#pragma once
#include<string>
#include <vector>

#include "../core/GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "../renderer/Texture.h"
#include "Renderable.h"
#include "../physics/Collidable.h"

class Mesh: public Renderable
{
public:
	const std::vector<Vertex>& vertices;
	const std::vector<GLuint>& indices;
	const std::vector<Texture>& textures;

	bool hasTextures = false;

	// Initializes the mesh
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

	void InitECS();
	BoundingBox CalcBoundingBox();
private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures): vertices(vertices), indices(indices), textures(textures)
{
	mSize = indices.size();
	hasTextures = true;
	InitVAO();
}

inline Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices): vertices(vertices), indices(indices), textures(std::vector<Texture>{})
{
	mSize = indices.size();
	InitVAO();
}

inline void Mesh::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	mVAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

inline void Mesh::InitECS()
{
	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ GL_TRIANGLES,mVAO.ID, ShaderID, indices.size(), mColor});
	if (hasTextures)
		ecsController.AddComponent(mEntityID, Components::TextureInfo{ textures[0].ID, textures[1].ID });
}

inline size_t Mesh::GetSize()
{
	return mSize;
}

inline BoundingBox Mesh::CalcBoundingBox()
{
	BoundingBox box;
	transform.CalculateModelMat();
	box.min = transform.modelMat * glm::vec4(vertices[0].position, 1.0f);
	box.max = transform.modelMat * glm::vec4(vertices[0].position, 1.0f);
	for (const auto& pt : vertices)
	{
		auto point = transform.modelMat * glm::vec4(pt.position, 1.0f);
		for (unsigned int i = 0; i < 3; i++)
		{
			box.max[i] = std::max(box.max[i], point[i]);
			box.min[i] = std::min(box.min[i], point[i]);
		}
	}
	box.UpdateSurfaceArea();
	return box;
}