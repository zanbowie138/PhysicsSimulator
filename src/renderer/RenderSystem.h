#pragma once
#include "../core/System.h"
#include "Camera.h"
#include "../core/ECS.h"

#include "../components/RenderInfo.h"
#include "../core/ComponentManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// https://github.com/maxbrundev/OpenGL-Modern-ECS-GameEngine/blob/master/Renderer/include/Systems/RenderSystem.h

class RenderSystem: public System 
{
private:

    const unsigned int screen_width = 800;
    const unsigned int screen_height = 800;

    const unsigned int aliasing_samples = 2;

    GLFWwindow* mWindow; // TODO: Figure out cleaning
    Camera* mCamera;
    mutable ImGuiIO* ioptr;
public:
    explicit RenderSystem(): mWindow(nullptr), mCamera(nullptr), ioptr(nullptr){}
    ~RenderSystem() = default;

    void InitOpenGL() const;

    void PreUpdate(double dt);

    void Update(double dt);

    void PostUpdate(double dt);

    void SetWindow(GLFWwindow* window);
    void SetCamera(Camera* camera);

    void Clean();
};
