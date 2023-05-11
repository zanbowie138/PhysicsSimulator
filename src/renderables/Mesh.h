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
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Initializes the mesh
	Mesh(std::vector<Vertex> vertices, const std::vector<GLuint>& indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);

	void InitECS();

private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Mesh::Mesh(std::vector<Vertex> vertices, const std::vector<GLuint>& indices, std::vector<Texture> textures): indices(std::move(indices)), vertices(
	std::move(
		vertices)), textures(std::move(textures))
{
	InitVAO();
}

inline Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices): indices(
	                                                                              std::move(indices)), vertices(std::move(vertices))
{
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
	assert(!indices.empty() && "No indices");

	// Initialize entity
	mEntityID = ecsController.CreateEntity();

	// Add components
	ecsController.AddComponent(mEntityID, transform);
	ecsController.AddComponent(mEntityID, Components::RenderInfo{ GL_TRIANGLES,mVAO.ID, ShaderID, indices.size() });
	if (!textures.empty())
		ecsController.AddComponent(mEntityID, Components::TextureInfo{ textures[0].ID, textures[1].ID });
}

inline size_t Mesh::GetSize()
{
	return indices.size();
}