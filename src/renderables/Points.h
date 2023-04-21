#pragma once

#include <vector>

#include "components/Renderable.h"
#include "Camera.h"
#include "VBO.h"

class Points: public Renderable {
    public:
    std::vector<glm::vec3> points;

    VBO VBO;

    inline Points(GLuint amount);

    inline void PushBack(const std::vector<glm::vec3>& pts);
    inline void Draw(Shader& shader, Camera& camera);

};

Points::Points(GLuint amount)
{
    points = std::vector<glm::vec3>();

    VAO.Bind();

    VBO.AllocBuffer(amount * sizeof(glm::vec3), GL_DYNAMIC_DRAW);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);

    VAO.Unbind();
    VBO.Unbind();
}

void Points::PushBack(const std::vector<glm::vec3>& pts)
{
    VBO.Bind();
    points.insert(points.end(), pts.begin(), pts.end());
    VBO.PushData(pts);
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