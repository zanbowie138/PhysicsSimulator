#pragma once
#include <bitset>

#ifndef BASE_DIR
#define BASE_DIR std::filesystem::current_path().string()
#endif

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

// Position information
struct ShapeData
{
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
};

/**
 * @struct MeshData
 * @brief The MeshData struct is used to store the data of a 3D model without texture coordinates.
 *
 * @var std::vector<ModelPt> vertices Stores the position, normal, and uv information for each vertex
 */
struct MeshData
{
	/**
	 * @var std::vector<ModelPt> vertices
	 * @brief Stores the position, normal, and uv information for each vertex
	 */
	std::vector<MeshPt> vertices;

	/**
	 * @var std::vector<GLuint> indices
	 * @brief Stores the indices of the vertices that form each triangle
	 */
	std::vector<GLuint> indices;
};

/**
 * @struct ModelData
 * @brief The ModelData struct is used to store the data of a 3D model with texture coordinates.
 */
struct ModelData
{
	/**
	 * @var std::vector<ModelPt> vertices
	 * @brief Stores the position, normal, and uv information for each vertex
	 */
	std::vector<ModelPt> vertices;

	/**
	 * @var std::vector<GLuint> indices
	 * @brief Stores the indices of the vertices that form each triangle
	 */
	std::vector<GLuint> indices;
};
