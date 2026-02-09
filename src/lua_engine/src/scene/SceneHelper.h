#pragma once

#include <sol/sol.hpp>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>
#include <glad/glad.h>
#include "core/World.h"

namespace SceneImporterInternal {
    class SceneHelper {
    public:
        virtual ~SceneHelper() = default;
        virtual Entity Create(sol::table cfg, World& world, const std::unordered_map<std::string, GLuint>& shaders) = 0;
        virtual std::string GetName() = 0;
    };
}
