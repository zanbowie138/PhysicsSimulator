#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include "utils/PathUtils.h"
#include "core/World.h"
#include "components/Components.h"
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderer/Texture.h"
#include "math/mesh/SimpleShapes.h"
#include "utils/Logger.h"

extern World world;

namespace SceneImporterInternal {
    inline glm::vec3 GetVec3(sol::optional<sol::table> t, glm::vec3 def) {
        if (!t) return def;
        auto tbl = t.value();
        return glm::vec3(
            tbl[1].get_or(def.x),
            tbl[2].get_or(def.y),
            tbl[3].get_or(def.z)
        );
    }

    inline Entity CreateCubeHelper(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) {
        glm::vec3 position = GetVec3(cfg["position"], glm::vec3(0.0f));
        float scale = cfg["scale"].get_or(1.0f);
        glm::vec3 rotation = GetVec3(cfg["rotation"], glm::vec3(0.0f));
        std::string shaderName = cfg["shader"].get_or(std::string("flat"));
        glm::vec3 color = GetVec3(cfg["color"], glm::vec3(1.0f));

        GLuint shaderID = 999;
        auto it = shaders.find(shaderName);
        if (it != shaders.end()) {
            shaderID = it->second;
        } else {
            LOG(LOG_ERROR) << "Shader '" << shaderName << "' not found, using default ID 999\n";
        }

        const auto cubeData = Utils::CubeData();
        Mesh cube(cubeData);
        cube.SetPosition(position);
        cube.Scale(scale);
        cube.SetRotation(rotation);
        cube.ShaderID = shaderID;
        cube.SetColor(color);
        cube.AddToECS();

        return cube.mEntityID;
    }

    inline Entity CreateFloorHelper(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) {
        float scale = cfg["scale"].get_or(1.0f);
        std::string shaderName = cfg["shader"].get_or(std::string("default"));
        sol::optional<std::string> textureName = cfg["texture"];
        sol::optional<std::string> specularName = cfg["specular"];

        GLuint shaderID = 999;
        auto it = shaders.find(shaderName);
        if (it != shaders.end()) {
            shaderID = it->second;
        } else {
            LOG(LOG_ERROR) << "Shader '" << shaderName << "' not found, using default ID 999\n";
        }

        const ModelData planeData = Utils::PlaneData();
        Entity entityID;

        // Create textures and Model based on what textures are specified
        if (textureName && specularName) {
            auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
            auto specularTex = Texture::Load(specularName.value().c_str(), GL_TEXTURE_2D, GL_RED, GL_UNSIGNED_BYTE);
            if (diffuseTex && specularTex) {
                Model floor(planeData, *diffuseTex, *specularTex);
                floor.Scale(scale);
                floor.ShaderID = shaderID;
                floor.AddToECS();
                entityID = floor.mEntityID;
            } else if (diffuseTex) {
                Model floor(planeData.vertices, planeData.indices, *diffuseTex);
                floor.Scale(scale);
                floor.ShaderID = shaderID;
                floor.AddToECS();
                entityID = floor.mEntityID;
            } else {
                Model floor(planeData);
                floor.Scale(scale);
                floor.ShaderID = shaderID;
                floor.AddToECS();
                entityID = floor.mEntityID;
            }
        } else if (textureName) {
            auto diffuseTex = Texture::Load(textureName.value().c_str(), GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);
            if (diffuseTex) {
                Model floor(planeData.vertices, planeData.indices, *diffuseTex);
                floor.Scale(scale);
                floor.ShaderID = shaderID;
                floor.AddToECS();
                entityID = floor.mEntityID;
            } else {
                Model floor(planeData);
                floor.Scale(scale);
                floor.ShaderID = shaderID;
                floor.AddToECS();
                entityID = floor.mEntityID;
            }
        } else {
            Model floor(planeData);
            floor.Scale(scale);
            floor.ShaderID = shaderID;
            floor.AddToECS();
            entityID = floor.mEntityID;
        }

        return entityID;
    }

    inline Entity CreateSphereHelper(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) {
        glm::vec3 position = GetVec3(cfg["position"], glm::vec3(0.0f));
        float scale = cfg["scale"].get_or(1.0f);
        std::string shaderName = cfg["shader"].get_or(std::string("basic"));
        glm::vec3 color = GetVec3(cfg["color"], glm::vec3(1.0f));

        GLuint shaderID = 999;
        auto it = shaders.find(shaderName);
        if (it != shaders.end()) {
            shaderID = it->second;
        } else {
            LOG(LOG_ERROR) << "Shader '" << shaderName << "' not found, using default ID 999\n";
        }

        const ModelData sphereData = Utils::UVSphereData(20, 20, 1);
        Model sphere(sphereData);
        sphere.SetPosition(position);
        sphere.Scale(scale);
        sphere.ShaderID = shaderID;
        sphere.SetColor(color);
        sphere.AddToECS();

        return sphere.mEntityID;
    }
}

inline bool InitializeSceneFromLua(
    World& world,
    const std::string& filename,
    const std::unordered_map<std::string, GLuint>& shaders
) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);

    lua.set_function("CreateCube", [&](sol::table cfg) {
        return SceneImporterInternal::CreateCubeHelper(cfg, world, shaders);
    });

    lua.set_function("CreateFloor", [&](sol::table cfg) {
        return SceneImporterInternal::CreateFloorHelper(cfg, world, shaders);
    });

    lua.set_function("CreateSphere", [&](sol::table cfg) {
        return SceneImporterInternal::CreateSphereHelper(cfg, world, shaders);
    });

    try {
        lua.script_file(Utils::GetResourcePath("/scenes/", filename));
    } catch (const sol::error& e) {
        LOG(LOG_ERROR) << "Lua error: " << e.what() << "\n";
        return false;
    }

    return true;
}