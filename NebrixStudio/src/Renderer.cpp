// headers

#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// utils

#include <vector>
#include <cmath>
#include <stdio.h>

using namespace glm;
using namespace std;

static const float PI = 3.14159265358979323846f;

// nebrix shaders

static const char* VertSrc = R"(
#version 460 core
layout(location=0) in vec3 a_Position;
layout(location=1) in vec3 a_Normal;
uniform mat4 u_ViewProj;
uniform mat4 u_Transform;
out vec3 v_Normal;
out vec3 v_FragPos;
void main(){
    vec4 world  = u_Transform * vec4(a_Position,1.0);
    v_FragPos   = world.xyz;
    v_Normal    = mat3(transpose(inverse(u_Transform))) * a_Normal;
    gl_Position = u_ViewProj * world;
}
)";

static const char* FragSrc = R"(
#version 460 core
in vec3 v_Normal;
in vec3 v_FragPos;
uniform vec4 u_Color;
out vec4 FragColor;
void main(){
    vec3  light   = normalize(vec3(1.0,2.0,1.5));
    float diff    = max(dot(normalize(v_Normal), light), 0.0);
    float ambient = 0.25;
    float lit     = ambient + diff * 0.75;
    FragColor     = vec4(u_Color.rgb * lit, u_Color.a);
}
)";

static const char* OutlineVertSrc = R"(
#version 460 core
layout(location=0) in vec3 a_Position;
layout(location=1) in vec3 a_Normal;
uniform mat4 u_ViewProj;
uniform mat4 u_Transform;
void main(){
    vec3 pos    = a_Position + a_Normal * 0.04;
    gl_Position = u_ViewProj * u_Transform * vec4(pos, 1.0);
}
)";

static const char* OutlineFragSrc = R"(
#version 460 core
out vec4 FragColor;
void main(){
    FragColor = vec4(0.18, 0.53, 1.0, 1.0);
}
)";

static void UploadMesh(Renderer::Mesh& m,
    const vector<float>& verts,
    const vector<unsigned int>& idx) {
    m.IndexCount = (GLuint)idx.size();
	glGenVertexArrays(1, &m.VAO);
	glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.IBO);
    glBindVertexArray(m.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void Renderer::BuildCube() {
    std::vector<float> v = {
        -0.5f,-0.5f, 0.5f, 0,0,1,   0.5f,-0.5f, 0.5f, 0,0,1,
         0.5f, 0.5f, 0.5f, 0,0,1,  -0.5f, 0.5f, 0.5f, 0,0,1,
        -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,
         0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f, 0.5f,-0.5f, 0,0,-1,
        -0.5f,-0.5f,-0.5f,-1,0,0,  -0.5f,-0.5f, 0.5f,-1,0,0,
        -0.5f, 0.5f, 0.5f,-1,0,0,  -0.5f, 0.5f,-0.5f,-1,0,0,
         0.5f,-0.5f,-0.5f, 1,0,0,   0.5f,-0.5f, 0.5f, 1,0,0,
         0.5f, 0.5f, 0.5f, 1,0,0,   0.5f, 0.5f,-0.5f, 1,0,0,
        -0.5f, 0.5f, 0.5f, 0,1,0,   0.5f, 0.5f, 0.5f, 0,1,0,
         0.5f, 0.5f,-0.5f, 0,1,0,  -0.5f, 0.5f,-0.5f, 0,1,0,
        -0.5f,-0.5f, 0.5f, 0,-1,0,  0.5f,-0.5f, 0.5f, 0,-1,0,
         0.5f,-0.5f,-0.5f, 0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,
    };
    std::vector<unsigned int> idx = {
        0,1,2,2,3,0,   4,6,5,6,4,7,
        8,9,10,10,11,8, 12,14,13,14,12,15,
        16,17,18,18,19,16, 20,22,21,22,20,23
    };
    UploadMesh(m_Cube, v, idx);
}

void Renderer::BuildSphere(int stacks, int slices) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    for (int i = 0; i <= stacks; i++) {
        float phi = PI * ((float)i / stacks);
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * PI * ((float)j / slices);
            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);
            v.insert(v.end(), { x * 0.5f, y * 0.5f, z * 0.5f, x, y, z });
        }
    }
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int a = i * (slices + 1) + j, b = a + slices + 1;
            idx.insert(idx.end(), {
                (unsigned)a,(unsigned)b,(unsigned)(a + 1),
                (unsigned)(a + 1),(unsigned)b,(unsigned)(b + 1)
                });
        }
    }
    UploadMesh(m_Sphere, v, idx);
}

void Renderer::BuildCylinder(int slices) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    for (int i = 0; i <= slices; i++) {
        float theta = 2.0f * PI * ((float)i / slices);
        float x = cosf(theta), z = sinf(theta);
        v.insert(v.end(), { x * 0.5f,-0.5f,z * 0.5f, x,0,z });
        v.insert(v.end(), { x * 0.5f, 0.5f,z * 0.5f, x,0,z });
    }
    for (int i = 0; i < slices; i++) {
        unsigned a = i * 2, b = a + 1, c = a + 2, d = a + 3;
        idx.insert(idx.end(), { a,b,c, b,d,c });
    }
    unsigned base = (unsigned)v.size() / 6;
    v.insert(v.end(), { 0,-0.5f,0, 0,-1,0 });
    unsigned center = base++;
    for (int i = 0; i < slices; i++) {
        float t0 = 2 * PI * ((float)i / slices), t1 = 2 * PI * ((float)(i + 1) / slices);
        v.insert(v.end(), { cosf(t0) * 0.5f,-0.5f,sinf(t0) * 0.5f, 0,-1,0 });
        v.insert(v.end(), { cosf(t1) * 0.5f,-0.5f,sinf(t1) * 0.5f, 0,-1,0 });
        idx.insert(idx.end(), { center, base + (unsigned)i * 2 + 1, base + (unsigned)i * 2 });
    }
    base = (unsigned)v.size() / 6;
    v.insert(v.end(), { 0,0.5f,0, 0,1,0 });
    center = base++;
    for (int i = 0; i < slices; i++) {
        float t0 = 2 * PI * ((float)i / slices), t1 = 2 * PI * ((float)(i + 1) / slices);
        v.insert(v.end(), { cosf(t0) * 0.5f,0.5f,sinf(t0) * 0.5f, 0,1,0 });
        v.insert(v.end(), { cosf(t1) * 0.5f,0.5f,sinf(t1) * 0.5f, 0,1,0 });
        idx.insert(idx.end(), { center, base + (unsigned)i * 2, base + (unsigned)i * 2 + 1 });
    }
    UploadMesh(m_Cylinder, v, idx);
}

