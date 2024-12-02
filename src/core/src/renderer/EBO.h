#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    void Bind() const { GL_FCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID)); }
    static void Unbind() { GL_FCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); }
    void Delete() const { GL_FCHECK(glDeleteBuffers(1, &ID)); }
};

template <typename T>
EBO::EBO(const std::vector<T>& indices)
{
    GL_FCHECK(glGenBuffers(1, &ID));
    GL_FCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
    GL_FCHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(T), indices.data(), GL_STATIC_DRAW));
    LOG(LOG_INFO) << "Created EBO buffer of size " << indices.size() << ".\n";
}

inline void EBO::PushData(const std::vector<GLuint>& indices)
{
    if (currentBufSize + indices.size() * sizeof(GLuint) < bufSize)
    {
        GL_FCHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentBufSize, indices.size() * sizeof(GLuint), indices.data()));
        currentBufSize += static_cast<GLuint>(indices.size() * sizeof(GLuint));
    } else
    {
        LOG(LOG_ERROR) << "Trying to push " << indices.size() << " elements into a buffer. Current size: " << currentBufSize/sizeof(GLuint) << ". Buffer size: " << bufSize/sizeof(GLuint) << ".\n";
    }
}

inline void EBO::AllocBuffer(const GLint size, const GLenum type)
{
    GL_FCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
    GL_FCHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, type));
    bufSize = size;
    LOG(LOG_INFO) << "Allocated EBO buffer of size " << size << ".\n";
}