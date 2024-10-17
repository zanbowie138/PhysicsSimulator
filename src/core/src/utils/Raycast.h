#pragma once
#include <glm/glm.hpp>
#include "math/Ray.h"

namespace Utils
{
    Ray ScreenPointToRay(const glm::vec2& uv, const glm::mat4& cam_matrix)
    {
        // Convert screen coordinates to normalized device coordinates (NDC)
        float x = (2.0f * uv.x) - 1.0f;
        float y = (2.0f * (1.0f - uv.y)) - 1.0f;

        glm::vec4 screenSpace(x, y, -1.0f, 1.0f);  // Near plane (in NDC)

        // Unproject to get the near plane ray (view space coordinates)
        glm::mat4 inverseViewProjection = glm::inverse(cam_matrix);
        glm::vec4 nearRay = inverseViewProjection * screenSpace;

        // Perform perspective divide to transform to camera space (divide by w)
        glm::vec3 rayOrigin = glm::vec3(nearRay) / nearRay.w;

        // For the ray direction, we use the far plane (for simplicity, use a point at far distance)
        screenSpace.z = 1.0f;  // Far plane in NDC
        glm::vec4 farRay = inverseViewProjection * screenSpace;
        glm::vec3 rayDirection = glm::normalize(glm::vec3(farRay) / farRay.w - rayOrigin);

        return Ray(glm::vec3(rayOrigin), rayDirection);
    }
}
