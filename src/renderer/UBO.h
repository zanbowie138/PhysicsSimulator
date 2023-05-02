#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <iostream>

#include "shaderClass.h"

class UBO
{
public:
	mutable GLuint ID{};

	inline UBO();

	inline void AllocBuffer(const GLint size, const GLenum type = GL_DYNAMIC_DRAW) const;

	// Binds area of uniform with an index
	inline void BindUniformRange(const GLuint index, const GLuint offset, const GLuint size) const;

	// Binds a shader's uniform block to index of uniform buffer range
	inline void BindShader(const Shader& shader, const char* uniformName, const GLuint index) const;

	inline void EditBuffer(const char* buffer, const GLuint size, const GLenum type = GL_DYNAMIC_DRAW) const;
	inline void EditBufferRange(const char* buffer, const GLuint offset, const GLuint size) const;

	inline void Bind() const;
	static inline void Unbind();
	inline void Delete() const;
};

inline UBO::UBO()
{
	glGenBuffers(1, &ID);
}

inline void UBO::AllocBuffer(const GLint size, const GLenum type) const
{
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, type);
}

inline void UBO::BindUniformRange(const GLuint index, const GLuint offset, const GLuint size) const
{
	glBindBufferRange(GL_UNIFORM_BUFFER, index, ID, offset, size);	
}

inline void UBO::BindShader(const Shader& shader, const char* uniformName, const GLuint index) const
{
	glUniformBlockBinding(shader.ID, shader.GetUniformBlockIndex(uniformName) , index);
}

inline void UBO::EditBuffer(const char* buffer, const GLuint size, const GLenum type) const
{
	glBufferData(GL_UNIFORM_BUFFER, size, buffer, type);
}

inline void UBO::EditBufferRange(const char* buffer, const GLuint offset, const GLuint size) const
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, buffer);
}

// Binds the UBO
inline void UBO::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

// Unbinds the UBO
inline void UBO::Unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Deletes the UBO
inline void UBO::Delete() const
{
	glDeleteBuffers(1, &ID);
}
