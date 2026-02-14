#pragma once

#include "Camera.h"
#include "ShadowMap.h"

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

    ShadowMap mShadowMap;
    GLuint mShadowShaderID = 0;
    glm::mat4 mLightSpaceMatrix{1.0f};
public:
    explicit RenderSystem(): mWindow(nullptr){}

    void PreUpdate();

    void Update() const;

    void PostUpdate();

    void ShadowPass();

    void SetWindow(GLFWwindow* window) { mWindow = window; }

    void InitShadowMap()                         { mShadowMap.Init(); }
    void SetShadowShader(GLuint id)              { mShadowShaderID = id; }
    void SetLightSpaceMatrix(const glm::mat4& m) { mLightSpaceMatrix = m; }

    void Clean() override;
};

