#pragma once

#include <sol/sol.hpp>
#include <glm/glm.hpp>

class World;

namespace Core {
    class WindowManager;
}

namespace Physics {
    class DynamicBBTree;
}

class Lines;
class Points;
class Camera;

namespace LuaBindings {
    // Wrapper for input state (read-only from Lua)
    struct LuaInput {
        bool leftMouse, rightMouse, w, a, s, d, space, ctrl, shift;
        glm::vec2 mousePos, mousePosNormalized;

        static LuaInput FromWindowManager(const Core::WindowManager& wm);
    };

    // Wrapper for camera state (read-only from Lua)
    struct LuaCameraView {
        glm::vec3 position, orientation;
        glm::mat4 cameraMatrix;

        static LuaCameraView FromCamera(const Camera& cam);
    };

    // Bind core types and World API to Lua state
    // Usage: Call once during initialization before loading scripts
    void BindCore(sol::state& lua, World& world);

    // Bind physics types (Ray, BoundingBox, DynamicBBTree) and raycast utilities
    // Usage: Call after BindCore, pass physics system tree
    void BindPhysics(sol::state& lua, Physics::DynamicBBTree& tree);

    // Bind debug rendering types (Lines, Points) via Debug namespace
    // Usage: Call after BindCore, register renderables with names
    void BindDebugRendering(sol::state& lua,
                           const std::unordered_map<std::string, Lines*>& lines,
                           const std::unordered_map<std::string, Points*>& points);

    // Bind input and camera wrapper structs
    // Usage: Call after BindCore
    void BindInputAndCamera(sol::state& lua);
}
