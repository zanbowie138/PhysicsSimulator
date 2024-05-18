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
#include "../utils/Logger.h"

extern Utils::Logger logger;

class RenderSystem final : public System
{
    GLFWwindow* mWindow;
public:
    explicit RenderSystem(): mWindow(nullptr)
    {
        LOG(logger, LOG_INFO) << "RenderSystem initialization started.\n";
    }

    void PreUpdate() const
    {
        LOG(logger, LOG_INFO) << "RenderSystem PreUpdate.\n";
    }

    void Update() const
    {
        LOG(logger, LOG_INFO) << "RenderSystem Update.\n";
    }

    void PostUpdate() const
    {
        glfwSwapBuffers(mWindow);
        LOG(logger, LOG_INFO) << "RenderSystem PostUpdate: Buffers swapped.\n";
    }

    void SetWindow(GLFWwindow* window)
    {
        mWindow = window;
        LOG(logger, LOG_INFO) << "RenderSystem window set.\n";
    }

    void Clean() override
    {
        LOG(logger, LOG_INFO) << "Cleaning RenderSystem.\n";
    }
};
