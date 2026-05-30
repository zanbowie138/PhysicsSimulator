#pragma once

#include "DynamicTree.h"
#include "ContactGeneration.h"

#include "../core/World.h"

#include "../renderables/Mesh.h"
#include "../renderables/Model.h"

#define GRAVITY         -9.81f
#define SUBSTEPS        8      // XPBD substeps per frame; more substeps = more stable
#define FRICTION_STATIC  0.6f  // static friction coefficient (Coulomb cone)
#define FRICTION_DYNAMIC 0.4f  // kinetic friction coefficient (sliding)

// https://matthias-research.github.io/pages/publications/PBDBodies.pdf
class PhysicsSystem : public System
{
public:
    Physics::DynamicBBTree tree;

    PhysicsSystem();

    void AddRigidbody(Mesh& object);
    void AddRigidbody(Model& object);

    void AddToTree(Mesh& object);
    void AddToTree(Model& object);

    void Update(float dt);

    void Clean() override;

private:
    // Arvo's method: AABB wrapping an OBB (center, local half-extents, rotation)
    static BoundingBox MakeWorldAABB(const glm::vec3& center,
                                     const glm::vec3& localHE,
                                     const glm::mat3& rot)
    {
        glm::vec3 worldHE = glm::abs(rot[0]) * localHE.x
                          + glm::abs(rot[1]) * localHE.y
                          + glm::abs(rot[2]) * localHE.z;
        return BoundingBox(center - worldHE, center + worldHE);
    }

    // Apply impulse j at point r relative to body center (updates linear + angular velocity)
    static void ApplyImpulse(Components::Rigidbody* rb, bool isStatic,
                             const glm::vec3& j, const glm::vec3& r)
    {
        if (!rb || isStatic) return;
        rb->linearVelocity  += rb->inverseMass * j;
        rb->angularVelocity += rb->worldInverseInertia * glm::cross(r, j);
    }

    // XPBD per-substep: predict → solve constraints → derive velocities → velocity corrections → damp
    void SubStep(float h);

    // Phase 2: OBB-OBB narrow phase + XPBD position/orientation constraints
    std::vector<ContactManifold> SolvePositionConstraints();

    // Phase 4: velocity-level restitution + Coulomb friction (with angular contributions)
    void ApplyVelocityCorrections(
        const std::vector<ContactManifold>&        manifolds,
        const std::unordered_map<Entity, glm::vec3>& prevLinVel,
        const std::unordered_map<Entity, glm::vec3>& prevAngVel,
        float h);
};

inline PhysicsSystem::PhysicsSystem()
{
    tree = Physics::DynamicBBTree{ 1 };
}

inline void PhysicsSystem::AddRigidbody(Mesh& object)
{
    Components::Rigidbody rb{};
    rb.position = object.transform.worldPos;
    rb.rotation = object.transform.rotation;
    world.AddComponent(object.mEntityID, rb);
    AddToTree(object);
}

inline void PhysicsSystem::AddRigidbody(Model& object)
{
    Components::Rigidbody rb{};
    rb.position = object.transform.worldPos;
    rb.rotation = object.transform.rotation;
    world.AddComponent(object.mEntityID, rb);
    AddToTree(object);
}

inline void PhysicsSystem::AddToTree(Mesh& object)
{
    LOG(LOG_INFO) << "Adding mesh with entity ID " << object.mEntityID << " to tree\n";
    auto box = object.CalcBoundingBox();
    tree.InsertEntity(object.mEntityID, box);
    auto& rb = world.GetComponent<Components::Rigidbody>(object.mEntityID);
    rb.localHalfExtents = (box.max - box.min) * 0.5f;
    rb.shape = CollisionShapeData::MakeBox(rb.localHalfExtents, rb.GetMass());
    rb.UpdateWorldInertia();
}

inline void PhysicsSystem::AddToTree(Model& object)
{
    LOG(LOG_INFO) << "Adding model with entity ID " << object.mEntityID << " to tree\n";
    auto box = object.CalcBoundingBox();
    tree.InsertEntity(object.mEntityID, box);
    auto& rb = world.GetComponent<Components::Rigidbody>(object.mEntityID);
    rb.localHalfExtents = (box.max - box.min) * 0.5f;
    rb.shape = CollisionShapeData::MakeBox(rb.localHalfExtents, rb.GetMass());
    rb.UpdateWorldInertia();
}

