#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <iostream>

class VBO
{
public:
	mutable GLuint ID{};
	GLuint bufSize = 0;
	GLuint currentBufSize = 0;

	inline VBO();

	template <typename T>
	explicit inline VBO(const std::vector<T>& vertices);

	inline void PushData(const std::vector<glm::vec3>& vertices);
	inline void AllocBuffer(GLint size, GLenum type);

	inline void Bind() const;
	static inline void Unbind();
	inline void Delete() const;
};

// Constructors that generates a Vertex Buffer Object and links it to vertices
inline VBO::VBO()
{
	glGenBuffers(1, &ID);
}

template <typename T>
inline VBO::VBO(const std::vector<T>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);
}

inline void VBO::PushData(const std::vector<glm::vec3>& vertices)
{
	assert(currentBufSize + vertices.size() * sizeof(glm::vec3) < bufSize && "VBO Overflow");
	glBufferSubData(GL_ARRAY_BUFFER, currentBufSize, vertices.size() * sizeof(glm::vec3), vertices.data());
	currentBufSize += vertices.size() * sizeof(glm::vec3);
}

inline void VBO::AllocBuffer(const GLint size, const GLenum type)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, type);
	bufSize = size;
}

// Binds the VBO
inline void VBO::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
inline void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
inline void VBO::Delete() const
{
	glDeleteBuffers(1, &ID);
}
