#pragma once
#include "GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../utils/Logger.h"

extern Utils::Logger logger;

namespace Core {
    // Global Uniforms
    struct UniformBlock
    {
        glm::mat4 camMatrix;  // 0 // 64

        glm::vec4 camPos;	  // 64 // 16
        glm::vec4 lightPos;	  //    // 16
        glm::vec4 lightColor; // 112 // 16
    };

    class UniformBufferManager 
    {
    private:
        GLuint ID;
    public:
        inline UniformBufferManager();
        inline void BindBuffer();
        inline void UnbindBuffer();

        inline void AllocateBuffer();
        
        inline void DefineRanges();
        inline void BindShader(const Shader& shader);

        inline void UpdateData(const Camera& cam, const glm::vec3& lightPos = glm::vec3(0.0f, 1.0f, 0.0f));
        inline void SetCamera(Camera* camera);

        inline void Clean();
    };

    inline UniformBufferManager::UniformBufferManager()
    {
        glGenBuffers(1, &ID);
        LOG(logger, LOG_INFO) << "UniformBufferManager buffer generated.\n";
    }

    inline void UniformBufferManager::BindBuffer()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        LOG(logger, LOG_INFO) << "UniformBufferManager buffer bound.\n";
    }

    inline void UniformBufferManager::UnbindBuffer()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        LOG(logger, LOG_INFO) << "UniformBufferManager buffer unbound.\n";
    }
    
    inline void UniformBufferManager::AllocateBuffer()
    {
        BindBuffer();
        glBufferData(GL_UNIFORM_BUFFER, 112, nullptr, GL_DYNAMIC_DRAW);
        UnbindBuffer();
        LOG(logger, LOG_INFO) << "UniformBufferManager buffer allocated.\n";
    }

    inline void UniformBufferManager::DefineRanges()
    {
        // glBindBufferRange(GL_UNIFORM_BUFFER, index, ID, offset, size)
        BindBuffer();
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ID, 0, sizeof(glm::mat4));	
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, ID, 64, 48);	
        LOG(logger, LOG_INFO) << "UniformBufferManager buffer ranges defined.\n";
    }

    inline void UniformBufferManager::BindShader(const Shader& shader)
    {
        BindBuffer();
        if (shader.mUniforms.test((static_cast<std::size_t>(UniformBlockConfig::CAMERA))))
            glUniformBlockBinding(shader.ID, shader.GetUniformBlockIndex("Camera"), 0);
        if (shader.mUniforms.test((static_cast<std::size_t>(UniformBlockConfig::LIGHTING))))
            glUniformBlockBinding(shader.ID, shader.GetUniformBlockIndex("Lighting"), 1);
        LOG(logger, LOG_INFO) << "UniformBufferManager shader bound.\n";
    }

    inline void UniformBufferManager::UpdateData(const Camera& cam, const glm::vec3& lightPos)
    {
        BindBuffer();
        UniformBlock ub = {};
        ub.camMatrix = cam.cameraMatrix;
        ub.camPos = glm::vec4(cam.position, 1.0);
        ub.lightPos = glm::vec4(lightPos, 1.0f);
        ub.lightColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

        char ub_char[sizeof(ub)];
        memcpy(ub_char, &ub, sizeof(ub));

        glBufferData(GL_UNIFORM_BUFFER, sizeof(ub), ub_char, GL_DYNAMIC_DRAW);
        LOG(logger, LOG_INFO) << "UniformBufferManager data updated.\n";
    }

    // Deletes the UBO
    inline void UniformBufferManager::Clean()
    {
        glDeleteBuffers(1, &ID);
        LOG(logger, LOG_INFO) << "UniformBufferManager cleaned.\n";
    }
};
