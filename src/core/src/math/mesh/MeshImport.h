#pragma once
#include <filesystem>
#include <fstream>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "core/GlobalTypes.h"

namespace Utils
{
    static MeshData ReadSTL(const char* filepath)
    {
        // STL File format: https://people.sc.fsu.edu/~jburkardt/data/stlb/stlb.html
        std::vector<MeshPt> points;
        std::vector<unsigned int> indices;

        std::unordered_map<glm::vec3, unsigned int, std::hash<glm::vec3>> pointToIndex;

        std::ifstream is(filepath, std::ios::binary);

        // First 80 chars is the header (title)
        is.seekg(80);

        // Next is the number of triangles, which is an unsigned integer (4 bytes or 4 chars).  Buffer index is offset by 80 chars
        unsigned int numTriangles;
        is.read(reinterpret_cast<char*>(&numTriangles), sizeof(unsigned int));

        unsigned int currentIndex = 0;

        for (unsigned int t = 0; t < numTriangles; t++)
        {
            // Jump to triangle location
            is.seekg(t * 50 + 84);

            // Normal vector for entire triangle
            glm::vec3 tempNormal;
            is.read(reinterpret_cast<char*>(&tempNormal), sizeof(glm::vec3));

            // 3 points per triangle
            for (int p = 0; p < 3; p++)
            {
                glm::vec3 tempPos;
                is.read(reinterpret_cast<char*>(&tempPos), sizeof(glm::vec3));

                auto iterator = pointToIndex.find(tempPos);
                if (iterator == pointToIndex.end())
                {
                    // If the current point is new
                    pointToIndex.insert(std::make_pair(tempPos, currentIndex));
                    points.emplace_back(MeshPt{ tempPos, tempNormal });
                    indices.push_back(currentIndex);
                    currentIndex++;
                }
                else
                {
                    indices.push_back(iterator->second);
                    // Add normal to existing point normal
                    points[iterator->second].normal = points[iterator->second].normal + tempNormal;
                }
            }
        }
        is.close();

        for (auto& point : points)
        {
            point.normal = glm::normalize(point.normal);
        }

        return MeshData{ points, indices };
    }

	/*
	Format:
	# of vertexes, unsigned int - 4 bytes
	# of indices, unsigned int - 4 bytes

	For each vertex:
		vec3 position, 3 floats with 4 bytes each
		vec3 normal (normalized), 3 floats with 4 bytes each

	For each index
		unsigned int index, 4 bytes
	*/
	static MeshData ReadPackedSTL(const char* filepath)
	{
		// Reading packed file
		std::ifstream is(filepath, std::ios::out | std::ios::binary);
		is.seekg(0);

		// Get vertex amount
		unsigned int vertexAmount;
		is.read(reinterpret_cast<char*>(&vertexAmount), sizeof(unsigned int));

		// Get index amount
		unsigned int indexAmount;
		is.read(reinterpret_cast<char*>(&indexAmount), sizeof(unsigned int));

		// Read position and normal information
		MeshPt tempPt{};
		std::vector<MeshPt> vertices;
		vertices.reserve(vertexAmount);
		for (int i = 0; i < static_cast<int>(vertexAmount); i++)
		{
			is.read(reinterpret_cast<char*>(&tempPt.position), sizeof(glm::vec3));
			is.read(reinterpret_cast<char*>(&tempPt.normal), sizeof(glm::vec3));
			vertices.emplace_back(tempPt);
		}

		// Read index information
		unsigned int tempIndex;
		std::vector<unsigned int> indices;
		indices.reserve(indexAmount);
		for (int i = 0; i < static_cast<int>(indexAmount); i++)
		{
			is.read(reinterpret_cast<char*>(&tempIndex), sizeof(unsigned int));
			indices.push_back(tempIndex);
		}
		is.close();

		return MeshData{ vertices, indices };
	}
}
