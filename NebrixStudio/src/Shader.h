#pragma once
// headers

#include "glad/glad.h"
#include "glm/glm.hpp"

// utils

#include <string>

class Shader {
public:
    Shader(const char* vertSrc, const char* fragSrc);
    ~Shader();

    GLuint GetID() const { return m_ID; }

    void Bind() const;
    void Unbind() const;

    void SetMat4(const char* name, const glm::mat4& mat) const;
    void SetFloat3(const char* name, const glm::vec3& val) const;
    void SetFloat4(const char* name, const glm::vec4& val) const;
    void SetInt(const char* name, int val) const;

private:
    GLuint m_ID = 0;
    GLuint CompileShader(GLenum type, const char* src);
};