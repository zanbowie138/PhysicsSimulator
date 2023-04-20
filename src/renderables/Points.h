#pragma once

#include <vector>

#include "components/Renderable.h"

class Points: public Renderable {
    public:
    std::vector<glm::vec3> points;

    Points();
    Points(std::vector<glm::vec3> points);

    void Draw(Shader& shader, Camera& camera);

};

Points::Points() 
{
    points = std::vector<glm::vec3>();

    VAO.Bind();

    VBO VBO(points, GL_DYNAMIC_DRAW);

    VAO.LinkAttrib(VBO, 0, 1, GL_FLOAT, sizeof(glm::vec3), (void*)0);

    VAO.Unbind();
    VBO.Unbind();
}

Points::Points(std::vector<glm::vec3> points)
{
    Points::points = points;

    VAO.Bind();

    VBO VBO(points, GL_DYNAMIC_DRAW);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);

    VAO.Unbind();
    VBO.Unbind();
}

void Points::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    VAO.Bind();

	glEnable(GL_PROGRAM_POINT_SIZE);

    glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    camera.Matrix(shader, "camMatrix");

    glDrawArrays(GL_POINTS, 0, points.size());

    VAO.Unbind();
}