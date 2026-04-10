// headers

#include "Shader.h"

// utils

#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertSrc, const char* fragSrc) {
    GLuint vert = CompileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vert);
    glAttachShader(m_ID, frag);
    glLinkProgram(m_ID);

    int success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_ID, 512, nullptr, log);
        printf("ERROR:: FAILED TO LINK SHADER %s\n", log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    glDeleteProgram(m_ID);
}

void Shader::Bind()   const { glUseProgram(m_ID); }
void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetMat4(const char* name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::SetFloat3(const char* name, const glm::vec3& val) const {
    glUniform3fv(glGetUniformLocation(m_ID, name), 1, glm::value_ptr(val));
}
void Shader::SetFloat4(const char* name, const glm::vec4& val) const {
    glUniform4fv(glGetUniformLocation(m_ID, name), 1, glm::value_ptr(val));
}
void Shader::SetInt(const char* name, int val) const {
    glUniform1i(glGetUniformLocation(m_ID, name), val);
}

GLuint Shader::CompileShader(GLenum type, const char* src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        printf("ERROR:: FAILED TO COMPILE SHADER %s\n", log);
    }
    return id;
}