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
}

inline void EBO::PushData(const std::vector<GLuint>& indices)
{
    assert(currentBufSize + indices.size() * sizeof(GLuint) < bufSize && "EBO Overflow");

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentBufSize, indices.size() * sizeof(GLuint), indices.data());
    currentBufSize += static_cast<GLuint>(indices.size() * sizeof(GLuint));
}

inline void EBO::AllocBuffer(const GLint size, const GLenum type)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, type);
    bufSize = size;
}