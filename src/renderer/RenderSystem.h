#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../core/System.h"
#include "Camera.h"

#include "../components/RenderInfo.h"



#include "../core/ComponentManager.h"

// https://github.com/maxbrundev/OpenGL-Modern-ECS-GameEngine/blob/master/Renderer/include/Systems/RenderSystem.h

extern GLFWwindow* window;
class RenderSystem: public System 
{
private:

    constexpr unsigned int screen_width = 800;
    constexpr unsigned int screen_height = 800;

    constexpr unsigned int aliasing_samples = 2;

    GLFWwindow* mWindow;
    Camera* mCamera;
public:
    explicit RenderSystem(GLFWwindow* window): mWindow(window){}
    ~RenderSystem() = default;

    void InitOpenGL() const;

    void PreUpdate(double dt);

    void Update(double dt);

    void PostUpdate(double dt);

    void SetCamera(Camera* camera);
};
