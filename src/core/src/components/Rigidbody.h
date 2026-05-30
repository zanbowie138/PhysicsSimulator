#pragma once
#include "Collider.h"

namespace Components
{
    struct Rigidbody
    {
        float inverseMass = 1.0f / 100.0f;

        glm::vec3 centroid{};

        // XPBD: current and previous position/rotation for velocity derivation
        glm::vec3 position{};
        glm::vec3 prevPosition{};

        glm::quat rotation    = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::quat prevRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        glm::vec3 linearVelocity{};
        glm::vec3 angularVelocity{};

        glm::vec3 forceAccumulator{};
        glm::vec3 torqueAccumulator{};

        // Half-extents of initial AABB in local space, set once at tree insertion
        glm::vec3 localHalfExtents{};

        CollisionShapeData shape{};

        // Cached world-space inverse inertia tensor — call UpdateWorldInertia() after rotation changes
        glm::mat3 worldInverseInertia{0.0f};

        bool sleeping  = false;
        bool isStatic  = false;

        void SetMass(float mass)   { inverseMass = 1.0f / mass; }
        float GetMass() const      { return (inverseMass > 0.0f) ? 1.0f / inverseMass : 0.0f; }

        void ClearAccumulator()
        {
            forceAccumulator  = glm::vec3(0.0f);
            torqueAccumulator = glm::vec3(0.0f);
        }

        void AddForce(const glm::vec3& force)   { forceAccumulator  += force; }
        void AddTorque(const glm::vec3& torque) { torqueAccumulator += torque; }

        // Recompute worldInverseInertia from current rotation.
        // Static or zero-mass bodies always get zero (infinite inertia).
        void UpdateWorldInertia()
        {
            if (isStatic || inverseMass == 0.0f) {
                worldInverseInertia = glm::mat3(0.0f);
                return;
            }
            glm::mat3 R = glm::mat3_cast(rotation);
            worldInverseInertia = R * shape.localInverseInertia * glm::transpose(R);
        }
    };
}
