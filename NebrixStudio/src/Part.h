#pragma once

// headers

#include "glm/glm.hpp"

// utils

#include <string>

enum class PartShape {
    Box,
    Wedge,
    Sphere,
    Cylinder
};

struct Part {
    std::string Name = "Part";
    PartShape   Shape = PartShape::Box;

    // transform

    glm::vec3 Position = { 0.0f, 5.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    // appearance

    glm::vec4 Color = { 0.2f, 0.5f, 1.0f, 1.0f };

    // physics
   
    bool Anchored = false;
    bool CanCollide = true;
    float Mass = 1.0f;

    // physics state

    glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
    bool OnGround = false;
};