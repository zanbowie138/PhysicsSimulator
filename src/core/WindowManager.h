#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <bitset>

#include "GlobalTypes.h"

namespace Core {
	class WindowManager
	{
		unsigned int screenWidth = 0;
		unsigned int screenHeight = 0;

		const unsigned int aliasingSamples = 5;

		GLFWwindow* mWindow = nullptr;
		Camera* mCamera = nullptr;

		glm::vec2 mMousePos = glm::vec2(0.0f);
		InputBitset mButtons;

		bool firstClick = false;
	public:
		WindowManager(const char* windowTitle, unsigned windowWidth, unsigned windowHeight);

		const InputBitset& GetInputs() const;
		const glm::vec2& GetMousePos() const;
		std::pair<unsigned int, unsigned int> GetWindowDimensions() const;

		void SetCamera(Camera* cam);
		void UpdateWindowDimensions(int width, int height);

		bool mouseShown = true;

		void ImGuiInit();

		void ProcessInputs(bool);

		void Shutdown() const;

		GLFWwindow* GetWindow() const;
	};

	inline WindowManager::WindowManager(const char* windowTitle, unsigned windowWidth, unsigned windowHeight)
	{
		screenHeight = windowHeight;
		screenWidth = windowWidth;

		// Initialize GLFW
		glfwInit();

		// Set GLFW version to 3.3 and use core profile (uses modern OpenGL functions)
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, aliasingSamples);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Creates window, if window is null, throw error
		mWindow = glfwCreateWindow(screenWidth, screenHeight, "OpenGLWindow", nullptr, nullptr);
		assert(mWindow != nullptr && "GLFW window failed to initialize");

		// Creates object that stores the state of this instance of OpenGL (i think)
		glfwMakeContextCurrent(mWindow);

		gladLoadGL();

		glfwSetWindowUserPointer(mWindow, this);

		// Define function that will be called when window is resized
		auto resizeWindowCallback = [](GLFWwindow* w, int width, int height)
			{
				static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->UpdateWindowDimensions(width, height);
			};
		glfwSetWindowSizeCallback(mWindow, resizeWindowCallback);

		//Specifies the transformation from normalized coordinates (0-1) to screen coordinates
		glViewport(0, 0, screenWidth, screenHeight);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		// Enable anti-aliasing
		glEnable(GL_MULTISAMPLE);
	}

	inline const InputBitset& WindowManager::GetInputs() const
	{
		return mButtons;
	}

	inline const glm::vec2& WindowManager::GetMousePos() const
	{
		return mMousePos;
	}

	inline std::pair<unsigned, unsigned> WindowManager::GetWindowDimensions() const
	{
		return std::make_pair(screenWidth, screenHeight);
	}

	inline void WindowManager::SetCamera(Camera* cam)
	{
		mCamera = cam;
	}

	inline void WindowManager::UpdateWindowDimensions(int width, int height)
	{
		screenWidth = static_cast<unsigned>(width);
		screenHeight = static_cast<unsigned>(height);

		// Update viewport
		glViewport(0, 0, screenWidth, screenHeight);

		// Update camera dimensions
		mCamera->width = screenWidth;
		mCamera->height = screenHeight;
	}

	inline void WindowManager::ProcessInputs(bool active)
	{
		glfwPollEvents();

		mouseShown = true;
		mButtons.reset();

		if (active)
		{
			// Set mouse position variables before they are possibly reset
			double mouseX;
			double mouseY;
			glfwGetCursorPos(mWindow, &mouseX, &mouseY);
			mMousePos = glm::vec2(mouseX, mouseY);

			// Handles key inputs
			if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				mButtons.set(static_cast<std::size_t>(InputButtons::RIGHT_MOUSE));

				// Checking if this is a first time click
				if (firstClick)
				{
					glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					mMousePos = glm::vec2(screenWidth / 2, screenHeight / 2);
				}

				mouseShown = false;
				firstClick = false;

				// Center mouse to prevent drifting
				glfwSetCursorPos(mWindow, (screenWidth / 2), (screenHeight / 2));
			}
			else if (!firstClick)
			{
				// Unhides cursor since camera is not looking around anymore
				glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

				mouseShown = true;
				firstClick = true;
			}
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
		}
	}

	inline void WindowManager::Shutdown() const
	{
		// Delete window
		glfwDestroyWindow(mWindow);

		// Terminates GLFW
		glfwTerminate();
	}

	inline GLFWwindow* WindowManager::GetWindow() const
	{
		return mWindow;
	}

}
