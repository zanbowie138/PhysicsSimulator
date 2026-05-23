#include <algorithm>
#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>

#include "core/GUI.h"
#include "core/UniformBufferManager.h"
#include "core/WindowManager.h"
#include "core/World.h"

#include "components/Components.h"

#include "renderer/RenderSystem.h"
#include "renderer/Texture.h"

#include "physics/PhysicsSystem.h"

#include "renderables/Lines.h"

#include "lua_engine/LuaRuntime.h"
#include "lua_engine/LuaBindings.h"

#include "utils/Timer.h"
#include "utils/Logger.h"

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

		// Camera creation (position applied after scene load)
		const auto &windowDimensions = windowManager.GetWindowDimensions();
		Camera viewportCam{windowDimensions.first, windowDimensions.second, glm::vec3(0.0f)};
		windowManager.SetCamera(&viewportCam);

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
		auto shadowShader = Shader::Create("shadow.vert", "shadow.frag");
		if (!shadowShader) {
			LOG(LOG_ERROR) << "Failed to load shadow shader\n";
			return 1;
		}

		basicShader->DisableUniform(static_cast<size_t>(UniformBlockConfig::LIGHTING));

		// Create shader map for lua scene loading
		std::unordered_map<std::string, GLuint> shaders;
		shaders["basic"] = basicShader->ID;
		shaders["flat"] = flatShader->ID;
		shaders["default"] = defaultShader->ID;
		shaders["diffuse"] = diffuseShader->ID;

		renderSystem->InitShadowMap();
		renderSystem->SetShadowShader(shadowShader->ID);

		// Initialize Lua runtime
		LuaRuntime luaRuntime;
		std::string sceneErrorMsg;
		bool showSceneError = false;
		std::string currentScenePath = "test.lua";
		std::vector<std::string> sceneFiles;
		int selectedSceneIdx = 0;

		if (!luaRuntime.Initialize(world, tree, shaders, sceneErrorMsg)) {
			// Continue with an empty world + error overlay rather than exiting.
			showSceneError = true;
		} else if (!luaRuntime.LoadScene(currentScenePath, sceneErrorMsg)) {
			LOG(LOG_ERROR) << "Failed to load scene, loading fallback\n";
			showSceneError = true;
			std::string fallbackError;
			if (!luaRuntime.LoadFallbackScene(fallbackError)) {
				sceneErrorMsg += "\nFallback also failed: " + fallbackError;
			}
		} else {
			luaRuntime.CallOnInit();
		}

		Entity lightEntity = luaRuntime.GetLightEntity();
		{
			const auto& cfg = luaRuntime.GetCameraConfig();
			viewportCam.position    = cfg.position;
			viewportCam.orientation = cfg.orientation;
		}

		// Shared reload path: rebuild Lua state, clear world, load scene (or fallback).
		auto reloadScene = [&]() {
			LOG(LOG_INFO) << "Reloading scene...\n";
			world.ClearAllEntities();

			std::string err;
			if (!luaRuntime.Reinitialize(err)) {
				sceneErrorMsg = "Lua reinit failed: " + err;
				showSceneError = true;
				lightEntity = luaRuntime.GetLightEntity();
				return;
			}

			if (luaRuntime.LoadScene(currentScenePath, err)) {
				showSceneError = false;
				luaRuntime.CallOnInit();
				LOG(LOG_INFO) << "Scene reloaded successfully\n";
			} else {
				sceneErrorMsg = err;
				showSceneError = true;
				std::string fbErr;
				if (!luaRuntime.LoadFallbackScene(fbErr)) {
					sceneErrorMsg += "\nFallback also failed: " + fbErr;
				}
			}
			lightEntity = luaRuntime.GetLightEntity();
			const auto& cfg = luaRuntime.GetCameraConfig();
			viewportCam.position    = cfg.position;
			viewportCam.orientation = cfg.orientation;
		};

		auto scanScenes = [&]() {
			sceneFiles.clear();
			std::string scenesDir = Utils::GetResourcePath("/scenes/", "");
			try {
				for (const auto& entry : std::filesystem::directory_iterator(scenesDir)) {
					if (entry.path().extension() == ".lua")
						sceneFiles.push_back(entry.path().filename().string());
				}
			} catch (const std::exception& e) {
				LOG(LOG_WARNING) << "Failed to scan scenes: " << e.what() << "\n";
			}
			std::sort(sceneFiles.begin(), sceneFiles.end());
			auto it = std::find(sceneFiles.begin(), sceneFiles.end(), currentScenePath);
			selectedSceneIdx = (it != sceneFiles.end())
				? static_cast<int>(std::distance(sceneFiles.begin(), it)) : 0;
		};
		scanScenes();

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

		bool simRunning = true;
		uint64_t frameNumber = 0;

		std::cout << timer.ToString() << std::endl;

		// Render loop
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

			if (simRunning) {
				time += dt_mill;
				luaRuntime.simTime += dt_mill;
				frameNumber++;
			}

			// Update window input bitset
			windowManager.ProcessInputs(!GUI.MouseOver());
			GUI.SetMouse(windowManager.mouseShown);
			// Move camera based on window inputs
			if (luaRuntime.IsCameraMovementEnabled()) {
				viewportCam.MoveCam(windowManager.GetInputs(), windowManager.GetMousePos(), dt_mill);
			}
			// Update camera matrix
			const auto& camCfg = luaRuntime.GetCameraConfig();
			viewportCam.UpdateMatrix(camCfg.fov, camCfg.nearPlane, camCfg.farPlane);
			// Update uniform buffer
			glm::vec3 lightPos(0, 1, 0); // Default light position
			try {
				if (world.GetEntitySignature(lightEntity).test(world.GetComponentType<Components::Transform>())) {
					lightPos = world.GetComponent<Components::Transform>(lightEntity).worldPos;
				}
			} catch (const std::exception &e) {
				LOG(LOG_ERROR) << "Error getting light transform: " << e.what() << "\n";
			}
			UBO.UpdateData(viewportCam, lightPos);

			// Compute light-space matrix and render shadow pass
			static constexpr float kShadowSize = 20.0f;
			glm::mat4 lightProj = glm::ortho(-kShadowSize, kShadowSize,
			                                  -kShadowSize, kShadowSize, 0.1f, 100.0f);
			glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			renderSystem->SetLightSpaceMatrix(lightProj * lightView);
			renderSystem->ShadowPass();

			// Invoke Lua callbacks
			if (simRunning) {
				luaRuntime.CallOnUpdate(dt_mill,
				                        LuaBindings::LuaInput::FromWindowManager(windowManager),
				                        LuaBindings::LuaCameraView::FromCamera(viewportCam));

				if (windowManager.TestInput(InputButtons::LEFT_MOUSE)) {
					luaRuntime.CallOnClick(
						LuaBindings::LuaInput::FromWindowManager(windowManager),
						LuaBindings::LuaCameraView::FromCamera(viewportCam));
				}
			}

			// Scene reload with Ctrl+R
			static bool rKeyPressed = false;
			bool rKeyDown = windowManager.TestInput(InputButtons::CONTROL) &&
			                glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

			if (rKeyDown && !rKeyPressed) {
				rKeyPressed = true;
				reloadScene();
			} else if (!rKeyDown) {
				rKeyPressed = false;
			}

			std::string fpsString("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMSPF: " + std::to_string(mspf));

			renderSystem->Update();
			GUI.NewFrame();

			luaRuntime.CallOnGUI();

			GUI.StartWindow("Performance");
			GUI.Text(fpsString.c_str());
			GUI.EndWindow();

			GUI.StartWindow("Lua Scene");

			ImGui::SeparatorText("Scene Selection");

			float listHeight = std::min(static_cast<int>(sceneFiles.size()), 5)
				* ImGui::GetTextLineHeightWithSpacing()
				+ ImGui::GetStyle().FramePadding.y * 2;
			if (ImGui::BeginListBox("##scenes", ImVec2(-FLT_MIN, listHeight))) {
				for (int i = 0; i < static_cast<int>(sceneFiles.size()); i++) {
					std::string displayName = sceneFiles[i];
					if (displayName.size() > 4)
						displayName = displayName.substr(0, displayName.size() - 4);
					bool isSelected = (i == selectedSceneIdx);
					if (ImGui::Selectable(displayName.c_str(), isSelected) && !isSelected) {
						selectedSceneIdx = i;
						currentScenePath = sceneFiles[i];
						reloadScene();
						frameNumber = 0;
						time = 0.0f;
						luaRuntime.simTime = 0.0f;
						simRunning = true;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
						if (ImGui::IsItemHovered()) {
							const auto& desc = luaRuntime.GetSceneDescription();
							ImGui::SetTooltip("%s", desc.empty() ? "(no description)" : desc.c_str());
						}
					}
				}
				ImGui::EndListBox();
			}
			if (ImGui::Button("Refresh")) {
				scanScenes();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Rescan scenes/ folder");

			{
				ImGui::SeparatorText("Current Scene");
				std::string sceneName = currentScenePath.size() > 4
					? currentScenePath.substr(0, currentScenePath.size() - 4)
					: currentScenePath;
				ImGui::Text("Filename: %s", sceneName.c_str());
				const auto& desc = luaRuntime.GetSceneDescription();
				ImGui::Text("Description: ");
				// ImGui::SameLine();
				ImGui::PushTextWrapPos(0.0f);
				ImGui::TextDisabled("%s", desc.empty() ? "(none)" : desc.c_str());
				ImGui::PopTextWrapPos();
			}

			GUI.Text(("Frame: " + std::to_string(frameNumber)).c_str());
			GUI.ButtonFunc(simRunning ? "Stop" : "Start", [&]() {
				simRunning = !simRunning;
			});
			ImGui::SameLine();
			GUI.ButtonFunc("Restart", [&]() {
				reloadScene();
				frameNumber = 0;
				time = 0.0f;
				luaRuntime.simTime = 0.0f;
				simRunning = true;
			});
			GUI.EndWindow();

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
