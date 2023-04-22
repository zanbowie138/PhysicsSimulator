#pragma once

#include <string>
#include <string>
#include <vector>

#include "../Camera.h"
#include "../EBO.h"
#include "../VBO.h"
#include "../components/Renderable.h"

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

class ChessModel : public Renderable
{
public:
	ChessPiece type;

	std::vector<ModelPt> vertices;
	std::vector<GLuint> indices;

	// Initializes the object
	ChessModel(ChessPiece type, glm::vec3 worldPos);

	[[nodiscard]] std::string GetType() const;

	void Draw(const Shader& shader, const Camera& camera) const override;

	// Updates the vertices and indices vectors based on packed binary file
	void UpdateModel(const char* filename);
};

inline ChessModel::ChessModel(const ChessPiece type, const glm::vec3 worldPos)
{
	ChessModel::type = type;

	// Set vertices and indices array
	const auto filetype = ".bin";
	UpdateModel((GetType() + filetype).c_str());

	ChessModel::worldPos = worldPos;
	UpdateModelMat();

	VAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), nullptr);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
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

inline void ChessModel::Draw(const Shader& shader, const Camera& camera) const
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();

	// Update position
	glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, value_ptr(modelMatrix));

	// Take care of the camera Matrix
	glUniform3f(shader.GetUniformLocation("camPos"), camera.position.x, camera.position.y, camera.position.z);
	camera.Matrix(shader, "camMatrix");

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
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
