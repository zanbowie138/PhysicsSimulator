#pragma once

#include "../ModelHelper.h"
#include "math/mesh/SimpleShapes.h"

namespace SceneImporterInternal {
    class SphereHelper : public ModelHelper {
    public:
        Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) override {
            const ModelData sphereData = Utils::UVSphereData(20, 20, 1);
            Model sphere(sphereData);
            
            // Apply common settings with "basic" as default shader
            ApplyCommonSettings(sphere, cfg, shaders, "basic");

            return sphere.mEntityID;
        }

        std::string GetName() override { return "CreateSphere"; }
    };
}
