// headers

#include "Camera.h"
#include "imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

// utils

#include <algorithm>

Camera::Camera() {
    UpdateView();
}

void Camera::SetViewportSize(float w, float h) {
    m_Width = w;
    m_Height = h;
    m_Projection = glm::perspective(
        glm::radians(60.0f), w / h, 0.1f, 1000.0f);
}

void Camera::OnUpdate(GLFWwindow* window, float dt,
    bool viewportHovered, bool rightMouseHeld) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        m_FirstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    if (viewportHovered && rightMouseHeld) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        if (m_FirstMouse) {
            m_LastMouseX = mx;
            m_LastMouseY = my;
            m_FirstMouse = false;
        }

        float dx = (float)(mx - m_LastMouseX) * m_LookSpeed;
        float dy = (float)(my - m_LastMouseY) * m_LookSpeed;
        m_LastMouseX = mx;
        m_LastMouseY = my;

        m_Yaw += dx;
        m_Pitch -= dy;
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
    }
    else {
        m_FirstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (viewportHovered) {
        glm::vec3 forward;
        forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        forward.y = sin(glm::radians(m_Pitch));
        forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        forward = glm::normalize(forward);

        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::vec3(0, 1, 0);

        float speed = m_MoveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed *= 3.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_Position += forward * speed * dt;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_Position -= forward * speed * dt;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_Position -= right * speed * dt;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_Position += right * speed * dt;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) m_Position += up * speed * dt;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) m_Position -= up * speed * dt;
    }

    UpdateView();
}

void Camera::UpdateView() {
    glm::vec3 forward;
    forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    forward.y = sin(glm::radians(m_Pitch));
    forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    forward = glm::normalize(forward);
    m_View = glm::lookAt(m_Position, m_Position + forward, glm::vec3(0, 1, 0));
}