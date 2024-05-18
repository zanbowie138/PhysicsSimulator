#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include "../components/RenderInfo.h"
#include "../components/Transform.h"
#include "../components/TextureInfo.h"

#include "../core/World.h"
#include "../core/ECS/System.h"
#include "../core/ECS/ComponentManager.h"

class RenderSystem final : public System
{
    GLFWwindow* mWindow;
public:
    explicit RenderSystem(): mWindow(nullptr){}

    void PreUpdate() const;

    void Update() const;

    void PostUpdate() const { glfwSwapBuffers(mWindow); }

    void SetWindow(GLFWwindow* window) { mWindow = window; }

    void Clean() override;
};

