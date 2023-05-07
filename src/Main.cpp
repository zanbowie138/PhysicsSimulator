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

	Shader flatShader("flat.vert", "flat.frag");

	// TODO: Remove renderables, just have model loading
	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));

	// Add components to entity
	ecsController.AddComponent(entity, Components::Transform());
	// TODO: Add type of draw to RenderInfo(etc. Draw elements, points...)
	ecsController.AddComponent(entity, Components::RenderInfo{piece.VAO.ID, flatShader.ID, piece.indices.size()});

	// Manage Uniform Buffer
	// TODO: Inputs
	Core::UniformBufferManager UBO;
	UBO.SetCamera(&cam);
	UBO.Allocate();
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

	// TODO: Better cleaning system, add clean into system parent class as a virtual function?
	renderSystem->Clean();

	windowManager.Shutdown();

	return 0;
}