inline void PhysicsSystem::Update(float dt)
{
    const float h = dt / static_cast<float>(SUBSTEPS);
    for (int i = 0; i < SUBSTEPS; ++i)
        SubStep(h);
}

inline void PhysicsSystem::Clean() {}

inline void PhysicsSystem::SubStep(float h)
{
    // --- Phase 1: Predict ---
    std::unordered_map<Entity, glm::vec3> prevLinVel;
    std::unordered_map<Entity, glm::vec3> prevAngVel;
    prevLinVel.reserve(mEntities.size());
    prevAngVel.reserve(mEntities.size());

    for (const auto entity : mEntities)
    {
        auto& rb = world.GetComponent<Components::Rigidbody>(entity);
        if (rb.isStatic) { rb.ClearAccumulator(); continue; }

        prevLinVel[entity] = rb.linearVelocity;
        prevAngVel[entity] = rb.angularVelocity;

        // Integrate external forces into linear velocity
        glm::vec3 accel = rb.forceAccumulator * rb.inverseMass + glm::vec3(0.0f, GRAVITY, 0.0f);
        rb.linearVelocity += accel * h;

        // Integrate torque into angular velocity (world-frame)
        rb.angularVelocity += rb.worldInverseInertia * rb.torqueAccumulator * h;

        rb.ClearAccumulator();

        // Predict position
        rb.prevPosition = rb.position;
        rb.position    += rb.linearVelocity * h;

        // Predict orientation via quaternion integration (world-frame omega)
        rb.prevRotation = rb.rotation;
        glm::quat omegaQ(0.0f, rb.angularVelocity.x, rb.angularVelocity.y, rb.angularVelocity.z);
        rb.rotation = glm::normalize(rb.rotation + (h * 0.5f) * omegaQ * rb.rotation);

        // Cache world-space inverse inertia for the predicted rotation
        rb.UpdateWorldInertia();

        // Sync transform + tree
        auto& tf  = world.GetComponent<Components::Transform>(entity);
        tf.worldPos = rb.position;
        tf.rotation = rb.rotation;
        tree.UpdateEntity(entity, MakeWorldAABB(rb.position, rb.localHalfExtents, glm::mat3_cast(rb.rotation)));
    }

    // --- Phase 2: Solve position + orientation constraints ---
    const auto manifolds = SolvePositionConstraints();

    // --- Phase 3: Derive velocities from position/rotation deltas ---
    for (const auto entity : mEntities)
    {
        auto& rb = world.GetComponent<Components::Rigidbody>(entity);
        if (rb.isStatic) continue;

        rb.linearVelocity = (rb.position - rb.prevPosition) / h;

        // Angular velocity from quaternion delta: dq = q * conj(q_prev)
        glm::quat dq = rb.rotation * glm::conjugate(rb.prevRotation);
        if (dq.w < 0.0f) dq = -dq;  // shortest arc
        rb.angularVelocity = (2.0f / h) * glm::vec3(dq.x, dq.y, dq.z);
    }

    // --- Phase 4: Velocity-level restitution + friction ---
    ApplyVelocityCorrections(manifolds, prevLinVel, prevAngVel, h);

    // --- Phase 5: Damping ---
    // pow(0.9, h)^SUBSTEPS == pow(0.9, dt): identical per-frame damping regardless of substep count
    for (const auto entity : mEntities)
    {
        auto& rb = world.GetComponent<Components::Rigidbody>(entity);
        if (rb.isStatic) continue;

        float damp = static_cast<float>(std::pow(0.9, h));
        rb.linearVelocity  *= damp;
        rb.angularVelocity *= damp;
    }
}

