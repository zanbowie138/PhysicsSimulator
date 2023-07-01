#pragma once
#include "glm/glm.hpp"
namespace Components
{
	struct Rigidbody 
	{
	    float mass;
		glm::vec3 velocity;

		bool sleeping;
	};
}
