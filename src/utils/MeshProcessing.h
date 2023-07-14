#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "../core/GlobalTypes.h"

namespace Utils
{
	const std::vector<unsigned> triIdxOffset =
	{
		0,1,
		1,2,
		2,0
	};

	static ShapeData CalculatePairs(const MeshData& inputData)
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

	static ShapeData CalculatePairs(const MeshData& inputData, const glm::mat4& modelMat)
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
}