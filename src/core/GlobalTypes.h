#pragma once
#include <bitset>
#include "glm/glm.hpp"

constexpr unsigned int MAX_ENTITIES = 500;
constexpr unsigned int MAX_COMPONENTS = 10;

namespace Constants
{
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
}


// EntityID
using Entity = unsigned int;

// Basically an array of bools identifying what components are being used
using Signature = std::bitset<MAX_COMPONENTS>;

// Component ID
using ComponentType = uint8_t;


// Input
enum class InputButtons
{
	LEFT_MOUSE,
	RIGHT_MOUSE,
	W,
	A,
	S,
	D,
	SPACE,
	CONTROL,
	SHIFT,
	ENUM_LENGTH
};
using InputBitset = std::bitset<static_cast<size_t>(InputButtons::ENUM_LENGTH)>;

enum class UniformBlockConfig
{
	CAMERA,
	LIGHTING,
	ENUM_LENGTH
};
using UBBitset = std::bitset<static_cast<size_t>(UniformBlockConfig::ENUM_LENGTH)>;

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

// For interpreting stl files or models without textures
struct MeshPt
{
	glm::vec3 position;
	glm::vec3 normal;
};

// For user created meshes with textures
struct ModelPt
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

// Position and normal information
struct MeshData
{
	std::vector<MeshPt> vertices;
	std::vector<GLuint> indices;
};

// Position, normal, and uv information
struct ModelData
{
	std::vector<ModelPt> vertices;
	std::vector<GLuint> indices;
};