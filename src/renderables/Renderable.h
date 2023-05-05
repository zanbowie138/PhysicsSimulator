#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

class Renderable
{
public:
	virtual ~Renderable() = default;
	float scale = 1;
	glm::vec3 worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec4 color = glm::vec4(1.0f);

	virtual void Draw(const Shader& shader) const = 0;

	void UpdateModelMat();

	VAO VAO;

protected:
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};

inline void Renderable::UpdateModelMat()
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), worldPos);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}
