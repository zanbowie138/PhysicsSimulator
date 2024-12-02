#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <utils/Logger.h>

#include "../core/GlobalTypes.h"

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	UBBitset mUniforms;

	inline Shader(const char* vertexFile, const char* fragmentFile);

	void Activate() const { glUseProgram(ID); }
	void Delete() const { glDeleteProgram(ID); }

	inline GLint GetUniformLocation(const char* name) const;
	inline GLuint GetUniformBlockIndex(const char* name) const;

private:
	static inline void CompileErrors(unsigned int shader, const char* name, const char* type);
};

// Reads a text file and outputs a string with everything in the text file
inline std::string get_file_contents(const char* filename)
{
	const std::string localDir = "/shaders/";
	// TODO: Maybe have backup in case BASE_DIR is not defined
	std::string filePath = BASE_DIR + localDir + filename;
	std::ifstream fileText(filePath.c_str(), std::ios::binary);
	if (fileText)
	{
		std::string contents;
		fileText.seekg(0, std::ios::end);
		contents.resize(fileText.tellg());
		fileText.seekg(0, std::ios::beg);
		fileText.read(contents.data(), contents.size());
		fileText.close();
		return (contents);
	}
	LOG(LOG_ERROR) << "Failed to read file: " << filePath.c_str() << "\n";
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	mUniforms.set();
	
	const std::string vertexCode = get_file_contents(vertexFile);
	const std::string fragmentCode = get_file_contents(fragmentFile);

	// Convert string into char arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create and compile vertex shader
	// This controls where the vertices are located
	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GL_FCHECK(glShaderSource(vertexShader, 1, &vertexSource, nullptr));
	GL_FCHECK(glCompileShader(vertexShader));
	CompileErrors(vertexShader, vertexFile, "VERTEX");

	// Create and compile fragment shader
	// This controls how the shape is colored
	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_FCHECK(glShaderSource(fragmentShader, 1, &fragmentSource, nullptr));
	GL_FCHECK(glCompileShader(fragmentShader));
	CompileErrors(fragmentShader, fragmentFile, "FRAGMENT");

	// Create a shader program wrapping the two shaders together
	ID = glCreateProgram();
	GL_FCHECK(glAttachShader(ID, vertexShader));
	GL_FCHECK(glAttachShader(ID, fragmentShader));
	GL_FCHECK(glLinkProgram(ID));
	// TODO: This is pretty horrific
	CompileErrors(ID, (std::string(vertexFile) + std::string(", ").append(fragmentFile)).c_str(), "PROGRAM");

	// Clean up
	GL_FCHECK(glDeleteShader(vertexShader));
	GL_FCHECK(glDeleteShader(fragmentShader));
}

GLint Shader::GetUniformLocation(const char* name) const
{
	const GLint location =  glGetUniformLocation(ID, name);

	assert(location != -1 && "Uniform location not found.");

	return location;
}

inline GLuint Shader::GetUniformBlockIndex(const char* name) const
{
	return GL_FCHECK(glGetUniformBlockIndex(ID, name));
}

void Shader::CompileErrors(const unsigned int shader, const char* name, const char* type)
{
	GLint hasCompiled;
	char infolog[1024];
	if (type != "PROGRAM")
	{
		GL_FCHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled));
		if (hasCompiled == GL_FALSE)
		{
			GL_FCHECK(glGetShaderInfoLog(shader, 1024, nullptr, infolog));
			LOG(LOG_ERROR) << "SHADER_COMPILATION_ERROR for: " << type << " using " << name <<"\n" << infolog << "\n";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			GL_FCHECK(glGetProgramInfoLog(shader, 1024, nullptr, infolog));
			LOG(LOG_ERROR) << "SHADER_LINKING_ERROR for: " << type << " using " << name  << "\n" << infolog << "\n";
		}
	}
}
