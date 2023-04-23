#pragma once

#include<filesystem>

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	inline Shader(const char* vertexFile, const char* fragmentFile);

	inline void Activate() const;
	inline void Delete() const;

	inline GLuint GetUniformLocation(const char* name) const;
	inline GLuint GetUniformBlockIndex(const char* name) const;

private:
	static inline void CompileErrors(unsigned int shader, const char* type);
};

// Reads a text file and outputs a string with everything in the text file
inline std::string get_file_contents(const char* filename)
{
	const std::string localDir = "/shaders/";
	std::ifstream in((std::filesystem::current_path().string() + localDir + filename).c_str(), std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(contents.data(), contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	const std::string vertexCode = get_file_contents(vertexFile);
	const std::string fragmentCode = get_file_contents(fragmentFile);

	// Convert string into char arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create and compile vertex shader
	// This controls where the vertices are located
	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	CompileErrors(vertexShader, "VERTEX");

	// Create and compile fragment shader
	// This controls how the shape is colored
	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	CompileErrors(fragmentShader, "FRAGMENT");

	// Create a shader program wrapping the two shaders together
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	CompileErrors(ID, "PROGRAM");

	// Clean up
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate() const
{
	glUseProgram(ID);
}

void Shader::Delete() const
{
	glDeleteProgram(ID);
}

GLuint Shader::GetUniformLocation(const char* name) const
{
	return glGetUniformLocation(ID, name);
}

inline GLuint Shader::GetUniformBlockIndex(const char* name) const
{
	return glGetUniformBlockIndex(ID, name);
}

void Shader::CompileErrors(const unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infolog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
		}
	}
}
