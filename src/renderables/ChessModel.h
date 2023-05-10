#pragma once

#include <string>
#include <string>
#include <vector>

#include "../core/GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "../physics/Collidable.h"
#include "Renderable.h"

class ChessModel: public Renderable
{
public:
	ChessPiece type;

	std::vector<ModelPt> vertices;

	// Initializes the object
	ChessModel(ChessPiece type, glm::vec3 worldPos);

	std::string GetType() const;

	// Updates the vertices and indices vectors based on packed binary file
	void UpdateModel(const char* filename);

private:
	void InitVAO() override;
};

inline ChessModel::ChessModel(const ChessPiece type, const glm::vec3 worldPos)
{
	ChessModel::type = type;

	// Set vertices and indices array
	const auto filetype = ".bin";
	UpdateModel((GetType() + filetype).c_str());

	InitVAO();
}

inline std::string ChessModel::GetType() const
{
	switch (type)
	{
	case empty:
		return "empty";
		break;
	case pawn:
		return "pawn";
		break;
	case rook:
		return "rook";
		break;
	case knight:
		return "knight";
		break;
	case bishop:
		return "bishop";
		break;
	case queen:
		return "queen";
		break;
	case king:
		return "king";
		break;
	}
	return "error";
}

inline void ChessModel::UpdateModel(const char* filename)
{
	// Local file path
	const auto localDir = "/Resources/Models/chess_models/";

	// Initializing empty unsigned int variables
	unsigned int vertexLen;
	unsigned int indLen;

	// Open file
	std::ifstream is((std::filesystem::current_path().string() + localDir + filename).c_str(),
	                 std::ios::out | std::ios::binary);
	// Navigate to start
	is.seekg(0);

	// Get vertex amount and indice amount
	is.read((char*)&vertexLen, sizeof(unsigned int));
	is.read((char*)&indLen, sizeof(unsigned int));

	glm::vec3 temp;
	ModelPt temp_pt{};
	for (int i = 0; i < static_cast<int>(vertexLen); i++)
	{
		is.read((char*)&temp, sizeof(glm::vec3));
		temp_pt.position = temp;
		is.read((char*)&temp, sizeof(glm::vec3));
		temp_pt.normal = temp;
		vertices.push_back(temp_pt);
	}

	GLuint temp_i;
	for (int i = 0; i < static_cast<int>(indLen); i++)
	{
		is.read((char*)&temp_i, sizeof(unsigned int));
		indices.push_back(temp_i);
	}
	is.close();
}

inline void ChessModel::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}
