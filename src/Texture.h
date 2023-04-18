#pragma once

#include<filesystem>

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	const char* texType;
	GLenum texFormat;
	GLuint unit; 

	Texture(const char* image, const char* texType, GLenum texFormat,GLuint slot, GLenum colorChannels, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};