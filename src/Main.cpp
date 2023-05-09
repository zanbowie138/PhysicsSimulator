#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#include "core/ECS.h"
#include "components/Components.h"
#include "core/UniformBufferManager.h"
#include "renderer/RenderSystem.h"
#include "renderables/ChessModel.h"
#include "core/WindowManager.h"
#include "renderables/Model.h"

ECSController ecsController;

int main()
{
	// Window creation
	Core::WindowManager windowManager;
	windowManager.Init("OpenGL Window", 800, 800);
	GLFWwindow* window = windowManager.GetWindow();

	// Camera creation
	Camera cam{800,800, glm::vec3(0.0f, 1.0f, 7.0f)};

	// Initialize ECS
	ecsController.Init();

	// Register components
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();

	// Create RenderSystem and add dependencies
	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());
	renderSystem->SetCamera(&cam);

	// Set RenderSystem signature
	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	// Create entity
	auto entity = ecsController.CreateEntity();
	auto entity2 = ecsController.CreateEntity();
	auto entity3 = ecsController.CreateEntity();

	Shader flatShader("flat.vert", "flat.frag");

	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));
	ChessModel piece1(pawn, glm::vec3(3.0f, 0.0f, 0.0f));
	Model bunny("bunny.dat", false);

	// Add components to entity
	ecsController.AddComponent(entity, Components::Transform());
	ecsController.AddComponent(entity2, Components::Transform{glm::vec3(1.0f, 0.0f, 0.0f)});
	ecsController.AddComponent(entity3, Components::Transform{ glm::vec3(2.0f, 0.0f, 0.0f) });

	ecsController.AddComponent(entity, Components::RenderInfo{piece.VAO.ID, flatShader.ID, piece.indices.size()});
	ecsController.AddComponent(entity2, Components::RenderInfo{piece1.VAO.ID, flatShader.ID, piece1.indices.size()});
	ecsController.AddComponent(entity3, Components::RenderInfo{ bunny.VAO.ID, flatShader.ID, bunny.indices.size() });

	// Manage Uniform Buffer
	Core::UniformBufferManager UBO;
	UBO.SetCamera(&cam);
	// Allocate buffer in OpenGL
	UBO.Allocate();
	// 
	UBO.InitBind();

	// TODO: Make this applicable to all shaders
	// Maybe create some sort of bitset with dependencies for uniform blocks
	// Could have these dependencies in actual shader file, shader class reads these and updates accordingly
	UBO.BindShader(flatShader);

	while (!glfwWindowShouldClose(window))
	{
		windowManager.ProcessInputs();
		cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos());
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
		UBO.UpdateData();

		renderSystem->PreUpdate();
		renderSystem->Update();
		renderSystem->PostUpdate();
	}

	ecsController.Clean();
	// TODO: Add cleaning for OpenGL objects

	windowManager.Shutdown();

	return 0;
}
