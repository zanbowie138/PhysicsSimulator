#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <iostream>
#include <vector>


#include "Camera.h"
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/ChessModel.h"
#include "renderables/Points.h"
#include "renderables/Lines.h"
#include "shaderClass.h"
#include "UBO.h"
#include "EntityManager.h"
#include "ComponentManager.h"

struct UniformBlock
{
	glm::mat4 camMatrix;  // 0 // 64

	glm::vec4 camPos;	  // 64 // 16
	glm::vec4 lightPos;	  //    // 16
	glm::vec4 lightColor; // 112 // 16
};

class SceneController
{
public:
	Camera cam;
	GLFWwindow* window;
	UBO UBO{};
	bool mouseShown;

	std::vector<std::unique_ptr<Renderable>> objects;
	std::vector<std::unique_ptr<Shader>> shaders;

	SceneController(GLFWwindow* window, const GLuint screenWidth, const GLuint screenHeight);

	void DrawScene(const unsigned frame);
	void HandleInputs(bool mouseActive);
	void Clean() const;
};
