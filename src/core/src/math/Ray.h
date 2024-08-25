#pragma once
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/hash.hpp>
#include "core/GlobalTypes.h"
#include "physics/BoundingBox.h"

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
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (box.GetBound(sign[0]).x - origin.x) * invdir.x;
    tmax = (box.GetBound(1-sign[0]).x - origin.x) * invdir.x;
    tymin = (box.GetBound(sign[1]).y - origin.y) * invdir.y;
    tymax = (box.GetBound(1-sign[1]).y - origin.y) * invdir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return std::make_pair(FLT_MAX, false);

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (box.GetBound(sign[2]).z - origin.z) * invdir.z;
    tzmax = (box.GetBound(1-sign[2]).z - origin.z) * invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return std::make_pair(FLT_MAX, false);

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return std::make_pair(tmin, true);

}

