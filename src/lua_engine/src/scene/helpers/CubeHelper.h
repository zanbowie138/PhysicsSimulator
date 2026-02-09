#pragma once

#include "../MeshHelper.h"
#include "math/mesh/SimpleShapes.h"

namespace SceneImporterInternal {
    class CubeHelper : public MeshHelper {
    public:
        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            const auto cubeData = Utils::CubeData();
            // We need to use shared_ptr or manage memory for the Mesh if it's not copied. 
            // Mesh copy constructor seems to exist.
            Mesh cube(cubeData); 
            
            // Apply common settings with "flat" as default shader
            ApplyCommonSettings(cube, cfg, shaders, "flat");

            return cube.mEntityID;
        }

        std::string GetName() override { return "CreateCube"; }
    };
}
