#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "core/WindowManager.h"
#include "core/GUI.h"
#include "core/UniformBufferManager.h"
#include "renderer/Camera.h"
#include "core/GlobalTypes.h"

#include "LayerStack.h"

class RenderSystem;
class PhysicsSystem;
namespace Physics { class DynamicBBTree; }

class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();
    void Run();

    Core::WindowManager& GetWindowManager() { return windowManager; }
    Camera& GetCamera() { return viewportCam; }
    GUI& GetGUI() { return gui; }
    std::shared_ptr<RenderSystem> GetRenderSystem() { return renderSystem; }
    const std::unordered_map<std::string, GLuint>& GetShaders() const { return shaders; }
    Physics::DynamicBBTree& GetPhysicsTree();
    Entity GetLightEntity() const { return lightEntity; }
    Core::UniformBufferManager& GetUBO() { return ubo; }
    void SetLightEntity(Entity e) { lightEntity = e; }

private:
    Core::WindowManager windowManager;
    GUI gui;
    Camera viewportCam;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::unordered_map<std::string, GLuint> shaders;
    Core::UniformBufferManager ubo;
    LayerStack layerStack;

    Entity lightEntity{0};
};
