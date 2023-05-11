#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

#include "../components/RenderInfo.h"
#include "../components/Transform.h"
#include "../components/TextureInfo.h"

#include "../core/ECS.h"
#include "../core/System.h"
#include "../core/ComponentManager.h"

// https://github.com/maxbrundev/OpenGL-Modern-ECS-GameEngine/blob/master/Renderer/include/Systems/RenderSystem.h

class RenderSystem: public System 
{
private:
    GLFWwindow* mWindow;
    Camera* mCamera;
    mutable ImGuiIO* ioptr;
public:
    explicit RenderSystem(): mWindow(nullptr), mCamera(nullptr), ioptr(nullptr){}
    ~RenderSystem() = default;

    void InitOpenGL() const;

    void PreUpdate();

    void Update();

    void PostUpdate();

    void SetWindow(GLFWwindow* window);
    void SetCamera(Camera* camera);

    void Clean() override;
};

