#pragma once
#include "glm/glm.hpp"

namespace Components
{
	struct Transform {
	    glm::vec3 worldPos;
	    glm::vec3 rotation = glm::vec3(1.0f);
	    glm::vec3 scale = glm::vec3(1.0f);

	    glm::mat4 modelMat;
	};
}
