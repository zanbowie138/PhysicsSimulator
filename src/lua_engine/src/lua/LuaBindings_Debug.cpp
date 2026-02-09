#include <glad/glad.h>
#include <lua_engine/LuaBindings.h>
#include "utils/Logger.h"

// Sol2 template instantiation requires complete type definitions
// for all types used in bound method signatures
#include "math/Ray.h"
#include "renderables/Lines.h"
#include "renderables/Points.h"
#include <unordered_map>

namespace LuaBindings {

void BindDebugRendering(sol::state& lua,
                       const std::unordered_map<std::string, Lines*>& lines,
                       const std::unordered_map<std::string, Points*>& points) {
    LOG(LOG_INFO) << "Binding debug rendering types\n";

    // Bind Lines
    // Usage: lines:Clear(), lines:PushRay(ray, 10), lines:PushBoundingBox(box)
    auto lines_type = lua.new_usertype<Lines>("Lines",
        sol::no_constructor,
        "Clear", &Lines::Clear,
        "PushRay", sol::overload(
            static_cast<void(Lines::*)(const glm::vec3&, const glm::vec3&, float)>(&Lines::PushRay),
            static_cast<void(Lines::*)(const Ray&, float)>(&Lines::PushRay)
        ),
        "PushBoundingBox", &Lines::PushBoundingBox,
        "PushBoundingBoxes", &Lines::PushBoundingBoxes
    );

    // Bind Points
    // Usage: points:Clear(), points:PushToBuffer(vec3Array)
    auto points_type = lua.new_usertype<Points>("Points",
        sol::no_constructor,
        "Clear", &Points::Clear,
        "PushToBuffer", &Points::PushToBuffer
    );

    // Create Debug namespace
    // Usage: debugLines = Debug.GetLines("rays")
    sol::table debugTable = lua.create_table();

    debugTable["GetLines"] = [&lines](const std::string& name) -> Lines* {
        auto it = lines.find(name);
        if (it != lines.end()) {
            return it->second;
        }
        throw std::runtime_error("Debug lines '" + name + "' not found");
    };

    debugTable["GetPoints"] = [&points](const std::string& name) -> Points* {
        auto it = points.find(name);
        if (it != points.end()) {
            return it->second;
        }
        throw std::runtime_error("Debug points '" + name + "' not found");
    };

    lua["Debug"] = debugTable;
}

} // namespace LuaBindings
