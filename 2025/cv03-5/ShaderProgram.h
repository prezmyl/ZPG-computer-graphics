#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
    ShaderProgram() = default;
    explicit ShaderProgram(GLuint id) : id_(id) {}
    void use() const { glUseProgram(id_); }

    // Builder navrh. vzor
    static ShaderProgram fromSources(const char* vsSrc, const char* fsSrc);

    // ukol 2 -> pretizene metody
    void setUniform(const char* name, const glm::mat4& m) const;
    void setUniform(const char* name, const glm::vec3& v) const;
    void setUniform(const char* name, float x) const;
    void setUniform(const char* name, int i) const;

    GLuint id() const { return id_; }
private:
    GLuint id_ = 0;
    GLint loc(const char* n) const { return glGetUniformLocation(id_, n); }
};
