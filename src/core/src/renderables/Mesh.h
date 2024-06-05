#pragma once
#include <string>
#include <vector>

#include "../core/GlobalTypes.h"

#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"

#include "../math/mesh/MeshImport.h"
#include "../physics/StaticTree.h"
#include "../utils/Timer.h"

#include "Renderable.h"

class Mesh: public Renderable
{
public:
	std::vector<MeshPt> vertices;
	std::vector<unsigned int> indices;

	Physics::StaticTree mTree{};

	// Initializes the object
	Mesh(const char* filename, bool is_stl);
	Mesh(std::vector<MeshPt> vertices, std::vector<unsigned int> indices);
	explicit Mesh(const MeshData& data);

	BoundingBox CalcBoundingBox();
	void InitTree();

	void AddRigidbody();

private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Mesh::Mesh(const char* filename, const bool is_stl)
{
	const std::string localDir = "/res/models/";
	std::string filepath = BASE_DIR + localDir + filename;
	MeshData data;

	if (!is_stl) 
		data = Utils::ReadPackedSTL(filepath.c_str());
	else 
		data = Utils::ReadSTL(filepath.c_str());

	vertices = std::vector(data.vertices);
	indices = std::vector(data.indices);

	Mesh::InitVAO();
}

inline Mesh::Mesh(std::vector<MeshPt> vertices, std::vector<unsigned> indices): vertices(std::move(vertices)), indices(
	std::move(indices))
{
	Mesh::InitVAO();
}

inline Mesh::Mesh(const MeshData& data): vertices(data.vertices), indices(data.indices)
{
	Mesh::InitVAO();
}

inline BoundingBox Mesh::CalcBoundingBox()
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

inline void Mesh::InitTree()
{
	mTree.CreateStaticTree(vertices, indices);
}


inline void Mesh::InitVAO()
{
	mVAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	mVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(MeshPt), nullptr);
	mVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(MeshPt), (void*)(3 * sizeof(float)));

	VAO::Unbind();
	VBO::Unbind();
	EBO::Unbind();
}

inline size_t Mesh::GetSize()
{
	return indices.size();
}
