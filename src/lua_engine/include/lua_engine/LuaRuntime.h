#pragma once

#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "core/GlobalTypes.h"
#include "../../../core/src/math/BoundingBox.h"
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

struct CameraConfig {
    glm::vec3 position    = {0.0f, 0.0f, 0.0f};
    glm::vec3 orientation = {0.0f, 0.0f, -1.0f};
    float fov       = 45.0f;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;
};

// Manages Lua state lifecycle and script callbacks
class LuaRuntime {
public:
    LuaRuntime();   // Defined in .cpp where SceneHelper is complete
    ~LuaRuntime();  // Defined in .cpp where SceneHelper is complete

    sol::state lua;
    Entity lightEntity = 999;
    float simTime = 0.0f;
    CameraConfig cameraConfig;
    bool cameraMovementEnabled = true;
    Utils::LuaLogger luaLogger;
    std::unordered_map<std::string, Lines*> debugLines;
    std::unordered_map<std::string, Points*> debugPoints;
    std::unordered_map<Entity, BoundingBox> physicsRegistry;
    std::vector<std::unique_ptr<SceneImporterInternal::SceneHelper>> sceneHelpers;
    std::vector<std::unique_ptr<Lines>> ownedLines;
    std::vector<std::unique_ptr<Points>> ownedPoints;
    std::unordered_map<std::string, GLuint> shaderMap;

    // Initialize Lua state with all bindings. Idempotent — safe to call again
    // to fully rebuild the sol::state (drops all globals/callbacks).
    bool Initialize(World& world, Physics::DynamicBBTree& tree,
                   const std::unordered_map<std::string, GLuint>& shaders,
                   std::string& outErrorMsg);

    // Tear down + rebuild Lua state using cached world/tree/shaders from the
    // previous Initialize call. Used by reload paths.
    bool Reinitialize(std::string& outErrorMsg);

    // Clear runtime state between scene loads (tree, registry, owned renderables)
    void Reset();

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
    void CallOnGUI();

    Entity GetLightEntity() const { return lightEntity; }
    const CameraConfig& GetCameraConfig() const { return cameraConfig; }
    bool IsCameraMovementEnabled() const { return cameraMovementEnabled; }

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
