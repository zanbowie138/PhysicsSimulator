#pragma once
#include "System.h"
#include "Camera.h"
#include "shaderClass.h"
#include "UBO.h"
#include "EntityManager.h"
#include "ComponentManager.h"

// https://github.com/maxbrundev/OpenGL-Modern-ECS-GameEngine/blob/master/Renderer/include/Systems/RenderSystem.h
struct Transform {
    glm::vec3 worldPos;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 modelMat;
};

class RenderSystem: public System 
{
private:
    GLFWwindow* mWindow;
    Camera mCamera;
public:
    explicit RenderSystem(GLFWwindow window);
    ~RenderSystem() = default;

    void InitOpenGL() const;

    void SetCamera()
};