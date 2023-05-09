#pragma once
#include<string>
#include <vector>

#include "../core/GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "../renderer/Texture.h"
#include "../physics/Collidable.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Initializes the mesh
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);

private:
	VAO VAO;
	void InitVAO();
};

inline Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
                  std::vector<Texture> textures): vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
	InitVAO();
}

inline Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices): vertices(std::move(vertices)), indices(
	                                                                              std::move(indices))
{
	InitVAO();
}

inline void Mesh::InitVAO()
{
	VAO.Bind();

	const VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), nullptr);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}
