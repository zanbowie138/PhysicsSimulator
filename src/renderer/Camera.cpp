#include "Camera.h"

Camera::Camera(const int width, const int height, const glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
}

void Camera::UpdateMatrix(const float FOVdeg, const float nearPlane, const float farPlane)
{
	auto view = glm::mat4(1.0f);
	auto proj = glm::mat4(1.0f);

	view = lookAt(position, position + orientation, up);
	proj = glm::perspective(glm::radians(FOVdeg), static_cast<float>(width) / height, nearPlane, farPlane);

	cameraMatrix = proj * view;
}

void Camera::Matrix(const Shader& shader, const char* uniform) const
{
	glUniformMatrix4fv(shader.GetUniformLocation(uniform), 1, GL_FALSE, value_ptr(cameraMatrix));
}