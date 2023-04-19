#pragma once

#include <vector>

#include "Renderable.h"

class Points: public Renderable {
    public:
    std::vector<glm::vec3> points;

    Points();

    void Draw(Shader& shader, Camera& camera);

};

void Points::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    VAO.Bind();

	glEnable(GL_PROGRAM_POINT_SIZE);

    camera.Matrix(shader, "camMatrix");

    glDrawArrays(GL_POINTS, 0, points.size());

    VAO.Unbind();
    shader.
}