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
#include "../utils/ModelImport.h"
#include "physics/DynamicTree.h"

class Model: public Renderable
{
public:
	std::vector<ModelPt> vertices;
	std::vector<GLuint> indices;

	Physics::DynamicBBTree<GLuint> mTree;

	// Initializes the object
	Model(const char* filename, bool is_stl);

	BoundingBox CalcBoundingBox();
	void InitTree();

private:
	void InitVAO() override;
	size_t GetSize() override;
};

inline Model::Model(const char* filename, const bool is_stl)
{
	const auto localDir = "/Resources/Models/";
	std::pair<std::vector<ModelPt>, std::vector<unsigned int>> model;
	if (!is_stl)
	{
		model = Utils::ReadPackedSTL((std::filesystem::current_path().string() + localDir + filename).c_str());
	}
	else
	{
		model = Utils::ReadSTL((std::filesystem::current_path().string() + localDir + filename).c_str());
	}

	vertices = model.first;
	indices = model.second;

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

inline void Model::InitTree()
{
	std::cout << "Starting init" << std::endl;
	mTree = Physics::DynamicBBTree<GLuint>{ indices.size() / 3 };
	Entity triID = 0;
	transform.CalculateModelMat();
	for (size_t t = 0; t < indices.size()/3; t++)
	{
		BoundingBox box;
		box.min = transform.modelMat * glm::vec4(vertices[indices[t*3]].position, 1.0f);
		box.max = box.min;
		for (size_t i = 0; i < 3; i++)
		{
			auto point = transform.modelMat * glm::vec4(vertices[indices[t*3+i]].position, 1.0f);
			for (unsigned int i = 0; i < 3; i++)
			{
				box.max[i] = std::max(box.max[i], point[i]);
				box.min[i] = std::min(box.min[i], point[i]);
			}
		}
		mTree.InsertEntity(triID, box);
		triID++;
	}
	std::cout << "done" << std::endl;
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
