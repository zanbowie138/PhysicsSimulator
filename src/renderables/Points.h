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
}

Points::Points(std::vector<glm::vec3> points)
{
    Points::points = points;
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