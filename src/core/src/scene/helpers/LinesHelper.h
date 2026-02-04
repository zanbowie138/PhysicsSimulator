#pragma once

#include "../SceneHelper.h"
#include "renderables/Lines.h"
#include "utils/Logger.h"
#include "utils/Exceptions.h"

class LuaRuntime;

namespace SceneImporterInternal {
    class LinesHelper : public SceneHelper {
    public:
        explicit LinesHelper(LuaRuntime& runtime) : luaRuntime(runtime) {}

        Entity Create(sol::table cfg, World& world,
                     const std::unordered_map<std::string, GLuint>& shaders) override {
            // Validate name parameter
            sol::optional<std::string> nameOpt = cfg["name"];
            if (!nameOpt) {
                LOG(LOG_ERROR) << "CreateLines requires 'name' parameter\n";
                throw SceneException("Lines creation failed: missing 'name'");
            }
            std::string name = nameOpt.value();

            // Extract config with defaults
            GLuint capacity = cfg["capacity"].get_or(1000);
            std::string shaderName = cfg["shader"].get_or(std::string("basic"));
            glm::vec3 color = GetVec3(cfg["color"], glm::vec3(1.0f));

            // Resolve shader ID
            GLuint shaderID = GetShaderID(shaderName, shaders);

            // Create Lines object
            auto lines = std::make_unique<Lines>(capacity);
            lines->ShaderID = shaderID;
            lines->mColor = color;
            lines->AddToECS();

            Entity entityID = lines->mEntityID;

            // Transfer ownership and register for Lua access
            luaRuntime.RegisterDebugLines(name, lines.get());
            luaRuntime.TakeOwnership(std::move(lines));

            LOG(LOG_INFO) << "Created Lines '" << name
                         << "' (capacity=" << capacity << ", entity=" << entityID << ")\n";

            return entityID;
        }

        std::string GetName() override { return "CreateLines"; }

    private:
        LuaRuntime& luaRuntime;

        // Parse vec3 from lua table {r,g,b}
        glm::vec3 GetVec3(sol::optional<sol::table> t, glm::vec3 def) {
            if (!t) return def;
            auto tbl = t.value();
            return glm::vec3(
                tbl[1].get_or(def.x),
                tbl[2].get_or(def.y),
                tbl[3].get_or(def.z)
            );
        }

        // Lookup shader ID by name
        GLuint GetShaderID(const std::string& shaderName,
                          const std::unordered_map<std::string, GLuint>& shaders) {
            auto it = shaders.find(shaderName);
            if (it != shaders.end()) {
                return it->second;
            }
            LOG(LOG_ERROR) << "Shader '" << shaderName << "' not found, using fallback\n";
            return 999;
        }
    };
}
