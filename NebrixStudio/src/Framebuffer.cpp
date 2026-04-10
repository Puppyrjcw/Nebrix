// headers

#include "Framebuffer.h"

// utils

#include <stdio.h>

Framebuffer::Framebuffer(int width, int height)
    : m_Width(width), m_Height(height) {
    Create();
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_TextureID);
    glDeleteRenderbuffers(1, &m_RBO);
}

void Framebuffer::Create() {
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height,
        0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, m_TextureID, 0);

    // depth & stencil together

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR:: FRAME BUFFER NOT COMPLETE\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_Width, m_Height);
}

void Framebuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int width, int height) {
    if (width == m_Width && height == m_Height) return;
    m_Width = width;
    m_Height = height;
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_TextureID);
    glDeleteRenderbuffers(1, &m_RBO);
    Create();
}