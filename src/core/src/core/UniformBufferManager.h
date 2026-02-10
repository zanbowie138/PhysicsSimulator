#pragma once
#include "GlobalTypes.h"
#include "../renderer/Camera.h"
#include "../utils/Logger.h"

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
        GLint offsetAlignment;
        GLint secondOffset;
        GLint bufferSize;
    public:
        inline UniformBufferManager();
        inline void BindBuffer();
        inline void UnbindBuffer();

        inline void Init();

        inline void DefineRanges();

        template <typename... Args>
        void BindShaders(const Shader& first, const Args&... rest);

        inline void UpdateData(const Camera& cam, const glm::vec3& lightPos = glm::vec3(0.0f, 1.0f, 0.0f));
    private:
        inline void BindShader(const Shader& shader);

        // inline void SetCamera(Camera* camera);

        inline void Clean();
    };

    inline UniformBufferManager::UniformBufferManager()
    {
        GL_FCHECK(glGenBuffers(1, &ID));
        GL_FCHECK(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offsetAlignment));
        LOG(LOG_INFO) << "UBO Alignment: " << offsetAlignment << "\n";
        secondOffset = std::max(offsetAlignment, 64);
    }

    inline void UniformBufferManager::BindBuffer()
    {
        GL_FCHECK(glBindBuffer(GL_UNIFORM_BUFFER, ID));
    }

    inline void UniformBufferManager::UnbindBuffer()
    {
        GL_FCHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
    
    inline void UniformBufferManager::Init()
    {
        // Allocate Buffer
        // Offset for each block must be a multiple of the alignment
        // TODO: Do this more dynamically
        bufferSize = std::max(sizeof(glm::mat4), (size_t)offsetAlignment) + sizeof(glm::vec4) * 3;
        BindBuffer();
        GL_FCHECK(glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW));
        LOG(LOG_INFO) << "Allocated Uniform Buffer of size " << bufferSize << ".\n";
        UnbindBuffer();

        // Define Ranges
        GL_FCHECK(glBindBufferRange(GL_UNIFORM_BUFFER, 0, ID, 0, sizeof(glm::mat4)));
        GL_FCHECK(glBindBufferRange(GL_UNIFORM_BUFFER, 1, ID, secondOffset, 48)); // TODO: Make this more dynamic
    }

    inline void UniformBufferManager::BindShader(const Shader& shader)
    {
        if (shader.UsesUniform(static_cast<std::size_t>(UniformBlockConfig::CAMERA)))
        {
            GL_FCHECK(glUniformBlockBinding(shader.ID, shader.GetUniformBlockIndex("Camera"), 0));
        }
        if (shader.UsesUniform(static_cast<std::size_t>(UniformBlockConfig::LIGHTING)))
        {
            GL_FCHECK(glUniformBlockBinding(shader.ID, shader.GetUniformBlockIndex("Lighting"), 1));
        }
    }

    template <typename... Args>
    void UniformBufferManager::BindShaders(const Shader& first, const Args&... rest)
    {
        BindBuffer();
        BindShader(first);
        (BindShader(rest), ...);
    }

    inline void UniformBufferManager::UpdateData(const Camera& cam, const glm::vec3& lightPos)
    {
        BindBuffer();

        struct {
            glm::vec4 camPos;
            glm::vec4 lightPos;
            glm::vec4 lightColor;
        } lightStruct;
        lightStruct = { glm::vec4(cam.position, 1.0), glm::vec4(lightPos, 1.0f), glm::vec4(1.0, 1.0, 1.0, 1.0) };

        char data[bufferSize];
        memcpy(data, &cam.cameraMatrix, sizeof(glm::mat4));
        memcpy(data + secondOffset, &lightStruct, sizeof(lightStruct));

        GL_FCHECK(glBufferData(GL_UNIFORM_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW));
    }

    // Deletes the UBO
    inline void UniformBufferManager::Clean()
    {
        GL_FCHECK(glDeleteBuffers(1, &ID));
        LOG(LOG_INFO) << "Cleaning Uniform Buffer.\n";
    }
};