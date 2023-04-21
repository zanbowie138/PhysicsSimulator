#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <set>
#include <string>

#include "components/Renderable.h"
#include "Camera.h"
#include "VBO.h"
#include "EBO.h"

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

class ChessModel: public Renderable
{
public:

	ChessPiece type;

	std::vector <ModelPt> vertices;
	std::vector <GLuint> indices;

	// Initializes the object
	ChessModel(ChessPiece type, glm::vec3 worldPos);

	std::string getType();

	void Draw(Shader& shader, Camera& camera);

	// Updates the vertices and indices vectors based on packed binary file
	void updateModel(const char* filename);
};

ChessModel::ChessModel(ChessPiece type, glm::vec3 worldPos)
{

    ChessModel::type = type;

    // Set vertices and indices array
    const char* filetype = ".bin";
    updateModel((getType() + filetype).c_str());

    ChessModel::worldPos = worldPos;
    updateModelMat();

    VAO.Bind();

    VBO VBO(vertices);
    EBO EBO(indices);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), (void*)0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

std::string ChessModel::getType() 
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

void ChessModel::Draw(Shader& shader, Camera& camera)
{
    // Bind shader to be able to access uniforms
    shader.Activate();
    VAO.Bind();

    // Update position
    glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Take care of the camera Matrix
    glUniform3f(shader.GetUniformLocation("camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    // Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void ChessModel::updateModel(const char* filename)
{
    // Local file path
    const char* localDir = "/Resources/Models/chess_models/";

    // Initializing empty unsigned int variables
    unsigned int vertex_len;
    unsigned int ind_len;

    // Open file
    std::ifstream is((std::filesystem::current_path().string() + localDir + filename).c_str(), std::ios::out | std::ios::binary);
    // Navigate to start
    is.seekg(0);

    // Get vertex amount and indice amount
    is.read((char*)&vertex_len, sizeof(unsigned int));
    is.read((char*)&ind_len, sizeof(unsigned int));

    glm::vec3 temp;
    ModelPt temp_pt;
    for (int i = 0; i < (int)vertex_len; i++)
    {
        is.read((char*)&temp, sizeof(glm::vec3));
        temp_pt.position = temp;
        is.read((char*)&temp, sizeof(glm::vec3));
        temp_pt.normal = temp;
        vertices.push_back(temp_pt);
    }

    GLuint temp_i;
    for (int i = 0; i < (int)ind_len; i++)
    {
        is.read((char*)&temp_i, sizeof(unsigned int));
        indices.push_back(temp_i);
    }
    is.close();
}