#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BoundingBoxTree.h"

#include "../core/ECS.h"
#include "../core/System.h"
#include "../core/ComponentManager.h"

class PhysicsSystem : public System
{
public:
    BoundingBoxTree tree;

    explicit PhysicsSystem();
    ~PhysicsSystem() = default;

    void Update();

    void Clean() override;
private:
    void ResolveCollisions();
};

inline PhysicsSystem::PhysicsSystem()
{
    tree = BoundingBoxTree(1);
}

inline void PhysicsSystem::Clean()
{

}

