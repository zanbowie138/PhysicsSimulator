#pragma once
#include "GlobalTypes.h"
#include "UBO.h"

namespace Core {
    class UniformBufferManager 
    {
    public:
        UniformBufferManager();
        inline void Init();
    };

    inline void UniformBufferManager::Init() {
        UniformBlock ub = {};
        ub.camMatrix = cam.cameraMatrix;
        ub.camPos = glm::vec4(cam.position,1.0);
        ub.lightPos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        ub.lightColor = glm::vec4(1.0,1.0,1.0,1.0);
    }
};