#include "SimulationLayer.h"
#include "Application.h"

#include <algorithm>
#include <filesystem>

#include <GLFW/glfw3.h>

#include "lua_engine/LuaRuntime.h"
#include "lua_engine/LuaBindings.h"
#include "core/GUI.h"
#include "core/World.h"
#include "utils/PathUtils.h"
#include "utils/Logger.h"

extern World world;

SimulationLayer::SimulationLayer(Application& app)
    : app(app), luaRuntime(std::make_unique<LuaRuntime>()) {}

SimulationLayer::~SimulationLayer() = default;

void SimulationLayer::OnAttach() {
    const auto& shaderMap = app.GetShaders();
    auto& tree = app.GetPhysicsTree();

    if (!luaRuntime->Initialize(world, tree, shaderMap, sceneErrorMsg)) {
        showSceneError = true;
    } else if (!luaRuntime->LoadScene(currentScenePath, sceneErrorMsg)) {
        LOG(LOG_ERROR) << "Failed to load scene, loading fallback\n";
        showSceneError = true;
        std::string fallbackError;
        if (!luaRuntime->LoadFallbackScene(fallbackError))
            sceneErrorMsg += "\nFallback also failed: " + fallbackError;
    } else {
        luaRuntime->CallOnInit();
    }

    app.SetLightEntity(luaRuntime->GetLightEntity());

    const auto& cfg = luaRuntime->GetCameraConfig();
    auto& cam = app.GetCamera();
    cam.position    = cfg.position;
    cam.orientation = cfg.orientation;

    ScanScenes();
}

bool SimulationLayer::OnUpdate(float dt) {
    auto& wm  = app.GetWindowManager();
    auto& cam = app.GetCamera();

    if (simRunning) {
        luaRuntime->simTime += dt;
        frameNumber++;
    }

    if (luaRuntime->IsCameraMovementEnabled())
        cam.MoveCam(wm.GetInputs(), wm.GetMousePos(), dt);

    const auto& camCfg = luaRuntime->GetCameraConfig();
    cam.UpdateMatrix(camCfg.fov, camCfg.nearPlane, camCfg.farPlane);

    if (simRunning) {
        luaRuntime->CallOnUpdate(dt,
            LuaBindings::LuaInput::FromWindowManager(wm),
            LuaBindings::LuaCameraView::FromCamera(cam));

        if (wm.TestInput(InputButtons::LEFT_MOUSE))
            luaRuntime->CallOnClick(
                LuaBindings::LuaInput::FromWindowManager(wm),
                LuaBindings::LuaCameraView::FromCamera(cam));
    }

    bool rKeyDown = wm.TestInput(InputButtons::CONTROL) &&
                    glfwGetKey(wm.GetWindow(), GLFW_KEY_R) == GLFW_PRESS;
    if (rKeyDown && !rKeyPressed) {
        rKeyPressed = true;
        ReloadScene();
    } else if (!rKeyDown) {
        rKeyPressed = false;
    }

    return false;
}

void SimulationLayer::OnGUI() {
    auto& gui = app.GetGUI();

    luaRuntime->CallOnGUI();

    GUI::StartWindow("Lua Scene");

    ImGui::Spacing();
    ImGui::SeparatorText("Scene Selection");

    float listHeight = std::min(static_cast<int>(sceneFiles.size()), 5)
        * ImGui::GetTextLineHeightWithSpacing()
        + ImGui::GetStyle().FramePadding.y * 2;
    if (ImGui::BeginChild("##scenes", ImVec2(-FLT_MIN, listHeight), true)) {
        for (int i = 0; i < static_cast<int>(sceneFiles.size()); i++) {
            std::string displayName = sceneFiles[i];
            if (displayName.size() > 4)
                displayName = displayName.substr(0, displayName.size() - 4);
            bool isSelected = (i == selectedSceneIdx);
            if (ImGui::Selectable(displayName.c_str(), isSelected) && !isSelected) {
                selectedSceneIdx = i;
                currentScenePath = sceneFiles[i];
                ReloadScene();
                frameNumber = 0;
                luaRuntime->simTime = 0.0f;
                simRunning = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
            if (ImGui::IsItemHovered()) {
                const auto& desc = luaRuntime->GetSceneDescription();
                ImGui::SetTooltip("%s", desc.empty() ? "(no description)" : desc.c_str());
            }
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("Refresh"))
        ScanScenes();
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Rescan scenes/ folder");

    {
        ImGui::Spacing();
        ImGui::SeparatorText("Current Scene");
        std::string sceneName = currentScenePath.size() > 4
            ? currentScenePath.substr(0, currentScenePath.size() - 4)
            : currentScenePath;
        ImGui::Text("Filename: %s", sceneName.c_str());
        const auto& desc = luaRuntime->GetSceneDescription();
        ImGui::Text("Description: ");
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextDisabled("%s", desc.empty() ? "(none)" : desc.c_str());
        ImGui::PopTextWrapPos();
    }

    ImGui::Spacing();
    GUI::Text(("Frame: " + std::to_string(frameNumber)).c_str());
    GUI::ButtonFunc(simRunning ? "Stop" : "Start", [&]() {
        simRunning = !simRunning;
    });
    ImGui::SameLine();
    GUI::ButtonFunc("Restart", [&]() {
        ReloadScene();
        frameNumber = 0;
        luaRuntime->simTime = 0.0f;
        simRunning = true;
    });

    ImGui::Spacing();
    ImGui::SeparatorText("Lua Output");
    ImGui::Spacing();
    gui.RenderLogInline("Lua Output",
        luaRuntime->luaLogger.GetContents(),
        luaRuntime->luaLogger.GetLineLevels());

    GUI::EndWindow();

    if (showSceneError)
        gui.ShowErrorOverlay(sceneErrorMsg, showSceneError);
}

void SimulationLayer::ReloadScene() {
    LOG(LOG_INFO) << "Reloading scene...\n";
    world.ClearAllEntities();

    std::string err;
    if (!luaRuntime->Reinitialize(err)) {
        sceneErrorMsg = "Lua reinit failed: " + err;
        showSceneError = true;
        app.SetLightEntity(luaRuntime->GetLightEntity());
        return;
    }

    if (luaRuntime->LoadScene(currentScenePath, err)) {
        showSceneError = false;
        luaRuntime->CallOnInit();
        LOG(LOG_INFO) << "Scene reloaded successfully\n";
    } else {
        sceneErrorMsg = err;
        showSceneError = true;
        std::string fbErr;
        if (!luaRuntime->LoadFallbackScene(fbErr))
            sceneErrorMsg += "\nFallback also failed: " + fbErr;
    }

    app.SetLightEntity(luaRuntime->GetLightEntity());
    const auto& cfg = luaRuntime->GetCameraConfig();
    auto& cam = app.GetCamera();
    cam.position    = cfg.position;
    cam.orientation = cfg.orientation;
}

void SimulationLayer::ScanScenes() {
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
}
