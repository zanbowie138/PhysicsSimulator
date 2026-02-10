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

    // Bind stable types (POD, components, physics primitives, debug renderables)
    // These rarely change - compile once, reuse across sessions
    void BindStableTypes(sol::state& lua);

    // Bind dynamic APIs (World methods, Utils functions, tree queries)
    // These frequently change - recompile often during development
    void BindDynamicAPIs(sol::state& lua, World& world, Physics::DynamicBBTree& tree,
                        const std::unordered_map<std::string, Lines*>& lines,
                        const std::unordered_map<std::string, Points*>& points);
}
