#include "ShaderProgram.h"
#include <vector>
#include <iostream>
#include <utility>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::~ShaderProgram(){
    if(id_) glDeleteProgram(id_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& o) noexcept {
    id_       = o.id_;
    vertex_   = std::move(o.vertex_);
    fragment_ = std::move(o.fragment_);
    o.id_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& o) noexcept {
    if (this != &o) {
        if (id_) glDeleteProgram(id_);
        id_       = o.id_;
        vertex_   = std::move(o.vertex_);
        fragment_ = std::move(o.fragment_);
        o.id_ = 0;
    }
    return *this;
}

ShaderProgram ShaderProgram::fromSources(const char* vsSrc, const char* fsSrc){
    ShaderProgram p;
    p.vertex_   = Shader::fromSource(ShaderType::Vertex,   vsSrc);
    p.fragment_ = Shader::fromSource(ShaderType::Fragment, fsSrc);

    p.id_ = glCreateProgram();
    glAttachShader(p.id_, p.vertex_.id());
    glAttachShader(p.id_, p.fragment_.id());
    glLinkProgram(p.id_);

    GLint ok=GL_FALSE; glGetProgramiv(p.id_, GL_LINK_STATUS, &ok);
    if(!ok){
        GLint n=0; glGetProgramiv(p.id_, GL_INFO_LOG_LENGTH, &n);
        std::vector<char> log(n); glGetProgramInfoLog(p.id_, n, nullptr, log.data());
        std::cerr << "Program link error:\n" << log.data() << "\n";
    }
    return std::move(p); // (RVO/move)
}

GLint ShaderProgram::loc(const char* name) const {
    return glGetUniformLocation(id_, name);
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& m) const {
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const char* name, const glm::vec3& v) const {
    glUniform3fv(loc(name), 1, &v[0]);
}

void ShaderProgram::setUniform(const char* name, const glm::vec4& v) const {
    glUniform4fv(loc(name), 1, &v[0]);
}
void ShaderProgram::setUniform(const char* name, float v) const {
    glUniform1f(loc(name), v);
}
void ShaderProgram::setUniform(const char* name, int v) const {
    glUniform1i(loc(name), v);
}
void ShaderProgram::setUniform(const char* name, bool v) const {
    glUniform1i(loc(name), v ? 1 : 0);
}
