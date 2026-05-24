#include "Application.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/World.h"
#include "renderer/RenderSystem.h"
#include "renderer/Shader.h"
#include "physics/PhysicsSystem.h"
#include "components/Components.h"
#include "utils/Logger.h"

#include "layers/SimulationLayer.h"
#include "layers/RenderingLayer.h"
#include "layers/StatsLayer.h"

World world;

Application::Application(const std::string& title, int width, int height)
    : windowManager(title.c_str(), width, height, true),
      gui(windowManager.GetWindow()),
      viewportCam(windowManager.GetWindowDimensions().first,
                  windowManager.GetWindowDimensions().second,
                  glm::vec3(0.0f))
{
    windowManager.SetCamera(&viewportCam);

    renderSystem = world.RegisterSystem<RenderSystem,
        Components::Transform,
        Components::RenderInfo
    >();
    renderSystem->SetWindow(windowManager.GetWindow());

    physicsSystem = world.RegisterSystem<PhysicsSystem,
        Components::Transform,
        Components::Rigidbody
    >();

    auto basic   = Shader::Create("basic.vert",   "basic.frag");
    auto flat    = Shader::Create("flat.vert",    "flat.frag");
    auto def     = Shader::Create("default.vert", "default.frag");
    auto diffuse = Shader::Create("diffuse.vert", "diffuse.frag");
    auto shadow  = Shader::Create("shadow.vert",  "shadow.frag");

    if (!basic || !flat || !def || !diffuse || !shadow)
        throw std::runtime_error("Failed to load one or more shaders");

    basic->DisableUniform(static_cast<size_t>(UniformBlockConfig::LIGHTING));

    shaders["basic"]   = basic->ID;
    shaders["flat"]    = flat->ID;
    shaders["default"] = def->ID;
    shaders["diffuse"] = diffuse->ID;

    renderSystem->InitShadowMap();
    renderSystem->SetShadowShader(shadow->ID);

    ubo.Init();
    ubo.BindShaders(*basic, *def, *flat, *diffuse);

    layerStack.PushLayer(std::make_unique<SimulationLayer>(*this));
    layerStack.PushLayer(std::make_unique<RenderingLayer>(*this));
    layerStack.PushLayer(std::make_unique<StatsLayer>(*this));
}

Application::~Application() {
    world.Clean();
    GUI::Clean();
    windowManager.Shutdown();
}

Physics::DynamicBBTree& Application::GetPhysicsTree() {
    return physicsSystem->tree;
}

void Application::Run() {
    GLFWwindow* window = windowManager.GetWindow();

    layerStack.OnInit();

    double currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        renderSystem->PreUpdate();

        float dt_ms = static_cast<float>(glfwGetTime() - currentTime) * 1000.0f;
        currentTime = glfwGetTime();

        windowManager.ProcessInputs(!GUI::MouseOver());
        GUI::SetMouse(windowManager.mouseShown);

        layerStack.OnUpdate(dt_ms);

        GUI::NewFrame();
        layerStack.OnGUI();
        GUI::Render();

        renderSystem->PostUpdate();
    }

    GL_CHECK();
}
