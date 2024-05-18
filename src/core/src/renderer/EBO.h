#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern World world;
class EBO
{
public:
    mutable GLuint ID{};
    GLuint bufSize = 0;
    size_t currentBufSize = 0;

    // Constructors that generates a Element Buffer Object and links it to indices
    EBO() { glGenBuffers(1, &ID); }

    template <typename T>
    explicit EBO(const std::vector<T>& indices);

    inline void PushData(const std::vector<GLuint>& indices);
    inline void AllocBuffer(GLint size, GLenum type);
    void ClearData() { currentBufSize = 0; }

    void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
    static void Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    void Delete() const { glDeleteBuffers(1, &ID); }
};

template <typename T>
EBO::EBO(const std::vector<T>& indices)
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(T), indices.data(), GL_STATIC_DRAW);
    LOG(world.logger, LOG_INFO) << "Created EBO buffer of size " << indices.size() << ".\n";
}

inline void EBO::PushData(const std::vector<GLuint>& indices)
{
    if (currentBufSize + indices.size() * sizeof(GLuint) < bufSize)
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentBufSize, indices.size() * sizeof(GLuint), indices.data());
        currentBufSize += static_cast<GLuint>(indices.size() * sizeof(GLuint));
    } else
    {
        LOG(world.logger, LOG_ERROR) << "Trying to push " << indices.size() << " elements into a buffer. Current size: " << currentBufSize/sizeof(GLuint) << ". Buffer size: " << bufSize/sizeof(GLuint) << ".\n";
    }
}

inline void EBO::AllocBuffer(const GLint size, const GLenum type)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, type);
    bufSize = size;
    LOG(world.logger, LOG_INFO) << "Allocated EBO buffer of size " << size << ".\n";
}