void Renderer::BuildWedge() {
    std::vector<float> v = {
        -0.5f,-0.5f,-0.5f, 0,-1,0,
         0.5f,-0.5f,-0.5f, 0,-1,0,
         0.5f,-0.5f, 0.5f, 0,-1,0,
        -0.5f,-0.5f, 0.5f, 0,-1,0,
        -0.5f,-0.5f,-0.5f, 0,0,-1,
         0.5f,-0.5f,-0.5f, 0,0,-1,
         0.5f, 0.5f,-0.5f, 0,0,-1,
        -0.5f, 0.5f,-0.5f, 0,0,-1,
        -0.5f,-0.5f,-0.5f,-1,0,0,
        -0.5f,-0.5f, 0.5f,-1,0,0,
        -0.5f, 0.5f,-0.5f,-1,0,0,
         0.5f,-0.5f,-0.5f, 1,0,0,
         0.5f,-0.5f, 0.5f, 1,0,0,
         0.5f, 0.5f,-0.5f, 1,0,0,
        -0.5f,-0.5f, 0.5f, 0,0.707f,0.707f,
         0.5f,-0.5f, 0.5f, 0,0.707f,0.707f,
         0.5f, 0.5f,-0.5f, 0,0.707f,0.707f,
        -0.5f, 0.5f,-0.5f, 0,0.707f,0.707f,
    };
    std::vector<unsigned int> idx = {
        0,2,1, 2,0,3,
        4,5,6, 6,7,4,
        8,9,10,
        11,13,12,
        14,15,16, 16,17,14
    };
    UploadMesh(m_Wedge, v, idx);
}

Renderer::Renderer() {
    m_Framebuffer = new Framebuffer(1280, 720);
    m_Shader = new Shader(VertSrc, FragSrc);
    m_OutlineShader = new Shader(OutlineVertSrc, OutlineFragSrc);
    BuildCube();
    BuildSphere(24, 24);
    BuildCylinder(32);
    BuildWedge();
}

Renderer::~Renderer() {
    delete m_Framebuffer;
    delete m_Shader;
    delete m_OutlineShader;
    auto Del = [](Mesh& m) {
        glDeleteVertexArrays(1, &m.VAO);
        glDeleteBuffers(1, &m.VBO);
        glDeleteBuffers(1, &m.IBO);
        };
    Del(m_Cube); Del(m_Sphere); Del(m_Cylinder); Del(m_Wedge);
}

void Renderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::BeginScene(int w, int h,
    const glm::mat4& view, const glm::mat4& proj) {
    m_View = view;
    m_Proj = proj;
    m_ViewProj = proj * view;
    m_Framebuffer->Resize(w, h);
    m_Framebuffer->Bind();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glClearColor(0.18f, 0.18f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::EndScene() {
    glDisable(GL_STENCIL_TEST);
    m_Framebuffer->Unbind();
}

GLuint Renderer::GetVAO(PartShape shape) {
    switch (shape) {
    case PartShape::Sphere:   return m_Sphere.VAO;
    case PartShape::Cylinder: return m_Cylinder.VAO;
    case PartShape::Wedge:    return m_Wedge.VAO;
    default:                  return m_Cube.VAO;
    }
}

GLuint Renderer::GetIndexCount(PartShape shape) {
    switch (shape) {
    case PartShape::Sphere:   return m_Sphere.IndexCount;
    case PartShape::Cylinder: return m_Cylinder.IndexCount;
    case PartShape::Wedge:    return m_Wedge.IndexCount;
    default:                  return m_Cube.IndexCount;
    }
}

void Renderer::DrawPart(const Part& part, bool outlined) {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), part.Position);
    t = glm::rotate(t, glm::radians(part.Rotation.y), { 0,1,0 });
    t = glm::rotate(t, glm::radians(part.Rotation.x), { 1,0,0 });
    t = glm::rotate(t, glm::radians(part.Rotation.z), { 0,0,1 });
    t = glm::scale(t, part.Scale);

    GLuint vao = GetVAO(part.Shape);
    GLuint cnt = GetIndexCount(part.Shape);

    if (outlined) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
    }
    else {
        glStencilMask(0x00);
    }

    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProj", m_ViewProj);
    m_Shader->SetMat4("u_Transform", t);
    m_Shader->SetFloat4("u_Color", part.Color);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    m_Shader->Unbind();

    if (outlined) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        m_OutlineShader->Bind();
        m_OutlineShader->SetMat4("u_ViewProj", m_ViewProj);
        m_OutlineShader->SetMat4("u_Transform", t);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        m_OutlineShader->Unbind();

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }
}