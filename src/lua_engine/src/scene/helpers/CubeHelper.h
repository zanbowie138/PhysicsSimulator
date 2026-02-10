#pragma once

#include "../MeshHelper.h"
#include "math/mesh/SimpleShapes.h"

class LuaRuntime;

namespace SceneImporterInternal {
    class CubeHelper : public MeshHelper {
    public:
        explicit CubeHelper(LuaRuntime& runtime) : luaRuntime(runtime) {}

        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            const auto cubeData = Utils::CubeData();
            Mesh cube(cubeData);

            ApplyCommonSettings(cube, cfg, shaders, "flat");

            luaRuntime.RegisterPhysics(cube.mEntityID, cube.CalcBoundingBox());
            return cube.mEntityID;
        }

        std::string GetName() override { return "CreateCube"; }

    private:
        LuaRuntime& luaRuntime;
    };
}
