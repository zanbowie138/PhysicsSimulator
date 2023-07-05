#pragma once
#include "glm/glm.hpp"

namespace Components
{
	struct Collider
	{
		glm::mat3 m_inertiaTensor;
	};
}