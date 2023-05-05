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
	Core::WindowManager windowManager;
	Camera cam{800,800, glm::vec3(0.0f, 1.0f, 7.0f)};
	windowManager.Init("OpenGL Window", 800, 800);
	GLFWwindow* window = windowManager.GetWindow();

	ecsController.Init();
	ecsController.RegisterComponent<Components::Transform>();
	ecsController.RegisterComponent<Components::RenderInfo>();

	auto renderSystem = ecsController.RegisterSystem<RenderSystem>();
	renderSystem->SetWindow(windowManager.GetWindow());

	Signature signature;
	signature.set(ecsController.GetComponentType<Components::Transform>());
	signature.set(ecsController.GetComponentType<Components::RenderInfo>());
	ecsController.SetSystemSignature<RenderSystem>(signature);

	auto entity = ecsController.CreateEntity();

	Shader flatShader("flat.vert", "flat.frag");
	ChessModel piece(king, glm::vec3(0.0f, 0.0f, 0.0f));

	ecsController.AddComponent(entity, Components::Transform());
	ecsController.AddComponent(entity, Components::RenderInfo{piece.VAO.ID, flatShader.ID, static_cast<unsigned int>(piece.indices.size())});

	Core::UniformBufferManager UBO;
	UBO.SetCamera(&cam);
	UBO.Allocate();
	UBO.InitBind();
	UBO.BindShader(flatShader);

	while (!glfwWindowShouldClose(window))
	{
		auto transform = ecsController.GetComponent<Components::Transform>(entity);
		transform.scale = glm::vec3(0.01f);
		transform.CalculateModelMat();

		glUniformMatrix4fv(flatShader.GetUniformLocation("model"), 1, GL_FALSE, value_ptr(transform.modelMat));

		UBO.UpdateData();

		renderSystem->PreUpdate(0.0f);
		renderSystem->Update(0.0f);
		renderSystem->PostUpdate(0.0f);
	}

	renderSystem->Clean();

	windowManager.Shutdown();

	return 0;
}
