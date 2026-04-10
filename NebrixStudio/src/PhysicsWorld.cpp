// headers

#include "PhysicsWorld.h"
#include "glm/glm.hpp"

// utils

#include <algorithm>

void PhysicsWorld::Step(std::vector<Part>& parts, float dt) {
    dt = std::min(dt, 0.05f);

    for (auto& part : parts) {
        if (part.Anchored) continue;

        part.Velocity.y += m_Gravity * dt;
        part.Position += part.Velocity * dt;

        if (part.CanCollide)
            ResolveGroundCollision(part);
    }

    for (int i = 0; i < (int)parts.size(); i++) {
        for (int j = i + 1; j < (int)parts.size(); j++) {
            Part& a = parts[i];
            Part& b = parts[j];

            if (!a.CanCollide || !b.CanCollide) continue;

            if (a.Anchored && b.Anchored) continue;

            ResolvePartCollision(a, b);
        }
    }
}

void PhysicsWorld::ResolveGroundCollision(Part& part) {
    float halfY = part.Scale.y * 0.5f;
    float bottom = part.Position.y - halfY;

    if (bottom < m_GroundY) {
        part.Position.y = m_GroundY + halfY;

        if (part.Velocity.y < 0.0f) {
            part.Velocity.y = -part.Velocity.y * m_Restitution;
            if (glm::abs(part.Velocity.y) < 0.5f)
                part.Velocity.y = 0.0f;
        }

        part.Velocity.x *= m_Friction;
        part.Velocity.z *= m_Friction;
        part.OnGround = true;
    }
    else {
        part.OnGround = false;
    }
}

bool PhysicsWorld::AABBOverlap(const Part& a, const Part& b,
    glm::vec3& outNormal, float& outDepth) {
    glm::vec3 aMin = a.Position - a.Scale * 0.5f;
    glm::vec3 aMax = a.Position + a.Scale * 0.5f;
    glm::vec3 bMin = b.Position - b.Scale * 0.5f;
    glm::vec3 bMax = b.Position + b.Scale * 0.5f;

    if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
    if (aMax.y < bMin.y || aMin.y > bMax.y) return false;
    if (aMax.z < bMin.z || aMin.z > bMax.z) return false;

    float dx1 = aMax.x - bMin.x, dx2 = bMax.x - aMin.x;
    float dy1 = aMax.y - bMin.y, dy2 = bMax.y - aMin.y;
    float dz1 = aMax.z - bMin.z, dz2 = bMax.z - aMin.z;

    float ox = std::min(dx1, dx2);
    float oy = std::min(dy1, dy2);
    float oz = std::min(dz1, dz2);

    if (ox < oy && ox < oz) {
        outDepth = ox;
        outNormal = glm::vec3(dx1 < dx2 ? 1.0f : -1.0f, 0, 0);
    }
    else if (oy < ox && oy < oz) {
        outDepth = oy;
        outNormal = glm::vec3(0, dy1 < dy2 ? 1.0f : -1.0f, 0);
    }
    else {
        outDepth = oz;
        outNormal = glm::vec3(0, 0, dz1 < dz2 ? 1.0f : -1.0f);
    }

    return true;
}

void PhysicsWorld::ResolvePartCollision(Part& a, Part& b) {
    glm::vec3 normal;
    float     depth;
    if (!AABBOverlap(a, b, normal, depth)) return;

    if (a.Anchored) {
        b.Position -= normal * depth;
    }
    else if (b.Anchored) {
        a.Position += normal * depth;
    }
    else {
        a.Position += normal * (depth * 0.5f);
        b.Position -= normal * (depth * 0.5f);
    }

    glm::vec3 relVel = a.Velocity - b.Velocity;
    float     velAlongNormal = glm::dot(relVel, normal);
    if (velAlongNormal > 0.0f) return;

    float invMassA = a.Anchored ? 0.0f : 1.0f / a.Mass;
    float invMassB = b.Anchored ? 0.0f : 1.0f / b.Mass;
    float j = -(1.0f + m_Restitution) * velAlongNormal
        / (invMassA + invMassB);

    glm::vec3 impulse = j * normal;
    if (!a.Anchored) a.Velocity += impulse * invMassA;
    if (!b.Anchored) b.Velocity -= impulse * invMassB;
}