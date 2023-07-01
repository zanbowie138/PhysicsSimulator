#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"
#include "../core/GlobalTypes.h"

class Camera
{
public:
	glm::vec3 position{};
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	unsigned int width;
	unsigned int height;

	float baseSpeed = 0.001f;
	float speed = 0.0001f;
	float sensitivity = 100.0f;

	Camera(unsigned int width, unsigned int height, glm::vec3 position);

	void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);

	void MoveCam(const InputBitset& buttons, const glm::vec2& mousePos, const float dt);

	//Sends camera matrix to inputted shader to update position
	void Matrix(const Shader& shader, const char* uniform) const;
};

inline Camera::Camera(const unsigned int width, const unsigned int height, const glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
}

inline void Camera::UpdateMatrix(const float FOVdeg, const float nearPlane, const float farPlane)
{
	auto view = glm::mat4(1.0f);
	auto proj = glm::mat4(1.0f);

	view = lookAt(position, position + orientation, Constants::UP);
	proj = glm::perspective(glm::radians(FOVdeg), static_cast<float>(width) / height, nearPlane, farPlane);

	cameraMatrix = proj * view;
}

inline void Camera::MoveCam(const InputBitset& buttons, const glm::vec2& mousePos, const float dt)
{
	if (buttons.test((static_cast<std::size_t>(InputButtons::W))))
		position += speed * orientation * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::A))))
		position += speed * -normalize(cross(orientation, Constants::UP)) * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::S))))
		position += speed * -orientation * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::D))))
		position += speed * normalize(cross(orientation, Constants::UP)) * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::SPACE))))
		position += speed * Constants::UP * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::CONTROL))))
		position += speed * -Constants::UP * dt;
	if (buttons.test((static_cast<std::size_t>(InputButtons::SHIFT))))
		speed = baseSpeed * 5;
	else
		speed = baseSpeed;

	if (buttons.test((static_cast<std::size_t>(InputButtons::RIGHT_MOUSE))))
	{
		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		const float rotX = sensitivity * static_cast<float>(mousePos.y - (height / 2)) / height;
		const float rotY = sensitivity * static_cast<float>(mousePos.x - (width / 2)) / width;

		// Calculates upcoming vertical change in the cam.orientation
		const glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, Constants::UP)));


		// Decides whether or not the next vertical cam.orientation is legal or not
		if (abs(angle(newOrientation, Constants::UP) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Rotates the cam.orientation left and right
		orientation = rotate(orientation, glm::radians(-rotY), Constants::UP);
	}
}
