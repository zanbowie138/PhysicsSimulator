#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "core/ECS.h"
#include "components/Components.h"
#include "renderer/RenderSystem.h"
#include "renderables/ChessModel.h"
#include "core/WindowManager.h"


int main()
{
	Core::WindowManager windowManager;
	windowManager.Init("OpenGL Window", 800, 800);

	ECSController ecsController;
	ecsController.Init();
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();

	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();

	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	auto entity = ecsController.CreateEntity();

	Shader flatShader("flat.vert", "flat.frag");
	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));

	// TODO: MODEL MATRIX

	ecsController.AddComponent(entity, Components::Transform());
	ecsController.AddComponent(entity, Components::RenderInfo{piece.VAO.ID, flatShader.ID, static_cast<unsigned int>(piece.indices.size())});

	while (!glfwWindowShouldClose(window))
	{
		renderSystem->PreUpdate(0.0f);
		renderSystem->Update(0.0f);
		renderSystem->PostUpdate(0.0f);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}
