#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <optional>
#include <utils/Logger.h>
#include <utils/PathUtils.h>

#include "../core/GlobalTypes.h"

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;

	static inline std::optional<Shader> Create(const char* vertexFile, const char* fragmentFile);

	void Activate() const { glUseProgram(ID); }
	void Delete() const { glDeleteProgram(ID); }

	inline GLint GetUniformLocation(const char* name) const;
	inline GLuint GetUniformBlockIndex(const char* name) const;
	inline bool UsesUniform(std::size_t ub_idx) const;
	inline void DisableUniform(std::size_t ub_idx);

private:
	UBBitset mUniforms;

	inline Shader(const char* vertexFile, const char* fragmentFile);
	static inline bool CompileErrors(unsigned int shader, const char* name, const char* type);
};

// Reads a text file and outputs a string with everything in the text file
inline std::string get_file_contents(const char* filename)
{
	std::string filePath = Utils::GetResourcePath("/shaders/", filename);
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
	return "";
}

std::optional<Shader> Shader::Create(const char* vertexFile, const char* fragmentFile)
{
	const std::string vertexCode = get_file_contents(vertexFile);
	const std::string fragmentCode = get_file_contents(fragmentFile);

	if (vertexCode.empty()) {
		LOG(LOG_ERROR) << "Failed to read vertex shader: " << vertexFile << "\n";
		return std::nullopt;
	}
	if (fragmentCode.empty()) {
		LOG(LOG_ERROR) << "Failed to read fragment shader: " << fragmentFile << "\n";
		return std::nullopt;
	}

	// Convert string into char arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create and compile vertex shader
	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GL_FCHECK(glShaderSource(vertexShader, 1, &vertexSource, nullptr));
	GL_FCHECK(glCompileShader(vertexShader));
	if (!CompileErrors(vertexShader, vertexFile, "VERTEX")) {
		glDeleteShader(vertexShader);
		return std::nullopt;
	}

	// Create and compile fragment shader
	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_FCHECK(glShaderSource(fragmentShader, 1, &fragmentSource, nullptr));
	GL_FCHECK(glCompileShader(fragmentShader));
	if (!CompileErrors(fragmentShader, fragmentFile, "FRAGMENT")) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return std::nullopt;
	}

	// Create a shader program wrapping the two shaders together
	GLuint programID = glCreateProgram();
	GL_FCHECK(glAttachShader(programID, vertexShader));
	GL_FCHECK(glAttachShader(programID, fragmentShader));
	GL_FCHECK(glLinkProgram(programID));

	// Clean up shaders
	GL_FCHECK(glDeleteShader(vertexShader));
	GL_FCHECK(glDeleteShader(fragmentShader));

	if (!CompileErrors(programID, (std::string(vertexFile) + std::string(", ").append(fragmentFile)).c_str(), "PROGRAM")) {
		glDeleteProgram(programID);
		return std::nullopt;
	}

	Shader shader(vertexFile, fragmentFile);
	shader.ID = programID;
	return shader;
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	mUniforms.set();
}

GLint Shader::GetUniformLocation(const char* name) const
{
	const GLint location =  glGetUniformLocation(ID, name);

	if (location == -1) {
		LOG(LOG_WARNING) << "Uniform location not found: " << name << "\n";
	}

	return location;
}

inline GLuint Shader::GetUniformBlockIndex(const char* name) const
{
	return GL_FCHECK(glGetUniformBlockIndex(ID, name));
}

inline bool Shader::UsesUniform(std::size_t ub_idx) const {
	return mUniforms.test(ub_idx);
}

inline void Shader::DisableUniform(std::size_t ub_idx) {
	mUniforms.reset(ub_idx);
}

bool Shader::CompileErrors(const unsigned int shader, const char* name, const char* type)
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
			return false;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			GL_FCHECK(glGetProgramInfoLog(shader, 1024, nullptr, infolog));
			LOG(LOG_ERROR) << "SHADER_LINKING_ERROR for: " << type << " using " << name  << "\n" << infolog << "\n";
			return false;
		}
	}
	return true;
}
