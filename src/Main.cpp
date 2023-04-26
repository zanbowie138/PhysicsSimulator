#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "SceneController.h"

constexpr unsigned int screen_width = 800;
constexpr unsigned int screen_height = 800;

constexpr unsigned int aliasing_samples = 8;

int main()
{
	// Initialize GLFW
	glfwInit();

	// Set GLFW version to 3.3 and use core profile (uses modern OpenGL functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, aliasing_samples);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates window, if window is null, throw error
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "OpenGLWindow", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

	// Creates object that stores the state of this instance of OpenGL (i think)
	glfwMakeContextCurrent(window);

	gladLoadGL();

	//Specifies the transformation from normalized coordinates (0-1) to screen coordinates
	glViewport(0, 0, screen_width, screen_height);

	glEnable(GL_DEPTH_TEST);

	// Enable anti-aliasing
	glEnable(GL_MULTISAMPLE);

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	auto renderer = SceneController(window, screen_width, screen_height);

	unsigned int frame = 0;
	double lastTime = glfwGetTime();
	double currentTime = 0.0;
	int nbFrames = 0;
	float fps = 0;
	float mspf = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!io.WantCaptureMouse)
		{
			renderer.HandleInputs(true);
			if (!renderer.mouseShown)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			}
		}

		currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{
			// If last printf() was more than 1 sec ago
			// printf and reset timer
			//printf("%f mspf, %f fps\n", 1000.0 / static_cast<double>(nbFrames), static_cast<double>(nbFrames));
			mspf = 1000.0 / static_cast<double>(nbFrames);
			fps = static_cast<double>(nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}
		frame++;

		renderer.DrawScene(frame);


		// Renders above everything
		//glClear(GL_DEPTH_BUFFER_BIT);
		//pointRenderer.Draw(basicShader, camera);

		ImGui::Begin("FPS Counter");
		std::stringstream ss;
		ss << "FPS: " << fps << "\nMSPF: " << mspf;
		std::string s = ss.str();
		ImGui::Text(s.c_str());
		//ImGui::Text("MSPF: " + mspf);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
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
