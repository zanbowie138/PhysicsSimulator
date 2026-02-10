#pragma once
#include <filesystem>
#include <fstream>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "core/GlobalTypes.h"

namespace Utils
{
	// Defines what index offsets define the edges of a triangle
	const std::vector<unsigned> triIdxOffset =
	{
		0,1,
		1,2,
		2,0
	};

	const std::vector<GLuint> cubeIdxOffset =
	{
		0,1,
		1,2,
		2,3,
		3,0,
		4,5,
		5,6,
		6,7,
		7,4,
		0,4,
		1,5,
		2,6,
		3,7
	};

	static std::vector<glm::vec3> GetCubeVertices(const BoundingBox& box)
	{
		return std::vector
		{
			glm::vec3(box.max),
			glm::vec3(box.max.x, box.max.y, box.min.z),
			glm::vec3(box.min.x, box.max.y, box.min.z),
			glm::vec3(box.min.x, box.max.y, box.max.z),
			glm::vec3(box.max.x, box.min.y, box.max.z),
			glm::vec3(box.max.x, box.min.y, box.min.z),
			glm::vec3(box.min),
			glm::vec3(box.min.x, box.min.y, box.max.z)
		};
	}

	// Returns the positions and indices that make up the triangle edges
	static ShapeData CalculateEdges(const MeshData& inputData)
	{
		std::vector<glm::vec3> positions(inputData.vertices.size());
		std::vector<unsigned> indices(inputData.indices.size() * 2);

		for (size_t i = 0; i < inputData.vertices.size(); ++i)
		{
			positions[i] = inputData.vertices[i].position;
		}

		for (size_t i = 0; i < inputData.indices.size(); i += 3)
		{
			for (size_t j = 0; j < 6; j++)
			{
				indices[i * 2 + j] = inputData.indices[i + triIdxOffset[j]];
			}
		}

		return ShapeData{ positions, indices };
	}

	static ShapeData CalculateEdges(const MeshData& inputData, const glm::mat4& modelMat)
	{
		std::vector<glm::vec3> positions(inputData.vertices.size());
		std::vector<unsigned> indices(inputData.indices.size() * 2);

		for (size_t i = 0; i < inputData.vertices.size(); ++i)
		{
			positions[i] = modelMat * glm::vec4(inputData.vertices[i].position, 1.0f);
		}

		for (size_t i = 0; i < inputData.indices.size(); i += 3)
		{
			for (size_t j = 0; j < 6; j++)
			{
				indices[i * 2 + j] = inputData.indices[i + triIdxOffset[j]];
			}
		}

		return ShapeData{ positions, indices };
	}

	static ShapeData CalculateEdges(const BoundingBox& box, size_t idxOffset = 0)
	{
		std::vector<glm::vec3> tempVertices(8);
		std::vector<GLuint> tempIndices(24);

		auto cubeVerts = GetCubeVertices(box);
		std::copy(cubeVerts.begin(), cubeVerts.end(), tempVertices.begin());

		for (size_t i = 0; i < 24; ++i)
		{
			tempIndices[i] = cubeIdxOffset[i] + idxOffset;
		}

		return ShapeData{ tempVertices, tempIndices };
	}

	static ShapeData CalculateEdges(const std::vector<BoundingBox>& boxes, size_t idxOffset = 0)
	{
		size_t vIdx = 0;
		size_t iIdx = 0;

		std::vector<glm::vec3> tempVertices(boxes.size() * 8);
		std::vector<GLuint> tempIndices(boxes.size() * 24);

		for (const auto& box : boxes)
		{
			for (size_t i = 0; i < 24; ++i)
			{
				tempIndices[iIdx] = static_cast<GLuint>(vIdx) + cubeIdxOffset[i] + idxOffset;
				++iIdx;
			}

			auto cubeVerts = GetCubeVertices(box);
			std::copy(cubeVerts.begin(), cubeVerts.end(), tempVertices.begin() + vIdx);
			vIdx += 8;
		}

		return ShapeData{ tempVertices, tempIndices };
	}
}