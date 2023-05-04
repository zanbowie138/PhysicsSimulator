#pragma once
#include "glm/glm.hpp"

namespace Components
{
	struct Transform {
	    glm::vec3 worldPos;
	    glm::vec3 rotation = glm::vec3(1.0f);
	    glm::vec3 scale = glm::vec3(1.0f);

	    glm::mat4 modelMat;

		static CalculateModelMat() 
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), worldPos);
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
			modelMat = translationMatrix * rotationMatrix * scaleMatrix;
		}
	};
}
