#pragma once

#include "../ModelHelper.h"
#include "math/mesh/SimpleShapes.h"
#include "renderer/Texture.h"

class LuaRuntime;

namespace SceneImporterInternal {
    class BoxHelper : public ModelHelper {
    public:
        explicit BoxHelper(LuaRuntime& runtime) : luaRuntime(runtime) {}

        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            const ModelData cubeData = Utils::CubeModelData();

            sol::optional<std::string> textureName = cfg["texture"];
            sol::optional<std::string> specularName = cfg["specular"];

            if (textureName && specularName) {
                auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
                auto specularTex = Texture::Load(specularName.value().c_str(), GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE);
                if (diffuseTex && specularTex) {
                    Model box(cubeData, *diffuseTex, *specularTex);
                    return Configure(box, cfg, shaders);
                }
            }

            if (textureName) {
                auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
                if (diffuseTex) {
                    Model box(cubeData.vertices, cubeData.indices, *diffuseTex);
                    return Configure(box, cfg, shaders);
                }
            }

            Model box(cubeData);
            return Configure(box, cfg, shaders);
        }

        std::string GetName() override { return "CreateBox"; }

    private:
        LuaRuntime& luaRuntime;

        Entity Configure(Model& box, sol::table cfg, const std::unordered_map<std::string, GLuint>& shaders) {
            ApplyCommonSettings(box, cfg, shaders, "flat");

            sol::optional<sol::table> dimTable = cfg["dimensions"];
            if (dimTable) {
                box.Scale(GetVec3(dimTable, glm::vec3(1.0f)));
                box.UpdateECSTransform();
            }

            luaRuntime.RegisterPhysics(box.mEntityID, box.CalcBoundingBox());
            return box.mEntityID;
        }
    };
}
