#include <lua_engine/LuaBindings.h>
#include <lua_engine/LuaLogger.h>

namespace LuaBindings {

// Helper conversion functions
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

void BindDynamicAPIs(sol::state& lua, World& world, Physics::DynamicBBTree& tree,
                     const std::unordered_map<std::string, Lines*>& lines,
                     const std::unordered_map<std::string, Points*>& points,
                     Utils::LuaLogger& luaLogger,
                     const std::unordered_map<Entity, BoundingBox>& physicsRegistry) {
    LOG(LOG_INFO) << "Binding dynamic APIs\n";

    // World API - frequently extended
    sol::table worldTable = lua.create_table();

    worldTable["CreateEntity"] = [&world]() -> Entity {
        return world.CreateEntity();
    };

    // UNUSED: DestroyEntity not called in test.lua
    // worldTable["DestroyEntity"] = [&world](Entity entity) {
    //     world.DestroyEntity(entity);
    // };

    worldTable["GetTransform"] = [&world](Entity entity) -> Components::Transform& {
        try {
            return world.GetComponent<Components::Transform>(entity);
        } catch (const ECSException& e) {
            throw std::runtime_error(std::string("GetTransform failed: ") + e.what());
        }
    };

    worldTable["HasTransform"] = [&world](Entity entity) -> bool {
        try {
            auto sig = world.GetEntitySignature(entity);
            auto compType = world.GetComponentType<Components::Transform>();
            return sig.test(compType);
        } catch (...) {
            return false;
        }
    };

    // UNUSED: Rigidbody accessors not called in test.lua
    // worldTable["GetRigidbody"] = [&world](Entity entity) -> Components::Rigidbody& {
    //     try {
    //         return world.GetComponent<Components::Rigidbody>(entity);
    //     } catch (const ECSException& e) {
    //         throw std::runtime_error(std::string("GetRigidbody failed: ") + e.what());
    //     }
    // };
    //
    // worldTable["HasRigidbody"] = [&world](Entity entity) -> bool {
    //     try {
    //         auto sig = world.GetEntitySignature(entity);
    //         auto compType = world.GetComponentType<Components::Rigidbody>();
    //         return sig.test(compType);
    //     } catch (...) {
    //         return false;
    //     }
    // };

    // UNUSED: RenderInfo accessors not called in test.lua
    // worldTable["GetRenderInfo"] = [&world](Entity entity) -> Components::RenderInfo& {
    //     try {
    //         return world.GetComponent<Components::RenderInfo>(entity);
    //     } catch (const ECSException& e) {
    //         throw std::runtime_error(std::string("GetRenderInfo failed: ") + e.what());
    //     }
    // };
    //
    // worldTable["HasRenderInfo"] = [&world](Entity entity) -> bool {
    //     try {
    //         auto sig = world.GetEntitySignature(entity);
    //         auto compType = world.GetComponentType<Components::RenderInfo>();
    //         return sig.test(compType);
    //     } catch (...) {
    //         return false;
    //     }
    // };

    lua["world"] = worldTable;

    // Utils namespace - frequently extended
    sol::table utilsTable = lua.create_table();

    utilsTable["Log"] = [&luaLogger](const std::string& message, sol::optional<std::string> level) {
        std::string lvl = level.value_or("INFO");
        if (lvl == "ERROR")        luaLogger.Log(message, Utils::LogLevel::ERROR);
        else if (lvl == "WARNING") luaLogger.Log(message, Utils::LogLevel::WARNING);
        else                       luaLogger.Log(message, Utils::LogLevel::INFO);
    };

    lua.set_function("print", [&luaLogger, &lua](sol::variadic_args args) {
        std::string message;
        sol::function tostr = lua["tostring"];
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) message += "\t";
            message += tostr(args[i]).get<std::string>();
        }
        luaLogger.Log(message, Utils::LogLevel::INFO);
    });

    utilsTable["Lerp"] = [](float a, float b, float t) -> float {
        return a + (b - a) * glm::clamp(t, 0.0f, 1.0f);
    };

    utilsTable["Clamp"] = [](float value, float min, float max) -> float {
        return glm::clamp(value, min, max);
    };

    utilsTable["GetTime"] = []() -> float {
        return static_cast<float>(glfwGetTime() * 1000.0);
    };

    utilsTable["ScreenPointToRay"] = [](const glm::vec2& uv, const glm::mat4& camMatrix) {
        return Utils::ScreenPointToRay(uv, camMatrix);
    };

    lua["Utils"] = utilsTable;

    // DynamicBBTree methods - physics queries
    lua.new_usertype<Physics::DynamicBBTree>("DynamicBBTree",
        sol::no_constructor,
        "QueryRay", [](Physics::DynamicBBTree& tree, const Ray& ray) -> std::tuple<Entity, bool> {
            LOG(LOG_INFO) << "Query TREE!" << "\n";
            auto [entity, hit] = tree.QueryRay(ray);
            return std::make_tuple(entity, hit);
        },
        "QueryRayCollisions", [](Physics::DynamicBBTree& tree, const Ray& ray) -> std::tuple<std::vector<BoundingBox>, bool> {
            auto [boxes, hit] = tree.QueryRayCollisions(ray);
            return std::make_tuple(boxes, hit);
        },
        "GetBoundingBox", &Physics::DynamicBBTree::GetBoundingBox,
        "GetAllBoxes", [](Physics::DynamicBBTree& tree, bool onlyLeaf) -> std::vector<BoundingBox> {
            return tree.GetAllBoxes(onlyLeaf);
        },
        // "GetAllBoxes", &Physics::DynamicBBTree::GetAllBoxes,
        // "ComputeCollisionPairs", &Physics::DynamicBBTree::ComputeCollisionPairs
        "InsertEntity", &Physics::DynamicBBTree::InsertEntity,
        "RemoveEntity", &Physics::DynamicBBTree::RemoveEntity,
        "UpdateEntity", sol::overload(
            static_cast<void(Physics::DynamicBBTree::*)(Entity, BoundingBox)>(&Physics::DynamicBBTree::UpdateEntity),
            static_cast<void(Physics::DynamicBBTree::*)(Entity, glm::vec3)>(&Physics::DynamicBBTree::UpdateEntity)
        ),
        "AddToTree", [&physicsRegistry](Physics::DynamicBBTree& tree, Entity entity) {
            auto it = physicsRegistry.find(entity);
            if (it == physicsRegistry.end())
                throw std::runtime_error("Entity " + std::to_string(entity) + " has no registered bounding box");
            tree.InsertEntity(entity, it->second);
        }
    );

    // Expose tree via PhysicsSystem namespace
    lua["PhysicsSystem"] = lua.create_table_with(
        "tree", std::ref(tree)
    );

    // Debug namespace - access registered renderables
    sol::table debugTable = lua.create_table();

    debugTable["GetLines"] = [&lines](const std::string& name) -> Lines* {
        auto it = lines.find(name);
        if (it != lines.end()) {
            return it->second;
        }
        throw std::runtime_error("Debug lines '" + name + "' not found");
    };

    // UNUSED: GetPoints not called in test.lua
    // debugTable["GetPoints"] = [&points](const std::string& name) -> Points* {
    //     auto it = points.find(name);
    //     if (it != points.end()) {
    //         return it->second;
    //     }
    //     throw std::runtime_error("Debug points '" + name + "' not found");
    // };

    lua["Debug"] = debugTable;
}

} // namespace LuaBindings
