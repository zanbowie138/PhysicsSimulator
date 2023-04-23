#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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

	auto renderer = SceneController(window, screen_width, screen_height);

	unsigned int frame = 0;
	double lastTime = glfwGetTime();
	double currentTime = 0.0;
	int nbFrames = 0;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{
			// If last printf() was more than 1 sec ago
			// printf and reset timer
			printf("%f mspf, %f fps\n", 1000.0 / static_cast<double>(nbFrames), static_cast<double>(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		frame++;


		renderer.DrawScene(frame);


		// Renders above everything
		glClear(GL_DEPTH_BUFFER_BIT);
		//pointRenderer.Draw(basicShader, camera);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	renderer.Clean();

	// Delete window
	glfwDestroyWindow(window);

	// Terminates GLFW
	glfwTerminate();
	return 0;
}
