#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include "../core/GlobalTypes.h"

namespace Utils
{
	static std::pair<std::vector<Vertex>, std::vector<GLuint>> CubeData(const bool reverseNormals = false)
	{
		const std::vector<glm::vec3> boxVertices =
		{
			glm::vec3(-0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, 0.5f, 0.5f),
			glm::vec3(-0.5f, 0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, 0.5f)
		};
		const std::vector<glm::vec3> boxNormals =
		{
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, -1),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 1, 0)
		};
		const std::vector<GLuint> boxIndices =
		{
			0, 1, 2, // Down
			0, 2, 3,
			0, 4, 7, // Front
			0, 7, 3,
			3, 7, 6, // Right
			3, 6, 2,
			2, 6, 5, // Back
			2, 5, 1,
			1, 5, 4, // Left
			1, 4, 0,
			4, 5, 6, // Top
			4, 6, 7
		};
		std::vector<Vertex> points;
		std::vector<GLuint> indices;
		size_t count = 0;
		for (int i = 0; i < boxIndices.size(); i++)
		{
			points.push_back(Vertex{boxVertices[boxIndices[i]], boxNormals[i / 6] * (reverseNormals ? 1.0f : -1.0f)});
			indices.push_back(i);
		}

		return std::pair<std::vector<Vertex>, std::vector<GLuint>>{points, indices};
	}

	static std::pair<std::vector<Vertex>, std::vector<GLuint>> PlaneData()
	{
		std::vector<Vertex> board_vertexes
		{
			Vertex{glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
			Vertex{glm::vec3(0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
			Vertex{glm::vec3(0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)},
			Vertex{glm::vec3(-0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)}
		};
		std::vector<GLuint> board_indices
		{
			0, 1, 2,
			2, 3, 0
		};
		return std::pair<std::vector<Vertex>, std::vector<GLuint>>{board_vertexes, board_indices};
	}
}
