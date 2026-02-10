// PCH automatically includes sol, glm, components, Ray, Lines, Points, etc.
#include <lua_engine/LuaBindings.h>

namespace LuaBindings {

void BindStableTypes(sol::state& lua) {
    LOG(LOG_INFO) << "Binding stable types\n";

    // POD types - rarely change
    lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    // UNUSED: vec2 not referenced in test.lua
    // lua.new_usertype<glm::vec2>("vec2",
    //     sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
    //     "x", &glm::vec2::x,
    //     "y", &glm::vec2::y
    // );

    // Component types - structure stable
    lua.new_usertype<Components::Transform>("Transform",
        sol::no_constructor,
        "worldPos", &Components::Transform::worldPos,
        "scale", &Components::Transform::scale
        // UNUSED: Rotation methods not called in test.lua
        // "SetRotationEuler", &Components::Transform::SetRotationEuler,
        // "GetEulerRotation", &Components::Transform::GetEulerRotation
    );

    // UNUSED: Rigidbody component not accessed from Lua in test.lua
    // lua.new_usertype<Components::Rigidbody>("Rigidbody",
    //     sol::no_constructor,
    //     "linearVelocity", &Components::Rigidbody::linearVelocity,
    //     "AddForce", &Components::Rigidbody::AddForce,
    //     "SetMass", &Components::Rigidbody::SetMass,
    //     "GetMass", &Components::Rigidbody::GetMass
    // );

    // UNUSED: RenderInfo component not accessed from Lua in test.lua
    // lua.new_usertype<Components::RenderInfo>("RenderInfo",
    //     sol::no_constructor,
    //     "color", &Components::RenderInfo::color,
    //     "enabled", &Components::RenderInfo::enabled
    // );

    // Physics types - actively used
    lua.new_usertype<Ray>("Ray",
        sol::constructors<Ray(const glm::vec3&, const glm::vec3&)>(),
        "origin", &Ray::origin,
        "direction", &Ray::direction,
        "GetPoint", &Ray::GetPoint
    );

    lua.new_usertype<BoundingBox>("BoundingBox",
        sol::constructors<BoundingBox(), BoundingBox(const glm::vec3, const glm::vec3)>(),
        "min", &BoundingBox::min,
        "max", &BoundingBox::max
    );

    // Debug renderables - actively used
    lua.new_usertype<Lines>("Lines",
        sol::no_constructor,
        "Clear", &Lines::Clear,
        "PushRay", sol::overload(
            static_cast<void(Lines::*)(const glm::vec3&, const glm::vec3&, float)>(&Lines::PushRay),
            static_cast<void(Lines::*)(const Ray&, float)>(&Lines::PushRay)
        ),
        "PushBoundingBox", &Lines::PushBoundingBox,
        "PushBoundingBoxes", &Lines::PushBoundingBoxes
    );

    // UNUSED: Points renderable never used in test.lua
    // lua.new_usertype<Points>("Points",
    //     sol::no_constructor,
    //     "Clear", &Points::Clear,
    //     "PushToBuffer", &Points::PushToBuffer
    // );

    // Input/Camera - read-only wrappers
    lua.new_usertype<LuaInput>("LuaInput",
        sol::no_constructor,
        "leftMouse", sol::readonly(&LuaInput::leftMouse),
        "rightMouse", sol::readonly(&LuaInput::rightMouse),
        "w", sol::readonly(&LuaInput::w),
        "a", sol::readonly(&LuaInput::a),
        "s", sol::readonly(&LuaInput::s),
        "d", sol::readonly(&LuaInput::d),
        "space", sol::readonly(&LuaInput::space),
        "ctrl", sol::readonly(&LuaInput::ctrl),
        "shift", sol::readonly(&LuaInput::shift),
        "mousePos", sol::readonly(&LuaInput::mousePos),
        "mousePosNormalized", sol::readonly(&LuaInput::mousePosNormalized)
    );

    lua.new_usertype<LuaCameraView>("LuaCameraView",
        sol::no_constructor,
        "position", sol::readonly(&LuaCameraView::position),
        "orientation", sol::readonly(&LuaCameraView::orientation),
        "cameraMatrix", sol::readonly(&LuaCameraView::cameraMatrix)
    );
}

} // namespace LuaBindings
