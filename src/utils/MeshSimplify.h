#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "../core/GlobalTypes.h"
#include "MeshProcessing.h"

// https://users.csc.calpoly.edu/~zwood/teaching/csc570/final06/jseeba/
namespace Utils
{
	inline MeshData DecimateMesh(const MeshData& inputData)
	{
		struct Triangle
		{
			size_t v1;
			size_t v2;
			size_t v3;
		};

		// Format: 
		auto CalculateQuadric = [](glm::vec4 planeEquation)
		{
			auto a = planeEquation.x;
			auto b = planeEquation.y;
			auto c = planeEquation.z;
			auto d = planeEquation.w;

			return glm::mat4
			{
				a*a, a*b, a*c, a*d,
				a*b, b*b, b*c, b*d,
				a*c, b*c, c*c, c*d,
				a*d, b*d, c*d, d*d
			};
		};
		// TODO: Make more storage efficient (quadric matrix is symmetrical along diagonal)

		const float threshold = 0;
		const ShapeData edges = CalculateEdges(inputData);

		std::vector<glm::vec3> normals(inputData.vertices.size());
		for (size_t n = 0; n < normals.size(); ++n)
		{
			normals[n] = inputData.vertices[n].normal;
		}

		// Format: Triangle index, quadric
		std::vector<std::vector<size_t>> verticeTriList;

		// Create array of triangles storing each vertice's position
		// Also create quadric for each triangle
		std::vector<Triangle> triangles(edges.indices.size() / 3);
		std::vector<glm::mat4> quadrics(edges.indices.size() / 3);
		for (size_t t = 0; t < edges.indices.size(); ++t)
		{
			triangles[t].v1 = edges.indices[t * 3];
			triangles[t].v2 = edges.indices[t * 3 + 1];
			triangles[t].v3 = edges.indices[t * 3 + 2];

			glm::vec3 normal = normals[t];
			// TODO: check algorithm
			float offset = -glm::dot(triangles[t], normal);
			glm::vec4 planeEquation = glm::vec4(normal, offset);

			quadrics[i] = CalculateQuadric(planeEquation);
		}
	}
}