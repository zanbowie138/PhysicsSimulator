#pragma once

#include "SceneHelper.h"
#include "utils/Logger.h"
#include "utils/Exceptions.h"
#include "renderables/Renderable.h"

namespace SceneImporterInternal {
    class RenderableHelper : public SceneHelper {
    protected:
        glm::vec3 GetVec3(sol::optional<sol::table> t, glm::vec3 def) {
            if (!t) return def;
            auto tbl = t.value();
            return glm::vec3(
                tbl[1].get_or(def.x),
                tbl[2].get_or(def.y),
                tbl[3].get_or(def.z)
            );
        }

        GLuint GetShaderID(const std::string& shaderName, const std::unordered_map<std::string, GLuint>& shaders) {
            auto it = shaders.find(shaderName);
            if (it != shaders.end()) {
                return it->second;
            }

            // Try fallback to "basic" shader
            LOG(LOG_ERROR) << "Shader '" << shaderName << "' not found\n";
            auto fallback = shaders.find("basic");
            if (fallback != shaders.end()) {
                LOG(LOG_INFO) << "Using 'basic' shader as fallback\n";
                return fallback->second;
            }

            // Fatal: no shader available
            throw SceneException("Shader '" + shaderName + "' not found and no 'basic' fallback");
        }

        void ApplyCommonSettings(Renderable& renderable, sol::table cfg, const std::unordered_map<std::string, GLuint>& shaders, const std::string& defaultShaderName) {
            glm::vec3 position = GetVec3(cfg["position"], glm::vec3(0.0f));
            float scale = cfg["scale"].get_or(1.0f);
            glm::vec3 rotation = GetVec3(cfg["rotation"], glm::vec3(0.0f));
            std::string shaderName = cfg["shader"].get_or(defaultShaderName);
            glm::vec3 color = GetVec3(cfg["color"], glm::vec3(1.0f));

            renderable.ShaderID = GetShaderID(shaderName, shaders);

            renderable.SetPosition(position);
            renderable.Scale(scale);
            renderable.SetRotation(rotation);
            renderable.SetColor(color);
            renderable.AddToECS();
        }
    };
}
