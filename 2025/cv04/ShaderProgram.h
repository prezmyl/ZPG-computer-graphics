#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator=(ShaderProgram&&) noexcept;

    static ShaderProgram fromSources(const char* vs, const char* fs);
    void use() const { glUseProgram(id_); }
    void setUniform(const char* name, const glm::mat4& m) const;
    void setUniform(const char* name, const glm::vec3& v) const;

    void setUniform(const char* name, const glm::vec4& v) const;
    void setUniform(const char* name, float v) const;
    void setUniform(const char* name, int v) const;
    void setUniform(const char* name, bool v) const;

    GLuint id() const { return id_; }

private:
    GLuint id_ = 0;
    Shader vertex_, fragment_; // kompozice
    GLint loc(const char* name) const;
};
