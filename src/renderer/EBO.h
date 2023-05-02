#pragma once

#include<glad/glad.h>
#include<vector>
#include <iostream>

class EBO
{
public:
	mutable GLuint ID{};
	GLuint bufSize = 0;
	GLuint currentBufSize = 0;

	inline EBO();
	explicit inline EBO(const std::vector<GLuint>& indices);

	inline void AllocBuffer(GLint size, GLenum type);
	inline void PushData(const std::vector<GLuint>& indices);

	inline void Bind() const;
	static inline void Unbind();
	inline void Delete() const;
};

// Constructor that generates a Vertex Buffer Object and links it to vertices
inline EBO::EBO()
{
	glGenBuffers(1, &ID);
}

inline EBO::EBO(const std::vector<GLuint>& indices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

inline void EBO::PushData(const std::vector<GLuint>& indices)
{
	if (indices.size() * sizeof(GLuint) > bufSize)
	{
		std::cout << "EBO Buffer Overflow..." << std::endl;
		return;
	}
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentBufSize, indices.size() * sizeof(GLuint), indices.data());
	currentBufSize += indices.size() * sizeof(GLuint);
}

inline void EBO::AllocBuffer(const GLint size, const GLenum type)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, type);
	bufSize = size;
}

// Binds the EBO
inline void EBO::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
inline void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
inline void EBO::Delete() const
{
	glDeleteBuffers(1, &ID);
}
