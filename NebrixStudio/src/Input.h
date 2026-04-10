#pragma once

// headers

#include "GLFW/glfw3.h"

class Input {
public:
    static void Init(GLFWwindow* window) { s_Window = window; }

    static bool IsKeyDown(int key) {
        return glfwGetKey(s_Window, key) == GLFW_PRESS;
    }

    static bool IsMouseButtonDown(int button) {
        return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
    }

    static void GetMousePos(double& x, double& y) {
        glfwGetCursorPos(s_Window, &x, &y);
    }

private:
    static inline GLFWwindow* s_Window = nullptr;
};