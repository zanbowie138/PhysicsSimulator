#pragma once

#include "../ModelHelper.h"
#include "math/mesh/SimpleShapes.h"

namespace SceneImporterInternal {
    class FloorHelper : public ModelHelper {
    public:
        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            // Specialized logic for floor as it has texture handling that's slightly more complex than just "apply common"
            // But we can still reuse parts or just do it manually as before.
            
            float scale = cfg["scale"].get_or(1.0f);
            std::string shaderName = cfg["shader"].get_or(std::string("default"));
            sol::optional<std::string> textureName = cfg["texture"];
            sol::optional<std::string> specularName = cfg["specular"];

            GLuint shaderID = GetShaderID(shaderName, shaders);
            const ModelData planeData = Utils::PlaneData();
            Entity entityID;

            // Helper lambda or just logic to construct the correct model
            // Refactored to avoid deep nesting
            
            // Texture loading logic
             if (textureName && specularName) {
                auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
                auto specularTex = Texture::Load(specularName.value().c_str(), GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE);
                if (diffuseTex && specularTex) {
                    Model floor(planeData, *diffuseTex, *specularTex);
                    ConfigureFloor(floor, scale, shaderID);
                    entityID = floor.mEntityID;
                    return entityID;
                }
            }
            
            if (textureName) {
                auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
                if (diffuseTex) {
                    Model floor(planeData.vertices, planeData.indices, *diffuseTex);
                    ConfigureFloor(floor, scale, shaderID);
                    entityID = floor.mEntityID;
                    return entityID;
                }
            }

            // Fallback no texture
            Model floor(planeData);
            ConfigureFloor(floor, scale, shaderID);
            entityID = floor.mEntityID;

            return entityID;
        }

        std::string GetName() override { return "CreateFloor"; }

    private:
        void ConfigureFloor(Model& floor, float scale, GLuint shaderID) {
            floor.Scale(scale);
            floor.ShaderID = shaderID;
            floor.AddToECS();
        }
    };
}
