#pragma once

enum class ShapeType { None, Box };

struct CollisionShapeData
{
    ShapeType type = ShapeType::None;
    glm::mat3 localInverseInertia{0.0f};

    union {
        struct { glm::vec3 halfExtents; } box;
        // struct { float radius; } sphere;  // future shapes go here
    };

    // Factory: solid-box inertia tensor for half-extents (hx, hy, hz) and mass.
    // Pass mass=0 for static bodies (produces zero inverse inertia, handled by UpdateWorldInertia).
    static CollisionShapeData MakeBox(const glm::vec3& he, float mass)
    {
        CollisionShapeData s;
        s.type = ShapeType::Box;
        s.box.halfExtents = he;

        if (mass <= 0.0f) {
            s.localInverseInertia = glm::mat3(0.0f);
            return s;
        }

        float hx = he.x, hy = he.y, hz = he.z;
        glm::mat3 I(0.0f);
        I[0][0] = mass / 3.0f * (hy * hy + hz * hz);
        I[1][1] = mass / 3.0f * (hx * hx + hz * hz);
        I[2][2] = mass / 3.0f * (hx * hx + hy * hy);
        s.localInverseInertia = glm::inverse(I);
        return s;
    }
};
