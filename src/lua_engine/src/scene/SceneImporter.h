#pragma once

#include <sol/sol.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

#include "utils/PathUtils.h"
#include "utils/Logger.h"
#include "utils/Exceptions.h"

// Include all helpers
#include "SceneHelper.h"
#include "helpers/CubeHelper.h"
#include "helpers/FloorHelper.h"
#include "helpers/SphereHelper.h"

struct SceneData {
    Entity lightID = 999;
};

inline std::optional<SceneData> InitializeSceneFromLua(
    World& world,
    const std::string& filename,
    const std::unordered_map<std::string, GLuint>& shaders
) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);

    std::vector<std::unique_ptr<SceneImporterInternal::SceneHelper>> helpers;
    helpers.push_back(std::make_unique<SceneImporterInternal::CubeHelper>());
    helpers.push_back(std::make_unique<SceneImporterInternal::FloorHelper>());
    helpers.push_back(std::make_unique<SceneImporterInternal::SphereHelper>());

    for (const auto& helper : helpers) {
        lua.set_function(helper->GetName(), [&](sol::table cfg) {
            return helper->Create(cfg, world, shaders);
        });
    }

    try {
        sol::table result = lua.script_file(Utils::GetResourcePath("/scenes/", filename));
        
        if (!result.valid()) {
            throw SceneException("Failed to load scene: Lua script did not return a valid table.");
        }

        SceneData data;
        sol::optional<Entity> lightID = result["light"];
        
        if (!lightID) {
            throw SceneException("Failed to load scene: 'light' entity missing from return table.");
        }
        
        data.lightID = lightID.value();
        return data;

    } catch (const sol::error& e) {
        LOG(LOG_ERROR) << "Lua error: " << e.what() << "\n";
        throw SceneException(std::string("Lua script execution failed: ") + e.what());
    }
}