#pragma once
// Input
enum class InputButtons
{
	MOUSE,
	W,
	A,
	S,
	D,
	SPACE,
	CONTROL,
	SHIFT,
	ENUM_LENGTH
};

// Global Uniforms
struct UniformBlock
{
	glm::mat4 camMatrix;  // 0 // 64

	glm::vec4 camPos;	  // 64 // 16
	glm::vec4 lightPos;	  //    // 16
	glm::vec4 lightColor; // 112 // 16
};