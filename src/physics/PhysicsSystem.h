#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DynamicTree.h"

#include "../core/ECS.h"
#include "../core/System.h"
#include "../core/ComponentManager.h"

#define GRAVITY -9.81

// http://graphics.stanford.edu/papers/rigid_bodies-sig03/
class PhysicsSystem : public System
{
public:
    Physics::DynamicBBTree tree;

    explicit PhysicsSystem();
    ~PhysicsSystem() = default;

    void Update();

    void Clean() override;
private:
    /*
     * Process collision.
     *  Compute candidate positions of bodies.
			For each intersecting pair of bodies:
				Determine interpenetrating points.
				Sort points by penetration depth (deepest first). 
				For each point in order:
					Apply frictional impulse (unless bodies receding).
				Repeat above a number of times.
					Resolving one collision might create new ones.
					Applies series of impulses rather than simultaneously resolving all collisions.
		Update velocity.
		Process contact.
			Determine contacts and prevent penetration.
		Update position
     */
    void ResolveCollisions();
};

inline PhysicsSystem::PhysicsSystem()
{
    tree = Physics::DynamicBBTree{ 1 };
}

inline void PhysicsSystem::Clean()
{

}

