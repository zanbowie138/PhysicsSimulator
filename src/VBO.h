#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

struct ModelPt
{
	glm::vec3 position;
	glm::vec3 normal;
};

class VBO
{
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices, GLenum type = GL_STATIC_DRAW);
	VBO(std::vector<ModelPt>& vertices, GLenum type = GL_STATIC_DRAW);
	VBO(std::vector<glm::vec3>& vertices, GLenum type = GL_STATIC_DRAW);

	inline void Bind();
	inline void Unbind();
	inline void Delete();
};

// Constructors that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<glm::vec3>& vertices, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(std::vector<glm::vec3>), vertices.data(), type);
}
VBO::VBO(std::vector<Vertex>& vertices, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), type);
}
VBO::VBO(std::vector<ModelPt>& vertices, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelPt), vertices.data(), type);
}

// Binds the VBO
inline void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
inline void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
inline void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}