inline std::vector<ContactManifold> PhysicsSystem::SolvePositionConstraints()
{
    std::vector<ContactManifold> manifolds;
    const auto pairs = tree.ComputeCollisionPairs();

    for (size_t i = 0; i + 1 < pairs.size(); i += 2)
    {
        Entity eA = pairs[i];
        Entity eB = pairs[i + 1];

        bool hasRbA = mEntities.count(eA) > 0;
        bool hasRbB = mEntities.count(eB) > 0;
        if (!hasRbA || !hasRbB) continue;  // both need Rigidbody for angular dynamics

        Components::Rigidbody& rbA = world.GetComponent<Components::Rigidbody>(eA);
        Components::Rigidbody& rbB = world.GetComponent<Components::Rigidbody>(eB);

        bool staticA = rbA.isStatic;
        bool staticB = rbB.isStatic;
        if (staticA && staticB) continue;

        // Skip entities without a collision shape (not yet initialized)
        if (rbA.shape.type == ShapeType::None || rbB.shape.type == ShapeType::None) continue;

        auto maybeManifold = ContactGeneration::GenerateContacts(eA, rbA, eB, rbB);
        if (!maybeManifold) continue;

        ContactManifold& m = *maybeManifold;

        for (int k = 0; k < m.numPoints; k++)
        {
            ContactPoint& cp = m.points[k];

            glm::vec3 rA = cp.worldPos - rbA.position;
            glm::vec3 rB = cp.worldPos - rbB.position;

            // Generalized inverse mass: m^-1 + (r × n)^T I^-1 (r × n)
            float wA = 0.0f, wB = 0.0f;
            if (!staticA) {
                glm::vec3 rAxN = glm::cross(rA, m.normal);
                wA = rbA.inverseMass + glm::dot(rAxN, rbA.worldInverseInertia * rAxN);
            }
            if (!staticB) {
                glm::vec3 rBxN = glm::cross(rB, m.normal);
                wB = rbB.inverseMass + glm::dot(rBxN, rbB.worldInverseInertia * rBxN);
            }

            float wSum = wA + wB;
            if (wSum < 1e-9f) continue;

            float deltaLambda = cp.depth / wSum;
            cp.lambdaN += deltaLambda;

            // Correction impulse (normal points from A to B, so A gets -p, B gets +p)
            glm::vec3 p = deltaLambda * m.normal;

            // Position corrections
            if (!staticA) rbA.position -= rbA.inverseMass * p;
            if (!staticB) rbB.position += rbB.inverseMass * p;

            // Orientation corrections: δq = 0.5 * [I^-1 (r × ±p), 0] * q
            if (!staticA) {
                glm::vec3 ang = rbA.worldInverseInertia * glm::cross(rA, -p);
                glm::quat dq(0.0f, ang.x, ang.y, ang.z);
                rbA.rotation = glm::normalize(rbA.rotation + 0.5f * dq * rbA.rotation);
                rbA.UpdateWorldInertia();
            }
            if (!staticB) {
                glm::vec3 ang = rbB.worldInverseInertia * glm::cross(rB, p);
                glm::quat dq(0.0f, ang.x, ang.y, ang.z);
                rbB.rotation = glm::normalize(rbB.rotation + 0.5f * dq * rbB.rotation);
                rbB.UpdateWorldInertia();
            }

            // Sync transforms + tree after each correction
            if (!staticA) {
                auto& tf = world.GetComponent<Components::Transform>(eA);
                tf.worldPos = rbA.position;
                tf.rotation = rbA.rotation;
                tree.UpdateEntity(eA, MakeWorldAABB(rbA.position, rbA.localHalfExtents, glm::mat3_cast(rbA.rotation)));
            }
            if (!staticB) {
                auto& tf = world.GetComponent<Components::Transform>(eB);
                tf.worldPos = rbB.position;
                tf.rotation = rbB.rotation;
                tree.UpdateEntity(eB, MakeWorldAABB(rbB.position, rbB.localHalfExtents, glm::mat3_cast(rbB.rotation)));
            }
        }

        manifolds.push_back(std::move(m));
    }

    return manifolds;
}

