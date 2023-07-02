#pragma once
#include "glm/glm.hpp"
namespace Components
{
	struct Rigidbody 
	{
	    float inverseMass = 1.0f/100.0f;

		// TODO: Use transform position
		glm::vec3 position;
	    glm::vec3 velocity;

		glm::vec3 forceAccum;

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
			forceAccum = glm::vec3(0.0f);
		}

		void AddForce(const glm::vec3& force)
		{
			forceAccum += force;
		}
	};
}
