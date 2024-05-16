#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <iostream>

class VBO
{
public:
	mutable GLuint ID{};
	GLuint bufSize = 0;
	GLuint currentBufSize = 0;

	// Constructors that generates a Vertex Buffer Object and links it to vertices
	VBO() { glGenBuffers(1, &ID); }

	template <typename T>
	explicit VBO(const std::vector<T>& vertices);

	inline void PushData(const std::vector<glm::vec3>& vertices);
	inline void AllocBuffer(GLint size, GLenum type);
	void ClearData() { currentBufSize = 0; }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }
	static void Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Delete() const { glDeleteBuffers(1, &ID); }
};

template <typename T>
VBO::VBO(const std::vector<T>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);
}

inline void VBO::PushData(const std::vector<glm::vec3>& vertices)
{
	assert(currentBufSize + vertices.size() * sizeof(glm::vec3) < bufSize && "VBO Overflow");

	glBufferSubData(GL_ARRAY_BUFFER, currentBufSize, vertices.size() * sizeof(glm::vec3), vertices.data());
	currentBufSize += static_cast<GLuint>(vertices.size() * sizeof(glm::vec3));
}

inline void VBO::AllocBuffer(const GLint size, const GLenum type)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, type);
	bufSize = size;
}
