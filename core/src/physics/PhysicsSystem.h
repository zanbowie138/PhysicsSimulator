#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DynamicTree.h"

#include "../core/ECS/ECSController.h"

#include "../renderables/Mesh.h"
#include "../renderables/Model.h"

#define GRAVITY -9.81

// http://graphics.stanford.edu/papers/rigid_bodies-sig03/
class PhysicsSystem : public System
{
public:
    Physics::DynamicBBTree tree;

    explicit PhysicsSystem();

	void AddRigidbody(Mesh& object);
	void AddRigidbody(Model& object);

	void AddToTree(Mesh& object);
	void AddToTree(Model& object);

    void Update(float dt);

    void Clean() override;
private:
    /*
     *	Process collision.
	       Compute candidate positions of bodies.
				For each intersecting pair of bodies:
					Determine interpenetrating points.
					Sort points by penetration depth (deepest first). 
					For each point in order:
						Apply frictional impulse (unless bodies receding).
					Repeat above a number of times.
						Resolving one collision might create new ones.
						Applies series of impulses rather than simultaneously resolving all collisions.
		Update linearVelocity.
		Process contact.
			Determine contacts and prevent penetration.
		Update position
     */
    void ResolveCollisions();

	// Iterates through all rigidbodies updating position and linearVelocity based on dt
	void Integrate(float dt);
};

inline PhysicsSystem::PhysicsSystem()
{
    tree = Physics::DynamicBBTree{ 1 };
}

inline void PhysicsSystem::AddRigidbody(Mesh& object)
{
	Components::Rigidbody newRb{};
	newRb.position = object.transform.worldPos;

	ecsController.AddComponent(object.mEntityID, newRb);
	AddToTree(object);
}

inline void PhysicsSystem::AddRigidbody(Model& object)
{
	Components::Rigidbody newRb{};
	newRb.position = object.transform.worldPos;

	ecsController.AddComponent(object.mEntityID, newRb);
	AddToTree(object);
}

inline void PhysicsSystem::AddToTree(Mesh& object)
{
	tree.InsertEntity(object.mEntityID, object.CalcBoundingBox());
}

inline void PhysicsSystem::AddToTree(Model& object)
{
	tree.InsertEntity(object.mEntityID, object.CalcBoundingBox());
}

inline void PhysicsSystem::Update(float dt)
{
	Integrate(dt);
}

inline void PhysicsSystem::Clean()
{

}

inline void PhysicsSystem::ResolveCollisions()
{
	const auto broadCollisions = tree.ComputeCollisionPairs();
}

inline void PhysicsSystem::Integrate(float dt)
{
	for (const auto entity : mEntities)
	{
		auto& rb = ecsController.GetComponent<Components::Rigidbody>(entity);

		glm::vec3 posOld = rb.position;
		rb.position += rb.linearVelocity * dt;

		glm::vec3 acceleration = rb.forceAccumulator * rb.inverseMass;
		acceleration += glm::vec3(0, GRAVITY, 0);

		rb.linearVelocity += acceleration * dt;
		rb.linearVelocity *= pow(0.9,dt);

		rb.ClearAccumulator();

		auto& transform = ecsController.GetComponent<Components::Transform>(entity);
		transform.worldPos = rb.position;
		tree.UpdateEntity(entity, rb.position - posOld);
	}
}
