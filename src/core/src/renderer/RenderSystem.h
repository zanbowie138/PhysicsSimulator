#pragma once

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
    unsigned long long frames = 0;
public:
    explicit RenderSystem(): mWindow(nullptr){}

    void PreUpdate() const;

    void Update() const;

    void PostUpdate();

    void SetWindow(GLFWwindow* window) { mWindow = window; }

    void Clean() override;
};

