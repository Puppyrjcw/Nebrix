#pragma once

// headers

#include "glad/glad.h"
#include <glm/glm.hpp>
#include "Framebuffer.h"
#include "Shader.h"
#include "Part.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Clear();
    void BeginScene(int vpWidth, int vpHeight,
        const glm::mat4& view, const glm::mat4& proj);
    void EndScene();
    void DrawPart(const Part& part, bool outlined = false);

    Framebuffer* GetFramebuffer() { return m_Framebuffer; }
    const glm::mat4& GetView()        const { return m_View; }
    const glm::mat4& GetProjection()  const { return m_Proj; }

    struct Mesh { GLuint VAO = 0, VBO = 0, IBO = 0; GLuint IndexCount = 0; };
    Mesh m_Cube, m_Sphere, m_Cylinder, m_Wedge;

private:
    void   BuildCube();
    void   BuildSphere(int stacks, int slices);
    void   BuildCylinder(int slices);
    void   BuildWedge();
    GLuint GetVAO(PartShape shape);
    GLuint GetIndexCount(PartShape shape);

    Framebuffer* m_Framebuffer = nullptr;
    Shader* m_Shader = nullptr;
    Shader* m_OutlineShader = nullptr;

    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Proj = glm::mat4(1.0f);
    glm::mat4 m_ViewProj = glm::mat4(1.0f);
};