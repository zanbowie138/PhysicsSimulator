#pragma once
#pragma once
#include<string>
#include <vector>
#include <tuple>
#include <set>
#include <string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

enum ChessPiece
{
	empty,
	pawn,
	rook,
	knight,
	bishop,
	queen,
	king
};

class ChessModel
{
public:

	ChessPiece type;

	std::vector <ModelPt> vertices;
	std::vector <GLuint> indices;

	glm::mat4 modelMatrix;

	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the object
	ChessModel(ChessPiece type, glm::vec3 worldPos);

	std::string getType();

	// Draws the mesh
	void Draw(Shader& shader, Camera& camera);

	// Updates the vertices and indices vectors based on packed binary file
	void updateModel(const char* filename);

	void updatePos(glm::vec3 worldPos);
};