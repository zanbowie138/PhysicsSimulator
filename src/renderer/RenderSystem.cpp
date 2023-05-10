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

void RenderSystem::PreUpdate()
{
	glfwPollEvents();
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();
}

void RenderSystem::Update()
{
	//assert(mCamera && "Camera not set.");
	assert(mWindow && "Window not set.");

	/*if (!ioptr->WantCaptureMouse)
	{
		// TODO: GET INPUTS
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	}*/


	for (const auto& entity : mEntities)
	{
		const auto& [primitive_type, VAO_ID, shader_ID, size] = ecsController.GetComponent<Components::RenderInfo>(entity);

		// Update transform
		auto transform = ecsController.GetComponent<Components::Transform>(entity);
		transform.scale = glm::vec3(0.01f);
		transform.CalculateModelMat();

		glUniformMatrix4fv(glGetUniformLocation(shader_ID, "model"), 1, GL_FALSE, value_ptr(transform.modelMat));

		if (ecsController.GetEntitySignature(entity).test(ecsController.GetComponentType<Components::TextureInfo>()))
		{
			const auto& [diffuse_ID, specular_ID] = ecsController.GetComponent<Components::TextureInfo>(entity);
			
			// Textures
			// Set texture uniform value
			glUniform1i(glGetUniformLocation(shader_ID, "diffuse"), 0);
			// Activate texture unit
			glActiveTexture(GL_TEXTURE0);
			// Bind texture to 
			glBindTexture(GL_TEXTURE_2D, diffuse_ID);

			glUniform1i(glGetUniformLocation(shader_ID, "specular"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_ID);
		}

		// Bind vertex array
		glBindVertexArray(VAO_ID);

		// Bind shader
		glUseProgram(shader_ID);

		// Draw VAO
		if (primitive_type == GL_POINTS)
			glDrawArrays(primitive_type, 0, size);
		else 
			glDrawElements(primitive_type, size, GL_UNSIGNED_INT, nullptr);
	}
}

void RenderSystem::PostUpdate()
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
