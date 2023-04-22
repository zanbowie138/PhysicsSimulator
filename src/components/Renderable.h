#pragma once

#include "VAO.h"
#include "shaderClass.h"
#include "Camera.h"

class Renderable
{
public:
	virtual ~Renderable() = default;
	float scale = 1;
	glm::vec3 worldPos = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 rotation = glm::vec3(0.0, 0.0, 0.0);

	virtual void Draw(const Shader& shader, const Camera& camera) const = 0;

	void UpdateModelMat();

protected:
	VAO VAO;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};

inline void Renderable::UpdateModelMat()
{
	const auto size = glm::mat4(scale);
	const glm::mat4 translated = translate(size, worldPos);
	modelMatrix = rotate(
		rotate(rotate(translated, glm::radians(rotation.x), glm::vec3(1, 0, 0)), glm::radians(rotation.y),
		       glm::vec3(0, 1, 0)), glm::radians(rotation.z), glm::vec3(0, 0, 1));
}
