#pragma once

// headers

#include "glad/glad.h"

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Resize(int width, int height);

    GLuint GetTextureID() const { return m_TextureID; }
    int GetWidth()  const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    void Create();

    GLuint m_FBO = 0;
    GLuint m_TextureID = 0;
    GLuint m_RBO = 0;
    int    m_Width = 0;
    int    m_Height = 0;
};