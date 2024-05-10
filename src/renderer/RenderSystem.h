#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include "../components/RenderInfo.h"
#include "../components/Transform.h"
#include "../components/TextureInfo.h"

#include "../core/ECS/ECSController.h"
#include "../core/ECS/System.h"
#include "../core/ECS/ComponentManager.h"

class RenderSystem: public System 
{
private:
    GLFWwindow* mWindow;
public:
    explicit RenderSystem(): mWindow(nullptr){}

    void PreUpdate();

    void Update();

    void PostUpdate();

    void SetWindow(GLFWwindow* window);

    void Clean() override;
};

