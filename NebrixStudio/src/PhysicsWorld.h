#pragma once

// headers

#include "Part.h"

// utils

#include <vector>

class PhysicsWorld {
public:
    void Step(std::vector<Part>& parts, float dt);

private:
    void ResolveGroundCollision(Part& part);
    void ResolvePartCollision(Part& a, Part& b);
    bool AABBOverlap(const Part& a, const Part& b,
        glm::vec3& outNormal, float& outDepth);

    const float m_Gravity = -35.0f;
    const float m_GroundY = 0.05f;
    const float m_Restitution = 0.3f;
    const float m_Friction = 0.85f;
};