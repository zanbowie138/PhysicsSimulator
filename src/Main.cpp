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
	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));

	// Add components to entity
	ecsController.AddComponent(entity, Components::Transform());
	ecsController.AddComponent(entity, Components::RenderInfo{piece.VAO.ID, flatShader.ID, piece.indices.size()});

	// Manage Uniform Buffer
	// TODO: Shader dependencies??
	Core::UniformBufferManager UBO;
	UBO.SetCamera(&cam);
	UBO.Allocate();
	UBO.InitBind();
	UBO.BindShader(flatShader);

	while (!glfwWindowShouldClose(window))
	{
		UBO.UpdateData();

		renderSystem->PreUpdate(0.0f);
		renderSystem->Update(0.0f);
		renderSystem->PostUpdate(0.0f);
	}

	renderSystem->Clean();

	windowManager.Shutdown();

	return 0;
}
