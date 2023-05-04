#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <bitset>

#include "GlobalTypes.h"

namespace Core {
	class WindowManager
	{
	private:
		const unsigned int screen_width = 800;
		const unsigned int screen_height = 800;

		const unsigned int aliasing_samples = 2;

		GLFWwindow* mWindow = nullptr;
		std::bitset<static_cast<size_t>(InputButtons::ENUM_LENGTH)> mButtons;
	public:
		glm::vec2 GetMousePos() const
		{
			double mouseX;
			double mouseY;
			glfwGetCursorPos(mWindow, &mouseX, &mouseY);

			// Sets mouse cursor to the middle of the screen so that it doesn't end cam.up roaming around
			glfwSetCursorPos(mWindow, (screen_width / 2), (screen_height / 2));

			return std::move(glm::vec2(mouseX, mouseY));
		}
	public:
		void Init(const char* windowTitle, unsigned int windowWidth, unsigned int windowHeight)
		{
			// Initialize GLFW
			glfwInit();

			// Set GLFW version to 3.3 and use core profile (uses modern OpenGL functions)
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_SAMPLES, aliasing_samples);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			// Creates window, if window is null, throw error
			mWindow = glfwCreateWindow(screen_width, screen_height, "OpenGLWindow", nullptr, nullptr);
			assert(mWindow != nullptr && "GLFW window failed to initialize");

			// Creates object that stores the state of this instance of OpenGL (i think)
			glfwMakeContextCurrent(mWindow);

			gladLoadGL();

			//Specifies the transformation from normalized coordinates (0-1) to screen coordinates
			glViewport(0, 0, screen_width, screen_height);

			glEnable(GL_DEPTH_TEST);

			// Enable anti-aliasing
			glEnable(GL_MULTISAMPLE);
		}

		void ProcessInputs()
		{
			glfwPollEvents();

			mButtons.reset();

			// Handles key inputs
			if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::MOUSE));
			if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::W));
			if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::A));
			if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::S));
			if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::D));
			if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::SPACE));
			if (glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::CONTROL));
			if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				mButtons.set(static_cast<std::size_t>(InputButtons::SHIFT));


			// Handles mouse inputs
			if (mButtons.test(static_cast<std::size_t>(InputButtons::MOUSE)))
			{
				// Hides mouse cursor
				glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}
			else
			{
				// Unhides cursor since camera is not looking around anymore
				glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		void Shutdown()
		{
			// Delete window
			glfwDestroyWindow(mWindow);

			// Terminates GLFW
			glfwTerminate();
		}

		GLFWwindow* GetWindow()
		{
			return mWindow;
		}
	};
}