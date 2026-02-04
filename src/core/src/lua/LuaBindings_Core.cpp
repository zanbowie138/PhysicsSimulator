#include <glad/glad.h>
#include "LuaBindings.h"

// Sol2 template instantiation requires complete type definitions
// for all types used in bound method signatures
#include "renderer/Camera.h"
#include "core/World.h"
#include "core/GlobalTypes.h"
#include "core/WindowManager.h"
#include "components/Components.h"
#include "utils/Exceptions.h"
#include "utils/Logger.h"

namespace LuaBindings {

void BindCore(sol::state& lua, World& world) {
    LOG(LOG_INFO) << "Binding core Lua types\n";

    // Bind glm::vec3
    // Usage: vec3(x, y, z) or vec3{x=1, y=2, z=3}
    auto vec3_type = lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    // Bind glm::vec2
    // Usage: vec2(x, y) or vec2{x=1, y=2}
    auto vec2_type = lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );

    // Bind Transform component
    // Usage: transform.worldPos = vec3(1, 2, 3)
    auto transform_type = lua.new_usertype<Components::Transform>("Transform",
        sol::no_constructor,
        "worldPos", &Components::Transform::worldPos,
        "scale", &Components::Transform::scale,
        "SetRotationEuler", &Components::Transform::SetRotationEuler,
        "GetEulerRotation", &Components::Transform::GetEulerRotation
    );

    // Bind Rigidbody component
    // Usage: rb.linearVelocity = vec3(0, 5, 0)
    auto rigidbody_type = lua.new_usertype<Components::Rigidbody>("Rigidbody",
        sol::no_constructor,
        "linearVelocity", &Components::Rigidbody::linearVelocity,
        "AddForce", &Components::Rigidbody::AddForce,
        "SetMass", &Components::Rigidbody::SetMass,
        "GetMass", &Components::Rigidbody::GetMass
    );

    // Bind RenderInfo component
    // Usage: renderInfo.color = vec3(1, 0, 0)
    auto renderInfo_type = lua.new_usertype<Components::RenderInfo>("RenderInfo",
        sol::no_constructor,
        "color", &Components::RenderInfo::color,
        "enabled", &Components::RenderInfo::enabled
    );

    // Create World table with methods
    // Usage: world:CreateEntity(), world:GetTransform(entity)
    lua["world"] = lua.create_table_with(
        "CreateEntity", [&world]() -> Entity {
            return world.CreateEntity();
        },

        "DestroyEntity", [&world](Entity entity) {
            world.DestroyEntity(entity);
        },

        "GetTransform", [&world](Entity entity) -> Components::Transform& {
            try {
                return world.GetComponent<Components::Transform>(entity);
            } catch (const ECSException& e) {
                throw std::runtime_error(std::string("GetTransform failed: ") + e.what());
            }
        },

        "GetRigidbody", [&world](Entity entity) -> Components::Rigidbody& {
            try {
                return world.GetComponent<Components::Rigidbody>(entity);
            } catch (const ECSException& e) {
                throw std::runtime_error(std::string("GetRigidbody failed: ") + e.what());
            }
        },

        "GetRenderInfo", [&world](Entity entity) -> Components::RenderInfo& {
            try {
                return world.GetComponent<Components::RenderInfo>(entity);
            } catch (const ECSException& e) {
                throw std::runtime_error(std::string("GetRenderInfo failed: ") + e.what());
            }
        },

        "HasTransform", [&world](Entity entity) -> bool {
            try {
                auto sig = world.GetEntitySignature(entity);
                auto compType = world.GetComponentType<Components::Transform>();
                return sig.test(compType);
            } catch (...) {
                return false;
            }
        },

        "HasRigidbody", [&world](Entity entity) -> bool {
            try {
                auto sig = world.GetEntitySignature(entity);
                auto compType = world.GetComponentType<Components::Rigidbody>();
                return sig.test(compType);
            } catch (...) {
                return false;
            }
        },

        "HasRenderInfo", [&world](Entity entity) -> bool {
            try {
                auto sig = world.GetEntitySignature(entity);
                auto compType = world.GetComponentType<Components::RenderInfo>();
                return sig.test(compType);
            } catch (...) {
                return false;
            }
        }
    );
}

LuaInput LuaInput::FromWindowManager(const Core::WindowManager& wm) {
    LuaInput input;
    input.leftMouse = wm.GetInputs().test(static_cast<size_t>(InputButtons::LEFT_MOUSE));
    input.rightMouse = wm.GetInputs().test(static_cast<size_t>(InputButtons::RIGHT_MOUSE));
    input.w = wm.GetInputs().test(static_cast<size_t>(InputButtons::W));
    input.a = wm.GetInputs().test(static_cast<size_t>(InputButtons::A));
    input.s = wm.GetInputs().test(static_cast<size_t>(InputButtons::S));
    input.d = wm.GetInputs().test(static_cast<size_t>(InputButtons::D));
    input.space = wm.GetInputs().test(static_cast<size_t>(InputButtons::SPACE));
    input.ctrl = wm.GetInputs().test(static_cast<size_t>(InputButtons::CONTROL));
    input.shift = wm.GetInputs().test(static_cast<size_t>(InputButtons::SHIFT));
    input.mousePos = wm.GetMousePos();
    input.mousePosNormalized = wm.GetMousePosNormalized();
    return input;
}

LuaCameraView LuaCameraView::FromCamera(const Camera& cam) {
    LuaCameraView view;
    view.position = cam.position;
    view.orientation = cam.orientation;
    view.cameraMatrix = cam.cameraMatrix;
    return view;
}

void BindInputAndCamera(sol::state& lua) {
    LOG(LOG_INFO) << "Binding input and camera types\n";

    // Bind LuaInput (read-only)
    // Usage: if input.leftMouse then ... end
    auto input_type = lua.new_usertype<LuaInput>("LuaInput",
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

    // Bind LuaCameraView (read-only)
    // Usage: ray = Utils.ScreenPointToRay(input.mousePosNormalized, camera.cameraMatrix)
    auto camera_type = lua.new_usertype<LuaCameraView>("LuaCameraView",
        sol::no_constructor,
        "position", sol::readonly(&LuaCameraView::position),
        "orientation", sol::readonly(&LuaCameraView::orientation),
        "cameraMatrix", sol::readonly(&LuaCameraView::cameraMatrix)
    );
}

} // namespace LuaBindings
