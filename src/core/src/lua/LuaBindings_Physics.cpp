#include <glad/glad.h>
#include "LuaBindings.h"
#include "utils/Logger.h"

// Sol2 template instantiation requires complete type definitions
// for all types used in bound method signatures
#include "math/Ray.h"
#include "physics/BoundingBox.h"
#include "physics/DynamicTree.h"
#include "utils/Raycast.h"

namespace LuaBindings {

void BindPhysics(sol::state& lua, Physics::DynamicBBTree& tree) {
    LOG(LOG_INFO) << "Binding physics Lua types\n";

    // Bind Ray
    // Usage: ray = Ray(vec3(0, 0, 0), vec3(0, 0, -1))
    auto ray_type = lua.new_usertype<Ray>("Ray",
        sol::constructors<Ray(const glm::vec3&, const glm::vec3&)>(),
        "origin", &Ray::origin,
        "direction", &Ray::direction,
        "GetPoint", &Ray::GetPoint
    );

    // Bind BoundingBox
    // Usage: box = BoundingBox(vec3(-1, -1, -1), vec3(1, 1, 1))
    auto box_type = lua.new_usertype<BoundingBox>("BoundingBox",
        sol::constructors<BoundingBox(), BoundingBox(const glm::vec3, const glm::vec3)>(),
        "min", &BoundingBox::min,
        "max", &BoundingBox::max
    );

    // Bind DynamicBBTree
    // Usage: entity, hit = tree:QueryRay(ray)
    auto tree_type = lua.new_usertype<Physics::DynamicBBTree>("DynamicBBTree",
        sol::no_constructor,
        "QueryRay", [](Physics::DynamicBBTree& tree, const Ray& ray) -> std::tuple<Entity, bool> {
            auto [entity, hit] = tree.QueryRay(ray);
            return std::make_tuple(entity, hit);
        },
        "QueryRayCollisions", [](Physics::DynamicBBTree& tree, const Ray& ray) -> std::tuple<std::vector<BoundingBox>, bool> {
            auto [boxes, hit] = tree.QueryRayCollisions(ray);
            return std::make_tuple(boxes, hit);
        },
        "GetBoundingBox", &Physics::DynamicBBTree::GetBoundingBox,
        "GetAllBoxes", &Physics::DynamicBBTree::GetAllBoxes,
        "ComputeCollisionPairs", &Physics::DynamicBBTree::ComputeCollisionPairs
    );

    // Create PhysicsSystem table
    // Usage: entity, hit = PhysicsSystem.tree:QueryRay(ray)
    lua["PhysicsSystem"] = lua.create_table_with(
        "tree", std::ref(tree)
    );

    // Create Utils namespace
    // Usage: ray = Utils.ScreenPointToRay(mouseNorm, cameraMatrix)
    lua["Utils"] = lua.create_table_with(
        "ScreenPointToRay", [](const glm::vec2& uv, const glm::mat4& camMatrix) {
            return Utils::ScreenPointToRay(uv, camMatrix);
        }
    );
}

} // namespace LuaBindings
