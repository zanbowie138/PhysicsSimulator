#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

namespace Components
{
	struct Transform {
	    glm::vec3 worldPos = glm::vec3(0.0f);
	    glm::vec3 rotation = glm::vec3(0.0f);
	    glm::vec3 scale = glm::vec3(1.0f);

	    glm::mat4 modelMat;

		void CalculateModelMat() 
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), worldPos);
			glm::mat4 rotationMatrix = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
			modelMat = translationMatrix * rotationMatrix * scaleMatrix;
		}
	};
}
