#pragma once
#include <string>
#include <vector>

#include "../core/GlobalTypes.h"

#include "../renderer/EBO.h"
#include "../renderer/VBO.h"
#include "../renderer/VAO.h"
#include "../physics/BoundingBox.h"
#include "../renderer/Texture.h"
#include "../math/mesh/OBJ_Loader.h"
#include "../utils/PathUtils.h"

#include "Renderable.h"

class ObjModel
{
public:
	std::vector<objl::Mesh> meshes = {};
	std::vector<glm::vec3> colors = {};
	std::vector<Model> models;
	std::string obj_filepath;

	// Initializes the mesh
    ObjModel(const char* obj_filepath, const char* obj_name);

	void AddToECS();
	void Scale(float scale);
	void SetPosition(glm::vec3 position);
	void ShaderID(GLuint shaderID);
	void SetRotation(glm::vec3 eulerRotation);
private:
	void InitVAOs();
};

inline ObjModel::ObjModel(const char* obj_filepath, const char* obj_name): obj_filepath(obj_filepath)
{
	std::string filepath = Utils::GetResourcePath("/res/models/", std::string(obj_filepath) + obj_name);
	LOG(LOG_INFO) << "Loading model: " << filepath << "\n";
	objl::Loader loader;
	if (loader.LoadFile(filepath)) {
		meshes = loader.LoadedMeshes;
		InitVAOs();
	} else {
		LOG(LOG_ERROR) << "Failed to load model: " << obj_filepath << "\n";
		std::cerr << "Failed to load model: " << obj_filepath << std::endl;
	}
}

inline void ObjModel::InitVAOs()
{
  	for (const auto& mesh : meshes)
  	{
  		LOG(LOG_INFO) << "Loaded mesh: " << mesh.MeshName << " with " << mesh.Vertices.size() << " vertices and " << mesh.Indices.size() << " indices\n";
  		std::vector<ModelPt> vertices;
  		std::vector<GLuint> indices (mesh.Indices.size());
  		for (const auto& vertex : mesh.Vertices) {
  			glm::vec3 position = { vertex.Position.X, vertex.Position.Y, vertex.Position.Z };
  			glm::vec3 normal = { vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z };
  			glm::vec2 uv = { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y };
  			vertices.push_back({ position, normal, uv });
  		}
  		std::copy(mesh.Indices.begin(), mesh.Indices.end(), indices.begin());
  		if (mesh.MeshMaterial.map_Kd.empty() && mesh.MeshMaterial.map_Ks.empty())
  		{
  			models.emplace_back(vertices, indices);
  		}
  		else if (mesh.MeshMaterial.map_Ks.empty())
  		{
  			std::string c = mesh.MeshMaterial.map_Kd;
  			c.erase(std::remove(c.begin(), c.end(), ' '), c.end());
  			std::string filepath = Utils::GetResourcePath("/res/models/", obj_filepath + c);
  			auto diffuseTexture = Texture::Load(filepath.c_str(), GL_TEXTURE_2D, GL_RGBA, GL_RGBA);
  			if (diffuseTexture) {
  				LOG(LOG_INFO) << "Loaded texture: " << mesh.MeshMaterial.map_Kd << "\n";
  				models.emplace_back(vertices, indices, *diffuseTexture);
  			} else {
  				LOG(LOG_WARNING) << "Failed to load texture, creating model without texture\n";
  				models.emplace_back(vertices, indices);
  			}
  		}
  		else {
  			std::string d_filepath = Utils::GetResourcePath("/res/models/", obj_filepath + mesh.MeshMaterial.map_Kd);
  			std::string s_filepath = Utils::GetResourcePath("/res/models/", obj_filepath + mesh.MeshMaterial.map_Ks);
  			auto diffuseTexture = Texture::Load((d_filepath).c_str(), GL_TEXTURE_2D, GL_RGB, GL_RGB);
  			auto specularTexture = Texture::Load((s_filepath).c_str(), GL_TEXTURE_2D, GL_RED, GL_RED);
  			if (diffuseTexture && specularTexture) {
  				LOG(LOG_INFO) << "Loaded textures: " << mesh.MeshMaterial.map_Kd << " and " << mesh.MeshMaterial.map_Ks << "\n";
  				models.emplace_back(vertices, indices, *diffuseTexture, *specularTexture);
  			} else if (diffuseTexture) {
  				LOG(LOG_WARNING) << "Failed to load specular texture, using only diffuse\n";
  				models.emplace_back(vertices, indices, *diffuseTexture);
  			} else {
  				LOG(LOG_WARNING) << "Failed to load textures, creating model without textures\n";
  				models.emplace_back(vertices, indices);
  			}
  		}
	}
}

inline void ObjModel::AddToECS()
{
	for (auto& model : models) {
		model.AddToECS();
	}
}

inline void ObjModel::Scale(float scale)
{
	for (auto& model : models) {
		model.Scale(scale);
	}
}

inline void ObjModel::SetPosition(glm::vec3 position)
{
	for (auto& model : models) {
		model.SetPosition(position);
	}
}

inline void ObjModel::ShaderID(GLuint shaderID)
{
	for (auto& model : models) {
		model.ShaderID = shaderID;
	}
}

inline void ObjModel::SetRotation(glm::vec3 eulerRotation)
{
	for (auto& model : models) {
		model.SetRotation(eulerRotation);
	}
}