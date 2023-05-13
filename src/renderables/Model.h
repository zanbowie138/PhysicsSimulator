#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <set>

#include "../core/GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "Renderable.h"
#include "../physics/Collidable.h"

struct CompareVec3
{
	// Adapted from https://stackoverflow.com/questions/46636721/how-do-i-use-glm-vector-relational-functions
	bool operator()(const std::tuple<glm::vec3, glm::vec3, unsigned int>& lhs,
	                const std::tuple<glm::vec3, glm::vec3, unsigned int>& rhs) const
	{
		glm::vec3 equal = notEqual(std::get<0>(lhs), std::get<0>(rhs));
		return lessThan(std::get<0>(lhs), std::get<0>(rhs))[equal[0] ? 0 : (equal[1] ? 1 : 2)];
	}
};

class Model: public Renderable
{
public:
	std::vector<ModelPt> vertices;
	std::vector<GLuint> indices;

	// Initializes the object
	Model(const char* filename, bool is_stl);

	BoundingBox CalcBoundingBox();

private:
	// Reads an stl file
	void ReadSTL(const char* filepath);

	// Reads a binary file
	void ReadBIN(const char* filepath);

	void InitVAO() override;
	size_t GetSize() override;
};

inline Model::Model(const char* filename, const bool is_stl)
{
	const auto localDir = "/Resources/Models/";
	if (!is_stl)
	{
		ReadBIN((std::filesystem::current_path().string() + localDir + filename).c_str());
	}
	else
	{
		ReadSTL((std::filesystem::current_path().string() + localDir + filename).c_str());
	}

	InitVAO();
}

inline BoundingBox Model::CalcBoundingBox()
{
	BoundingBox box;
	transform.CalculateModelMat();
	box.min = transform.modelMat * glm::vec4(vertices[0].position, 1.0f);
	box.max = transform.modelMat * glm::vec4(vertices[0].position, 1.0f);
	for (const auto& pt : vertices)
	{
		auto point = transform.modelMat * glm::vec4(pt.position, 1.0f);
		for (unsigned int i = 0; i < 3; i++)
		{
			box.max[i] = std::max(box.max[i], point[i]);
			box.min[i] = std::min(box.min[i], point[i]);
		}
	}
	return box;
}

inline void Model::ReadSTL(const char* filepath)
{
	// STL File format: https://people.sc.fsu.edu/~jburkardt/data/stlb/stlb.html

	// Vertice will have position and normal information
	std::set<std::tuple<glm::vec3, glm::vec3, unsigned int>, CompareVec3> vertexes;
	std::vector<unsigned int> indices;

	std::string localDir = "";
	std::ifstream is((localDir + filepath), std::ios::binary);

	// First 80 chars is the header (title)
	// Next is the number of triangles, which is an unsigned integer (4 bytes or 4 chars).  Buffer index is offset by 80 chars
	is.seekg(80);
	unsigned int numTriangles;
	is.read((char*)&numTriangles, sizeof(unsigned int));

	glm::vec3 temp;
	unsigned int indice = 0;
	std::pair<std::set<std::tuple<glm::vec3, glm::vec3, unsigned int>>::iterator, bool> temp_pair;
	glm::vec3 temp_normal;

	for (int t = 0; t < static_cast<int>(numTriangles); t++)
	{
		is.seekg(84 + 50 * t);

		// Normal vector
		is.read((char*)&temp_normal, sizeof(glm::vec3));

		// 3 Vertexes
		for (int i = 0; i < 3; i++)
		{
			is.read((char*)&temp, sizeof(glm::vec3));
			temp_pair = vertexes.insert(std::tuple<glm::vec3, glm::vec3, unsigned int>(temp, temp_normal, indice));

			// If inserted successfully, increment indice count and push a new indice value.  Else, find the copied vertex and push it's indice value.
			if (temp_pair.second)
			{
				indices.push_back(indice);
				indice++;
			}
			else
			{
				std::tuple<glm::vec3, glm::vec3, unsigned int> temp_vertex = (*(temp_pair.first));
				vertexes.erase(temp_vertex);

				indices.push_back(std::get<2>(temp_vertex));

				temp_vertex = std::make_tuple(std::get<0>(temp_vertex), std::get<1>(temp_vertex) + temp_normal,
				                              std::get<2>(temp_vertex));
				vertexes.insert(temp_vertex);
			}
		}
	}

	is.close();

	std::vector<std::pair<glm::vec3, glm::vec3>> vertexes_sorted(vertexes.size());

	for (const std::tuple<glm::vec3, glm::vec3, unsigned int>& t : vertexes)
	{
		vertexes_sorted[std::get<2>(t)] = std::pair<glm::vec3, glm::vec3>{std::get<0>(t), std::get<1>(t)};
	}

	std::vector<ModelPt> verticesVector;

	verticesVector.reserve(vertexes_sorted.size());
	for (const std::pair<glm::vec3, glm::vec3>& t : vertexes_sorted)
	{
		verticesVector.push_back(ModelPt{t.first, t.second});
	}

	vertices = verticesVector;
	Model::indices = indices;
}

inline void Model::ReadBIN(const char* filepath)
{
	// Reading packed file
	unsigned int vertex_len;
	unsigned int ind_len;

	std::ifstream is(filepath, std::ios::out | std::ios::binary);
	is.seekg(0);
	is.read((char*)&vertex_len, sizeof(unsigned int));
	is.read((char*)&ind_len, sizeof(unsigned int));

	glm::vec3 temp;
	ModelPt temp_pt{};
	for (int i = 0; i < static_cast<int>(vertex_len); i++)
	{
		is.read((char*)&temp, sizeof(glm::vec3));
		temp_pt.position = temp;
		is.read((char*)&temp, sizeof(glm::vec3));
		temp_pt.normal = temp;
		vertices.push_back(temp_pt);
	}

	GLuint temp_i;
	for (int i = 0; i < static_cast<int>(ind_len); i++)
	{
		is.read((char*)&temp_i, sizeof(unsigned int));
		indices.push_back(temp_i);
	}
	is.close();
}

inline void Model::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));

	mVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

inline size_t Model::GetSize()
{
	return indices.size();
}
