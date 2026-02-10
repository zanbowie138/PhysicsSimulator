#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <random>

#include "core/GUI.h"
#include "core/UniformBufferManager.h"
#include "core/WindowManager.h"
#include "core/World.h"

#include "components/Components.h"

#include "renderer/RenderSystem.h"
#include "renderer/Texture.h"

#include "physics/PhysicsSystem.h"

#include "renderables/Lines.h"
#include "renderables/Mesh.h"
#include "renderables/Model.h"
#include "renderables/Points.h"

#include "math/mesh/MeshSimplify.h"
#include "math/mesh/SimpleShapes.h"

#include "lua_engine/LuaRuntime.h"
#include "lua_engine/LuaBindings.h"

#include "utils/Timer.h"
#include "utils/Logger.h"
#include "utils/Raycast.h"
#include "utils/Exceptions.h"

// Force use of discrete Nvidia GPU
#ifdef _WIN32
#include <windows.h>

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // Optimus: force switch to discrete GPU
// __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;//AMD
}
#undef ERROR // Prevent Windows ERROR macro from conflicting with Logger::ERROR
#endif


World world;

int main() {
	LOG_INIT("output.log");
	LOG_SET_PRINT_TO_CONSOLE(true);

	try {
		Utils::Timer timer("Setup");

		// Window creation
		Core::WindowManager windowManager("OpenGL Window", 900, 900, true);
		GLFWwindow *window = windowManager.GetWindow();

		GUI GUI{window};

		// Camera creation
		const auto &windowDimensions = windowManager.GetWindowDimensions();
		Camera cam{windowDimensions.first, windowDimensions.second, glm::vec3(0.0f, 1.0f, 7.0f)};
		windowManager.SetCamera(&cam);

		// Create RenderSystem and add dependencies
		auto renderSystem = world.RegisterSystem<RenderSystem,
			Components::Transform,
			Components::RenderInfo
		>();
		renderSystem->SetWindow(windowManager.GetWindow());

		// Create PhysicsSystem
		auto physicsSystem = world.RegisterSystem<PhysicsSystem,
			Components::Transform,
			Components::Rigidbody
		>();
		auto &tree = physicsSystem->tree;

		auto basicShader = Shader::Create("basic.vert", "basic.frag");
		if (!basicShader) {
			LOG(LOG_ERROR) << "Failed to load basic shader\n";
			return 1;
		}
		auto flatShader = Shader::Create("flat.vert", "flat.frag");
		if (!flatShader) {
			LOG(LOG_ERROR) << "Failed to load flat shader\n";
			return 1;
		}
		auto defaultShader = Shader::Create("default.vert", "default.frag");
		if (!defaultShader) {
			LOG(LOG_ERROR) << "Failed to load default shader\n";
			return 1;
		}
		auto diffuseShader = Shader::Create("diffuse.vert", "diffuse.frag");
		if (!diffuseShader) {
			LOG(LOG_ERROR) << "Failed to load diffuse shader\n";
			return 1;
		}
		basicShader->DisableUniform(static_cast<size_t>(UniformBlockConfig::LIGHTING));

		// Create shader map for lua scene loading
		std::unordered_map<std::string, GLuint> shaders;
		shaders["basic"] = basicShader->ID;
		shaders["flat"] = flatShader->ID;
		shaders["default"] = defaultShader->ID;
		shaders["diffuse"] = diffuseShader->ID;

		// Initialize Lua runtime
		LuaRuntime luaRuntime;
		luaRuntime.Initialize(world, tree, shaders);

		// Scene error state
		std::string sceneErrorMsg;
		bool showSceneError = false;
		std::string currentScenePath = "test.lua";

		// Try to load scene
		if (!luaRuntime.LoadScene(currentScenePath, sceneErrorMsg)) {
			LOG(LOG_ERROR) << "Failed to load scene, loading fallback\n";
			showSceneError = true;

			// Load minimal fallback scene
			std::string fallbackError;
			if (!luaRuntime.LoadFallbackScene(fallbackError)) {
				LOG(LOG_ERROR) << "Fallback scene failed: " << fallbackError << "\n";
				return 1;
			}
		} else {
			// Scene loaded successfully
			luaRuntime.CallOnInit();
		}

		Entity lightEntity = luaRuntime.GetLightEntity();

		// Manage Uniform Buffer
		Core::UniformBufferManager UBO;
		UBO.Init();

		// Set uniform blocks in shaders to UBO indexes
		UBO.BindShaders(*basicShader, *defaultShader, *flatShader, *diffuseShader);

		double lastFPSTime, currentTime;
		lastFPSTime = currentTime = glfwGetTime();

		unsigned int fpsFrameCount = 0;

		float time, mspf, fps;
		time = mspf = fps = 0.0f;

		std::cout << timer.ToString() << std::endl;

		Entity entity = Entity();
		bool entitySelected = false;

		while (!glfwWindowShouldClose(window)) {
			renderSystem->PreUpdate();

			float dt_mill = static_cast<float>(glfwGetTime() - currentTime) * 1000;

			currentTime = glfwGetTime();
			fpsFrameCount++;

			// Updates fps every second
			if (currentTime - lastFPSTime >= 1.0) {
				// If last fps update() was more than 1 sec ago
				mspf = 1000.0f / static_cast<float>(fpsFrameCount);
				fps = static_cast<float>(fpsFrameCount);
				fpsFrameCount = 0;
				lastFPSTime += 1.0;
				LOG_WRITE();
			}

			time += dt_mill;

			// Update window input bitset
			windowManager.ProcessInputs(!GUI.MouseOver());
			GUI.SetMouse(windowManager.mouseShown);
			// Move camera based on window inputs
			cam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos(), dt_mill);
			// Update camera matrix
			cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
			// Update uniform buffer
			glm::vec3 lightPos(0, 1, 0); // Default light position
			try {
				if (world.GetEntitySignature(lightEntity).test(world.GetComponentType<Components::Transform>())) {
					lightPos = world.GetComponent<Components::Transform>(lightEntity).worldPos;
				}
			} catch (const std::exception &e) {
				LOG(LOG_ERROR) << "Error getting light transform: " << e.what() << "\n";
			}
			UBO.UpdateData(cam, lightPos);

			// Invoke Lua callbacks
			luaRuntime.CallOnUpdate(dt_mill,
			                        LuaBindings::LuaInput::FromWindowManager(windowManager),
			                        LuaBindings::LuaCameraView::FromCamera(cam));

			if (windowManager.TestInput(InputButtons::LEFT_MOUSE)) {
				luaRuntime.CallOnClick(
					LuaBindings::LuaInput::FromWindowManager(windowManager),
					LuaBindings::LuaCameraView::FromCamera(cam));
			}

			// Scene reload with Ctrl+R
			static bool rKeyPressed = false;
			bool rKeyDown = windowManager.TestInput(InputButtons::CONTROL) &&
			                glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

			if (rKeyDown && !rKeyPressed) {
				rKeyPressed = true;
				LOG(LOG_INFO) << "Reloading scene...\n";

				// Clear current world
				world.ClearAllEntities();

				// Attempt reload
				std::string reloadError;
				if (luaRuntime.LoadScene(currentScenePath, reloadError)) {
					// Success!
					showSceneError = false;
					luaRuntime.CallOnInit();
					LOG(LOG_INFO) << "Scene reloaded successfully\n";
				} else {
					// Failed, show error and load fallback
					sceneErrorMsg = reloadError;
					showSceneError = true;

					std::string fallbackError;
					luaRuntime.LoadFallbackScene(fallbackError);
				}

				// Update light entity
				lightEntity = luaRuntime.GetLightEntity();
			} else if (!rKeyDown) {
				rKeyPressed = false;
			}

			std::string fpsString("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMSPF: " + std::to_string(mspf));

			renderSystem->Update();
			GUI.NewFrame();

			GUI.StartWindow("Performance");
			GUI.Text(fpsString.c_str());
			GUI.EndWindow();

			GUI.ShowConfigWindow();
			GUI.EntityInfo(entity, entitySelected);
			GUI.RenderLog("Log Output", LOG_CONTENTS(), LOG_LINE_LEVELS());
		GUI.RenderLog("Lua Output", luaRuntime.luaLogger.GetContents(), luaRuntime.luaLogger.GetLineLevels());

			// Show error overlay if present
			if (showSceneError) {
				GUI.ShowErrorOverlay(sceneErrorMsg, showSceneError);
			}

			GUI.Render();

			renderSystem->PostUpdate();
		}

		GL_CHECK();

		world.Clean();
		GUI.Clean();

		windowManager.Shutdown();

		return 0;
	} catch (const std::exception &e) {
		LOG(LOG_ERROR) << "Fatal error: " << e.what() << "\n";
		return 1;
	}
}
