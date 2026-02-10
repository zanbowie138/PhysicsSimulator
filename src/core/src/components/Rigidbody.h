#pragma once
#include "Collider.h"
namespace Components
{
	struct Rigidbody 
	{
	    float inverseMass = 1.0f/100.0f;

		glm::vec3 centroid;
		glm::mat3 orientation;

		// TODO: Use transform position
		glm::vec3 position;

	    glm::vec3 linearVelocity;
		glm::vec3 angularVelocity;

		glm::vec3 forceAccumulator;
		glm::vec3 torqueAccumulator;

		Collider* m_collider;

	    bool sleeping;

		void SetMass(float mass)
		{
			inverseMass = 1 / mass;
		}
			
		float GetMass()
		{
			return 1 / inverseMass;
		}

		void ClearAccumulator()
		{
			forceAccumulator = glm::vec3(0.0f);
		}

		void AddForce(const glm::vec3& force)
		{
			forceAccumulator += force;
		}
	};
}
