#pragma once

// headers

#include "glm/glm.hpp"

struct GLFWwindow;

class Camera {
public:
    Camera();

    void OnUpdate(GLFWwindow* window, float dt,
        bool viewportHovered, bool rightMouseHeld);

    glm::mat4 GetView()       const { return m_View; }
    glm::mat4 GetProjection() const { return m_Projection; }
    glm::mat4 GetViewProj()   const { return m_Projection * m_View; }
    glm::vec3 GetPosition()   const { return m_Position; }

    void SetViewportSize(float w, float h);

private:
    void UpdateView();

    glm::vec3 m_Position = { 0.0f, 8.0f, 16.0f };
    float     m_Yaw = -90.0f;
    float     m_Pitch = -20.0f;
    float     m_MoveSpeed = 20.0f;
    float     m_LookSpeed = 0.15f;

    double    m_LastMouseX = 0.0;
    double    m_LastMouseY = 0.0;
    bool      m_FirstMouse = true;

    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Projection = glm::mat4(1.0f);

    float m_Width = 1280.0f;
    float m_Height = 720.0f;
};