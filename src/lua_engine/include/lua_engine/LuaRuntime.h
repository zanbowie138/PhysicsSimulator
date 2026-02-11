#pragma once

#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <glad/glad.h>

#include "core/GlobalTypes.h"
#include "physics/BoundingBox.h"
#include "LuaBindings.h"
#include "LuaLogger.h"

class World;
class Lines;
class Points;

namespace Physics {
    class DynamicBBTree;
}

namespace SceneImporterInternal {
    class SceneHelper;
}

// Manages Lua state lifecycle and script callbacks
class LuaRuntime {
public:
    LuaRuntime();   // Defined in .cpp where SceneHelper is complete
    ~LuaRuntime();  // Defined in .cpp where SceneHelper is complete

    sol::state lua;
    Entity lightEntity = 999;
    Utils::LuaLogger luaLogger;
    std::unordered_map<std::string, Lines*> debugLines;
    std::unordered_map<std::string, Points*> debugPoints;
    std::unordered_map<Entity, BoundingBox> physicsRegistry;
    std::vector<std::unique_ptr<SceneImporterInternal::SceneHelper>> sceneHelpers;
    std::vector<std::unique_ptr<Lines>> ownedLines;
    std::vector<std::unique_ptr<Points>> ownedPoints;
    std::unordered_map<std::string, GLuint> shaderMap;

    // Initialize Lua state with all bindings
    void Initialize(World& world, Physics::DynamicBBTree& tree,
                   const std::unordered_map<std::string, GLuint>& shaders);

    // Load scene script and execute initialization
    bool LoadScene(const std::string& filename, std::string& outErrorMsg);

    // Load minimal fallback scene on error
    bool LoadFallbackScene(std::string& outErrorMsg);

    // Register debug renderables for Lua access
    void RegisterDebugLines(const std::string& name, Lines* lines);
    void RegisterDebugPoints(const std::string& name, Points* points);

    // Register entity bounding box for physics tree insertion
    void RegisterPhysics(Entity entity, BoundingBox box) { physicsRegistry[entity] = box; }

    // Invoke script callbacks (optional - won't crash if undefined)
    void CallOnInit();
    void CallOnUpdate(float dt, const LuaBindings::LuaInput& input,
                     const LuaBindings::LuaCameraView& camera);
    void CallOnClick(const LuaBindings::LuaInput& input,
                    const LuaBindings::LuaCameraView& camera);

    Entity GetLightEntity() const { return lightEntity; }

    sol::optional<Entity> GetSelectedEntity() const {
        return lua.get<sol::optional<Entity>>("SelectedEntity");
    }

    // Transfer ownership of Lines/Points created in Lua
    void TakeOwnership(std::unique_ptr<Lines> lines);
    void TakeOwnership(std::unique_ptr<Points> points);

private:
    bool callbacksRegistered = false;
    World* worldPtr = nullptr;
    Physics::DynamicBBTree* treePtr = nullptr;
};
