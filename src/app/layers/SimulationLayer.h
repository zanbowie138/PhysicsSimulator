#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include "Layer.h"

class Application;
class LuaRuntime;

class SimulationLayer : public Layer {
public:
    explicit SimulationLayer(Application& app);
    ~SimulationLayer();

    void OnAttach() override;
    bool OnUpdate(float dt) override;
    void OnGUI() override;

private:
    void ReloadScene();
    void ScanScenes();

    Application& app;
    std::unique_ptr<LuaRuntime> luaRuntime;

    std::string currentScenePath{"test.lua"};
    std::vector<std::string> sceneFiles;
    std::unordered_map<std::string, std::string> sceneDescriptions;
    int selectedSceneIdx{0};
    bool simRunning{true};
    uint64_t frameNumber{0};
    bool showSceneError{false};
    std::string sceneErrorMsg;
    bool rKeyPressed{false};
};