inline void PhysicsSystem::ApplyVelocityCorrections(
    const std::vector<ContactManifold>&        manifolds,
    const std::unordered_map<Entity, glm::vec3>& prevLinVel,
    const std::unordered_map<Entity, glm::vec3>& prevAngVel,
    float h)
{
    constexpr float restitution = 0.3f;

    for (const auto& m : manifolds)
    {
        bool hasRbA = mEntities.count(m.eA) > 0;
        bool hasRbB = mEntities.count(m.eB) > 0;

        Components::Rigidbody* rbA = hasRbA ? &world.GetComponent<Components::Rigidbody>(m.eA) : nullptr;
        Components::Rigidbody* rbB = hasRbB ? &world.GetComponent<Components::Rigidbody>(m.eB) : nullptr;

        bool staticA = !rbA || rbA->isStatic;
        bool staticB = !rbB || rbB->isStatic;

        for (int k = 0; k < m.numPoints; k++)
        {
            const ContactPoint& cp = m.points[k];

            glm::vec3 rA = cp.worldPos - (rbA ? rbA->position : glm::vec3(0.0f));
            glm::vec3 rB = cp.worldPos - (rbB ? rbB->position : glm::vec3(0.0f));

            // Velocity at contact point (linear + angular contribution)
            glm::vec3 vA(0.0f), vB(0.0f);
            if (rbA && !staticA) vA = rbA->linearVelocity + glm::cross(rbA->angularVelocity, rA);
            if (rbB && !staticB) vB = rbB->linearVelocity + glm::cross(rbB->angularVelocity, rB);
            glm::vec3 vRel  = vB - vA;
            float     vRelN = glm::dot(vRel, m.normal);

            // Generalized inverse mass at this contact point (normal direction)
            float wA = staticA ? 0.0f : rbA->inverseMass;
            float wB = staticB ? 0.0f : rbB->inverseMass;
            if (rbA && !staticA) {
                glm::vec3 rAxN = glm::cross(rA, m.normal);
                wA += glm::dot(rAxN, rbA->worldInverseInertia * rAxN);
            }
            if (rbB && !staticB) {
                glm::vec3 rBxN = glm::cross(rB, m.normal);
                wB += glm::dot(rBxN, rbB->worldInverseInertia * rBxN);
            }
            float wSumN = wA + wB;
            if (wSumN < 1e-9f) continue;

            // --- Normal restitution ---
            if (vRelN < 0.0f)
            {
                auto findPrev = [](const std::unordered_map<Entity, glm::vec3>& m, Entity e) -> glm::vec3 {
                    auto it = m.find(e);
                    return (it != m.end()) ? it->second : glm::vec3(0.0f);
                };

                glm::vec3 prevVA = findPrev(prevLinVel, m.eA);
                glm::vec3 prevVB = findPrev(prevLinVel, m.eB);
                glm::vec3 prevOA = findPrev(prevAngVel, m.eA);
                glm::vec3 prevOB = findPrev(prevAngVel, m.eB);

                glm::vec3 prevVA_p = prevVA + glm::cross(prevOA, rA);
                glm::vec3 prevVB_p = prevVB + glm::cross(prevOB, rB);
                float vRelNOld = glm::dot(prevVB_p - prevVA_p, m.normal);

                float target = std::max(-restitution * vRelNOld, 0.0f);
                if (target > -vRelN)
                {
                    float deltaV     = (target - vRelN) / wSumN;
                    glm::vec3 impulse = deltaV * m.normal;

                    ApplyImpulse(rbA, staticA, -impulse, rA);
                    ApplyImpulse(rbB, staticB,  impulse, rB);

                    // Recompute vRel after impulse
                    vA    = rbA && !staticA ? rbA->linearVelocity + glm::cross(rbA->angularVelocity, rA) : glm::vec3(0.0f);
                    vB    = rbB && !staticB ? rbB->linearVelocity + glm::cross(rbB->angularVelocity, rB) : glm::vec3(0.0f);
                    vRel  = vB - vA;
                }
            }

            // --- Tangential friction (Coulomb) ---
            glm::vec3 vRelT = vRel - glm::dot(vRel, m.normal) * m.normal;
            float     vT    = glm::length(vRelT);
            if (vT < 1e-6f) continue;

            glm::vec3 tangent = vRelT / vT;

            // Generalized inverse mass in tangent direction
            float wAT = staticA ? 0.0f : (rbA ? rbA->inverseMass : 0.0f);
            float wBT = staticB ? 0.0f : (rbB ? rbB->inverseMass : 0.0f);
            if (rbA && !staticA) {
                glm::vec3 rAxT = glm::cross(rA, tangent);
                wAT += glm::dot(rAxT, rbA->worldInverseInertia * rAxT);
            }
            if (rbB && !staticB) {
                glm::vec3 rBxT = glm::cross(rB, tangent);
                wBT += glm::dot(rBxT, rbB->worldInverseInertia * rBxT);
            }
            float wSumT = wAT + wBT;
            if (wSumT < 1e-9f) continue;

            // Normal impulse equivalent from position constraint (for Coulomb cone)
            float j_n = cp.lambdaN / h;

            float impulseScalar = -vT / wSumT;
            if (std::abs(impulseScalar) > FRICTION_STATIC * j_n)
                impulseScalar = -FRICTION_DYNAMIC * j_n;

            glm::vec3 frictionImpulse = impulseScalar * tangent;
            // Friction opposes relative tangential motion:
            // A gets +frictionImpulse direction push, B gets -frictionImpulse direction push
            ApplyImpulse(rbA, staticA, -frictionImpulse, rA);
            ApplyImpulse(rbB, staticB,  frictionImpulse, rB);
        }
    }
}
