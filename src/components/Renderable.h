#pragma once

#include "VAO.h"
#include "shaderClass.h"
#include "Camera.h"

class Renderable {
    public:
    float scale = 1;
    glm::vec3 worldPos = glm::vec3(0,0,0);
    glm::vec3 rotation = glm::vec3(0,0,0);

    virtual void Draw(Shader& shader, Camera& camera) = 0;

    void updateModelMat();

    protected:
    VAO VAO;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

void Renderable::updateModelMat() {
    glm::mat4 size = glm::mat4(scale);
    glm::mat4 rotated = glm::rotate(glm::rotate(glm::rotate(size, glm::radians(rotation.x), glm::vec3(1,0,0)), glm::radians(rotation.y), glm::vec3(0,1,0)), glm::radians(rotation.z), glm::vec3(0,0,1));
    modelMatrix = glm::translate(rotated, worldPos);
}