#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "../core/GlobalTypes.h"
#include "MeshProcessing.h"

namespace Utils
{
	static MeshData DecimateMesh(const MeshData& inputData)
	{
		// TODO: Make more storage efficient (quadric matrix is symmetrical along diagonal)

		const float threshold = 0;
		const ShapeData validPairs = CalculatePairs(inputData);
		std::vector<glm::mat4> quadrics(validPairs.indices.size());

	}
}