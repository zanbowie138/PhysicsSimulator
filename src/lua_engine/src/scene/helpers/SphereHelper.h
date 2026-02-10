#pragma once

#include "../ModelHelper.h"
#include "math/mesh/SimpleShapes.h"

class LuaRuntime;

namespace SceneImporterInternal {
    class SphereHelper : public ModelHelper {
    public:
        explicit SphereHelper(LuaRuntime& runtime) : luaRuntime(runtime) {}

        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            static std::vector<ModelData> sphereDataStorage;
            sphereDataStorage.push_back(Utils::UVSphereData(20, 20, 1));

            Model sphere(sphereDataStorage.back());

            ApplyCommonSettings(sphere, cfg, shaders, "basic");

            luaRuntime.RegisterPhysics(sphere.mEntityID, sphere.CalcBoundingBox());
            return sphere.mEntityID;
        }

        std::string GetName() override { return "CreateSphere"; }

    private:
        LuaRuntime& luaRuntime;
    };
}
