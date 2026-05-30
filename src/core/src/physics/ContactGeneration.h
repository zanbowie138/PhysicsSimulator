#pragma once

#include "../components/Rigidbody.h"
#include "../core/GlobalTypes.h"

#include <optional>
#include <array>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------
// Data structures
// -----------------------------------------------------------------------

struct ContactPoint
{
    glm::vec3 worldPos;   // contact point in world space
    float     depth;      // penetration depth (positive = overlapping)
    float     lambdaN = 0.0f; // accumulated XPBD normal lambda this substep
};

struct ContactManifold
{
    Entity    eA, eB;
    glm::vec3 normal;     // unit vector from A toward B (push-apart direction for A)
    std::array<ContactPoint, 4> points;
    int numPoints = 0;
};

// -----------------------------------------------------------------------
// Contact generation — box-box via OBB SAT + Sutherland-Hodgman clipping
// -----------------------------------------------------------------------

namespace ContactGeneration {
namespace Detail {

// Projected radius of OBB (axes, he) onto a unit axis
inline float GetRadius(const glm::mat3& axes, const glm::vec3& he, const glm::vec3& axis)
{
    return std::abs(glm::dot(axes[0], axis)) * he.x
         + std::abs(glm::dot(axes[1], axis)) * he.y
         + std::abs(glm::dot(axes[2], axis)) * he.z;
}

// Penetration depth along (normalized) axis; negative means separated
inline float SATOverlap(
    const glm::vec3& posA, const glm::mat3& axA, const glm::vec3& heA,
    const glm::vec3& posB, const glm::mat3& axB, const glm::vec3& heB,
    const glm::vec3& axis)
{
    float dist = std::abs(glm::dot(posB - posA, axis));
    return GetRadius(axA, heA, axis) + GetRadius(axB, heB, axis) - dist;
}

// Sutherland-Hodgman: retain vertices where dot(v, planeNormal) <= planeDist
inline std::vector<glm::vec3> ClipPolygon(
    const std::vector<glm::vec3>& poly,
    const glm::vec3& planeNormal, float planeDist)
{
    std::vector<glm::vec3> out;
    if (poly.empty()) return out;

    size_t n = poly.size();
    for (size_t i = 0; i < n; i++)
    {
        const glm::vec3& a = poly[i];
        const glm::vec3& b = poly[(i + 1) % n];

        float da = glm::dot(a, planeNormal) - planeDist;
        float db = glm::dot(b, planeNormal) - planeDist;

        bool aIn = da <= 0.0f;
        bool bIn = db <= 0.0f;

        if (aIn) out.push_back(a);
        if (aIn != bIn) {
            float t = da / (da - db);
            out.push_back(a + t * (b - a));
        }
    }
    return out;
}

// 4 vertices of the face of (center, axes, he) most aligned with dir
inline std::array<glm::vec3, 4> SupportFaceVerts(
    const glm::vec3& center, const glm::mat3& axes, const glm::vec3& he,
    const glm::vec3& dir)
{
    float best = -std::numeric_limits<float>::max();
    int   fa   = 0;
    float fs   = 1.0f;
    for (int i = 0; i < 3; i++) {
        float d = glm::dot(axes[i], dir);
        if (d > best)       { best = d;  fa = i; fs =  1.0f; }
        if (-d > best)      { best = -d; fa = i; fs = -1.0f; }
    }
    glm::vec3 fc = center + fs * axes[fa] * he[fa];
    int u = (fa + 1) % 3, v = (fa + 2) % 3;
    return {
        fc + axes[u]*he[u] + axes[v]*he[v],
        fc - axes[u]*he[u] + axes[v]*he[v],
        fc - axes[u]*he[u] - axes[v]*he[v],
        fc + axes[u]*he[u] - axes[v]*he[v]
    };
}

// Face-face contact generation.
// refCenter/Axes/He: the reference body.
// contactNormal: outward normal of reference face (from reference toward incident).
// refFaceAxis: which axis index (0-2) of the reference body yields the reference face.
inline void FaceFaceContacts(
    const glm::vec3& refCenter, const glm::mat3& refAx, const glm::vec3& refHe,
    const glm::vec3& incCenter, const glm::mat3& incAx, const glm::vec3& incHe,
    const glm::vec3& contactNormal, int refFaceAxis,
    ContactManifold& manifold)
{
    // Plane of the reference face
    float refFaceDist = glm::dot(refCenter, contactNormal) + refHe[refFaceAxis];

    // Incident face vertices (the face of the incident body facing the reference body)
    auto verts4 = SupportFaceVerts(incCenter, incAx, incHe, -contactNormal);

    // Clip against the 4 side planes of the reference face
    int u = (refFaceAxis + 1) % 3, v = (refFaceAxis + 2) % 3;
    std::vector<glm::vec3> poly(verts4.begin(), verts4.end());

    poly = ClipPolygon(poly,  refAx[u],   glm::dot(refCenter,  refAx[u]) + refHe[u]);
    poly = ClipPolygon(poly, -refAx[u],  -glm::dot(refCenter,  refAx[u]) + refHe[u]);
    poly = ClipPolygon(poly,  refAx[v],   glm::dot(refCenter,  refAx[v]) + refHe[v]);
    poly = ClipPolygon(poly, -refAx[v],  -glm::dot(refCenter,  refAx[v]) + refHe[v]);

    // Retain vertices that lie below (inside) the reference face plane
    manifold.numPoints = 0;
    for (const auto& p : poly) {
        float depth = refFaceDist - glm::dot(p, contactNormal);
        if (depth > 0.0f && manifold.numPoints < 4)
            manifold.points[manifold.numPoints++] = { p, depth };
    }
}

// Edge-edge contact (one contact point via closest points on two line segments).
inline void EdgeEdgeContact(
    const glm::vec3& posA, const glm::mat3& axA, const glm::vec3& heA,
    const glm::vec3& posB, const glm::mat3& axB, const glm::vec3& heB,
    int edgeAxisA, int edgeAxisB,
    const glm::vec3& contactNormal, float penetration,
    ContactManifold& manifold)
{
    // Support edge of A along axA[edgeAxisA] in the +contactNormal direction
    int uA = (edgeAxisA + 1) % 3, vA = (edgeAxisA + 2) % 3;
    float sAu = (glm::dot(axA[uA], contactNormal) >= 0.0f) ? 1.0f : -1.0f;
    float sAv = (glm::dot(axA[vA], contactNormal) >= 0.0f) ? 1.0f : -1.0f;
    glm::vec3 pA = posA + sAu * axA[uA] * heA[uA] + sAv * axA[vA] * heA[vA];
    glm::vec3 dA = axA[edgeAxisA];

    // Support edge of B along axB[edgeAxisB] in the -contactNormal direction
    int uB = (edgeAxisB + 1) % 3, vB = (edgeAxisB + 2) % 3;
    float sBu = (glm::dot(axB[uB], contactNormal) <= 0.0f) ? 1.0f : -1.0f;
    float sBv = (glm::dot(axB[vB], contactNormal) <= 0.0f) ? 1.0f : -1.0f;
    glm::vec3 pB = posB + sBu * axB[uB] * heB[uB] + sBv * axB[vB] * heB[vB];
    glm::vec3 dB = axB[edgeAxisB];

    // Closest point between line pA+t*dA and pB+s*dB
    glm::vec3 r = pA - pB;
    float a = glm::dot(dA, dA);
    float e = glm::dot(dB, dB);
    float f = glm::dot(dB, r);
    float c = glm::dot(dA, r);
    float b = glm::dot(dA, dB);
    float denom = a * e - b * b;

    glm::vec3 contactPt;
    if (std::abs(denom) < 1e-6f) {
        contactPt = 0.5f * (pA + pB);
    } else {
        float t = (b * f - c * e) / denom;
        float s = (a * f - b * c) / denom;
        contactPt = 0.5f * ((pA + t * dA) + (pB + s * dB));
    }

    manifold.numPoints = 1;
    manifold.points[0] = { contactPt, penetration };
}

} // namespace Detail

// Box-box contact generation: SAT on 15 axes, then polygon clipping
inline std::optional<ContactManifold> GenerateContacts_BoxBox(
    Entity eA, const Components::Rigidbody& rbA,
    Entity eB, const Components::Rigidbody& rbB)
{
    glm::mat3 axA = glm::mat3_cast(rbA.rotation);
    glm::mat3 axB = glm::mat3_cast(rbB.rotation);
    const glm::vec3& heA = rbA.shape.box.halfExtents;
    const glm::vec3& heB = rbB.shape.box.halfExtents;

    float     minPen     = std::numeric_limits<float>::max();
    glm::vec3 bestNormal;
    int       featureType = 0;  // 0=faceA, 1=faceB, 2=edgeEdge
    int       bestI = 0, bestJ = 0;

    auto test = [&](glm::vec3 axis, int ft, int i, int j) -> bool {
        float len = glm::length(axis);
        if (len < 1e-6f) return true;  // degenerate cross product, skip
        axis /= len;
        float pen = Detail::SATOverlap(rbA.position, axA, heA, rbB.position, axB, heB, axis);
        if (pen < 0.0f) return false;  // separated — no contact
        if (pen < minPen) {
            minPen = pen;
            // Ensure normal points from A toward B
            bestNormal  = (glm::dot(rbB.position - rbA.position, axis) >= 0.0f) ? axis : -axis;
            featureType = ft;
            bestI = i; bestJ = j;
        }
        return true;
    };

    // 3 face normals of A
    for (int i = 0; i < 3; i++) if (!test(axA[i], 0, i, 0)) return std::nullopt;
    // 3 face normals of B
    for (int i = 0; i < 3; i++) if (!test(axB[i], 1, i, 0)) return std::nullopt;
    // 9 edge-edge cross products
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (!test(glm::cross(axA[i], axB[j]), 2, i, j)) return std::nullopt;

    ContactManifold manifold;
    manifold.eA     = eA;
    manifold.eB     = eB;
    manifold.normal = bestNormal;

    if (featureType == 0) {
        // A is the reference face, B is the incident
        Detail::FaceFaceContacts(
            rbA.position, axA, heA,
            rbB.position, axB, heB,
            bestNormal, bestI, manifold);
    } else if (featureType == 1) {
        // B is the reference face, A is the incident.
        // Contact normal from B toward A = -bestNormal (outward from B's reference face).
        Detail::FaceFaceContacts(
            rbB.position, axB, heB,
            rbA.position, axA, heA,
            -bestNormal, bestI, manifold);
    } else {
        // Edge-edge
        Detail::EdgeEdgeContact(
            rbA.position, axA, heA,
            rbB.position, axB, heB,
            bestI, bestJ,
            bestNormal, minPen, manifold);
    }

    if (manifold.numPoints == 0) return std::nullopt;
    return manifold;
}

// Dispatch based on shape types; returns nullopt if no contact or unsupported shape pair
inline std::optional<ContactManifold> GenerateContacts(
    Entity eA, const Components::Rigidbody& rbA,
    Entity eB, const Components::Rigidbody& rbB)
{
    switch (rbA.shape.type) {
        case ShapeType::Box:
            switch (rbB.shape.type) {
                case ShapeType::Box:   return GenerateContacts_BoxBox(eA, rbA, eB, rbB);
                default: break;
            }
            break;
        default: break;
    }
    return std::nullopt;
}

} // namespace ContactGeneration
