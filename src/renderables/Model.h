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

class Model: public Renderable
{
public:
	std::vector<ModelPt> vertices;
	std::vector<GLuint> indices;

	// Initializes the object
	Model(const char* filename, bool is_stl);

	BoundingBox CalcBoundingBox();

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
