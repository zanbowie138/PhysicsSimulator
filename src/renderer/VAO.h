#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VBO.h"

class VAO
{
public:
	GLuint ID{};
	inline VAO();

	/// <summary>
	/// </summary>
	/// <param name="VBO"></param>
	/// <param name="layout">Specifies the index of the generic vertex attribute to be modified.</param>
	/// <param name="numComponents">Number of components per vertex</param>
	/// <param name="type">data type of each component in array</param>
	/// <param name="stride">byte offset of each consecutive vertex</param>
	/// <param name="offset">Specifies a offset of the first component of the first generic vertex attribute</param>
	inline void LinkAttrib(const VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void* offset);
	inline void Bind() const;
	static inline void Unbind();
	inline void Delete() const;
};

// Constructor that generates a VAO ID
inline VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Links a VBO to the VAO using a certain layout
inline void VAO::LinkAttrib(const VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void* offset)
{
	/*
	* layout: Specifies the index of the generic vertex attribute to be modified.
	* numComponents: Number of components per vertex
	* type: data type of each component in array
	* stride: byte offset of each consecutive vertex
	* offset: Specifies a offset of the first component of the first generic vertex attribute
	*/
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

// Binds the VAO
inline void VAO::Bind() const
{
	glBindVertexArray(ID);
}

// Unbinds the VAO
inline void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Deletes the VAO
inline void VAO::Delete() const
{
	glDeleteVertexArrays(1, &ID);
}
