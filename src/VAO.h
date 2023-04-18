#pragma once

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	GLuint ID;
	VAO();

	/// <summary>
	/// </summary>
	/// <param name="VBO"></param>
	/// <param name="layout">Specifies the index of the generic vertex attribute to be modified.</param>
	/// <param name="numComponents">Number of components per vertex</param>
	/// <param name="type">data type of each component in array</param>
	/// <param name="stride">byte offset of each consecutive vertex</param>
	/// <param name="offset">Specifies a offset of the first component of the first generic vertex attribute</param>
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};