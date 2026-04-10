#pragma once

// utils

#include <string>

struct GLFWwindow;
class UILayer;
class Renderer;
class Scene;
class Camera;

class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();
    void Run();

private:
    GLFWwindow* m_Window = nullptr;
    UILayer* m_UI = nullptr;
    Renderer* m_Renderer = nullptr;
    Scene* m_Scene = nullptr;
    Camera* m_Camera = nullptr;
    float       m_LastTime = 0.0f;
};