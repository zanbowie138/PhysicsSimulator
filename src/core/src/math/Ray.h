#pragma once
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "core/GlobalTypes.h"
#include "physics/BoundingBox.h"
#include "utils/Logger.h"

class Ray
{
public:
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 invdir;
    bool sign[3];

    Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction), invdir(1.0f / direction)
    {
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }

    glm::vec3 GetPoint(float t) const { return origin + direction * t; }

    std::pair<float, bool> IsColliding(const BoundingBox& box) const;
};

inline std::pair<float, bool> Ray::IsColliding(const BoundingBox& box) const
{
    float txmin, txmax, tymin, tymax, tzmin, tzmax;

    txmin = (box.GetBound(1-sign[0]).x - origin.x) * invdir.x;
    txmax = (box.GetBound(sign[0]).x - origin.x) * invdir.x;
    tymin = (box.GetBound(1-sign[1]).y - origin.y) * invdir.y;
    tymax = (box.GetBound(sign[1]).y - origin.y) * invdir.y;
    // LOG(LOG_INFO) << "txmin: " << txmin << " txmax: " << txmax << " tymin: " << tymin << " tymax: " << tymax << "\n";

    if ((txmin > tymax) || (tymin > txmax))
        return std::make_pair(FLT_MAX, false);

    if (tymin > txmin)
        txmin = tymin;
    if (tymax < txmax)
        txmax = tymax;

    tzmin = (box.GetBound(1-sign[2]).z - origin.z) * invdir.z;
    tzmax = (box.GetBound(sign[2]).z - origin.z) * invdir.z;
    // LOG(LOG_INFO) << "tzmin: " << tzmin << " tzmax: " << tzmax << "\n";

    if ((txmin > tzmax) || (tzmin > txmax))
        return std::make_pair(FLT_MAX, false);

    if (tzmin > txmin)
        txmin = tzmin;
    if (tzmax < txmax)
        txmax = tzmax;

    return std::make_pair(txmin, true);
}

