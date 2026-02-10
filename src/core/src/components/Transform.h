#pragma once
#include "glm/gtx/quaternion.hpp"


// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
namespace Components
{
	struct Transform {
		glm::vec3 worldPos = glm::vec3(0.0f);
		glm::quat rotation = glm::identity<glm::quat>();
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 modelMat;

		void CalculateModelMat()
		{
			const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), worldPos);
			const glm::mat4 rotationMatrix = glm::toMat4(rotation);
			const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
			modelMat = translationMatrix * rotationMatrix * scaleMatrix;
		}

		// Returns a mat4 of the linear transformation of rotation and scale
		// Doesn't include worldPos
		// Useful if multiple transforms need o
		glm::mat4 GetLinearTransform() const
		{
			return glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
		}

		glm::vec3 GetEulerRotation() const
		{
			return glm::eulerAngles(rotation) * 3.14159f / 180.f;
		}

		void SetRotationEuler(glm::vec3 eulerRot)
		{
			rotation = glm::quat(glm::radians(eulerRot));
		}
	};
}
