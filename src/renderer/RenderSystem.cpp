#include "RenderSystem.h"

extern ECSController ecsController;

void RenderSystem::InitOpenGL() const
{
	/*
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO(); (void)io;
	ioptr = &io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");*/
}

void RenderSystem::PreUpdate(double dt)
{
	glfwPollEvents();
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();
}

void RenderSystem::Update(double dt)
{
	//assert(mCamera && "Camera not set.");
	assert(mWindow && "Window not set.");

	/*if (!ioptr->WantCaptureMouse)
	{
		// TODO: GET INPUTS
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	}*/

	for (auto const& entity : mEntities)
	{
		const auto& [VAO_ID, shader_ID, indices] = ecsController.GetComponent<Components::RenderInfo>(entity);

		// Bind vertex array
		glBindVertexArray(VAO_ID);

		// Bind shader
		glUseProgram(shader_ID);

		// TODO: Uniforms and model matrix

		// Draw VAO
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr);
	}
}

void RenderSystem::PostUpdate(double dt)
{
	/*ImGui::Begin("FPS Counter");
	std::stringstream ss;
	//ss << "FPS: " << fps << "\nMSPF: " << mspf;
	std::string s = ss.str();
	ImGui::Text(s.c_str());
	ImGui::End();*/

	glfwSwapBuffers(mWindow);
}

void RenderSystem::SetCamera(Camera* camera)
{
	mCamera = camera;
}

void RenderSystem::Clean()
{
	/*ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();*/
}

void RenderSystem::SetWindow(GLFWwindow* window)
{
	mWindow = window;
}
