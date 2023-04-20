#pragma once

#include<glad/glad.h>
#include<vector>

class EBO
{
public:
	GLuint ID;
	EBO(std::vector<GLuint>& indices, GLenum type = GL_STATIC_DRAW);

	inline void Bind();
	inline void Unbind();
	inline void Delete();
};

// Constructor that generates a Vertex Buffer Object and links it to vertices
EBO::EBO(std::vector<GLuint>& indices, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), type);
}

// Binds the EBO
inline void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
inline void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
inline void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}