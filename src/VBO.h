#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

struct ModelPt
{
	glm::vec3 position;
	glm::vec3 normal;
};

class VBO
{
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices);
	VBO(std::vector<ModelPt>& vertices);
	VBO(std::vector<glm::vec3>& vertices);

	void Bind();
	void Unbind();
	void Delete();
};