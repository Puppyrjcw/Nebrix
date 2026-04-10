// headers

#include "Application.h"
#include "UI/UILayer.h"
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

Application::Application(const std::string& title, int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    m_Renderer = new Renderer();
    m_Scene = new Scene();
    m_Camera = new Camera();
    m_UI = new UILayer();
    m_UI->Init(m_Window);
}

Application::~Application() {
    m_UI->Shutdown();
    delete m_UI;
    delete m_Camera;
    delete m_Renderer;
    delete m_Scene;
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Application::Run() {
    while (!glfwWindowShouldClose(m_Window)) {
        glfwPollEvents();

        float time = (float)glfwGetTime();
        float dt = time - m_LastTime;
        m_LastTime = time;

        m_Scene->Update(dt);
        m_Renderer->Clear();

        m_UI->Begin();
        m_UI->DrawPanels(m_Scene, m_Renderer, m_Camera, m_Window, dt);
        m_UI->End();

        glfwSwapBuffers(m_Window);
    }
}