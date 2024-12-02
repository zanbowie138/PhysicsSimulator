#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VBO.h"

class VAO
{
public:
    GLuint ID{};
    // Constructor that generates a VAO ID
    VAO() { glGenVertexArrays(1, &ID); }

    /**
     * @brief Links a VBO to the VAO using a certain layout
     *
     * @param VBO ID of the VBO to link
     * @param layout Specifies the index of the generic vertex attribute to be modified.
     * @param numComponents Number of components per vertex
     * @param type Data type of each component in array
     * @param stride Byte offset of each consecutive vertex
     * @param offset Specifies an offset of the first component of the first generic vertex attribute
    */
    static inline void LinkAttrib(const VBO& VBO, GLuint layout, GLint numComponents, GLenum type, GLsizei stride,
                                  const void* offset);

    void Bind() const { glBindVertexArray(ID); }
    static void Unbind() { glBindVertexArray(0); }
    void Delete() const { glDeleteVertexArrays(1, &ID); }
};

// Links a VBO to the VAO using a certain layout
inline void VAO::LinkAttrib(const VBO& VBO, const GLuint layout, const GLint numComponents, const GLenum type, const GLsizei stride,
                            const void* offset)
{
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO::Unbind();
}
