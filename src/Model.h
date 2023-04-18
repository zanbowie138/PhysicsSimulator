#pragma once
#include<string>
#include <vector>
#include <tuple>
#include <set>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

struct compareVec3
{
	// Adapted from https://stackoverflow.com/questions/46636721/how-do-i-use-glm-vector-relational-functions
	bool operator() (const std::tuple<glm::vec3, glm::vec3, unsigned int>& lhs, const std::tuple<glm::vec3, glm::vec3, unsigned int>& rhs) const
	{
		glm::vec3 nequ = glm::notEqual(std::get<0>(lhs), std::get<0>(rhs));
		return glm::lessThan(std::get<0>(lhs), std::get<0>(rhs))[nequ[0] ? 0 : (nequ[1] ? 1 : 2)];
	}
};

struct STLmesh
{
	std::vector<ModelPt> vertices;
	std::vector<unsigned int> indices;
};

class Model
{
public:
	std::vector <ModelPt> vertices;
	std::vector <GLuint> indices;

	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the object
	Model(const char* filename, bool is_stl);

	// Draws the mesh
	void Draw(Shader& shader, Camera& camera);

	// Reads an stl file
	STLmesh readSTL(const char* filepath);
};