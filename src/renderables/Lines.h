#pragma once

#include <vector>

#include "components/Renderable.h"

class Lines : public Renderable {
public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    Lines();
    Lines(std::vector<glm::vec3> lines);

    void Draw(Shader& shader, Camera& camera);

};

Lines::Lines()
{
    vertices = std::vector<glm::vec3>();
    VAO.Bind();

    VBO VBO(vertices, GL_DYNAMIC_DRAW);
    EBO EBO(indices, GL_DYNAMIC_DRAW);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

Lines::Lines(std::vector<glm::vec3> lines)
{
    Lines::vertices = lines;
    VAO.Bind();

    VBO VBO(vertices, GL_DYNAMIC_DRAW);
    EBO EBO(indices, GL_DYNAMIC_DRAW);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Lines::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    VAO.Bind();

    glEnable(GL_PROGRAM_POINT_SIZE);

    glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    camera.Matrix(shader, "camMatrix");

    glDrawArrays(GL_POINTS, 0, vertices.size());

    VAO.Unbind();
}