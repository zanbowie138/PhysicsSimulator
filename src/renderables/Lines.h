#pragma once

#include <vector>

#include "components/Renderable.h"

class Lines : public Renderable {
public:
    std::vector<glm::vec3> lines;

    Lines();
    Lines(std::vector<glm::vec3> lines);

    void Draw(Shader& shader, Camera& camera);

};

Lines::Lines()
{
    lines = std::vector<glm::vec3>();
}

Lines::Lines(std::vector<glm::vec3> lines)
{
    Lines::lines = lines;
}

void Lines::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    VAO.Bind();

    glEnable(GL_PROGRAM_POINT_SIZE);

    glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    camera.Matrix(shader, "camMatrix");

    glDrawArrays(GL_POINTS, 0, lines.size());

    VAO.Unbind();
}