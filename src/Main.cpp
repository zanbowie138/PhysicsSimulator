#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "core/ECS.h"
#include "components/Components.h"
#include "renderer/RenderSystem.h"

constexpr unsigned int screen_width = 800;
constexpr unsigned int screen_height = 800;

constexpr unsigned int aliasing_samples = 2;

int main()
{
	ECSController ecsController;
	ecsController.Init();
	ecsController.RegisterComponent<Components::Transform>();

	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();

	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	std::vector<Entity> entities;
	entities.emplace_back(ecsController.CreateEntity());


	while (!glfwWindowShouldClose(window))
	{
		
	}

	renderer.Clean();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}
