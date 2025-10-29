#include "ShaderProgram.h"
#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


static GLuint compile(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = GL_FALSE; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){
        GLint n=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &n);
        std::vector<char> log(n); glGetShaderInfoLog(s, n, nullptr, log.data());
        std::cerr << (type==GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                  << " shader compile error:\n" << log.data() << "\n";
    }
    return s;
}

ShaderProgram ShaderProgram::fromSources(const char* vsSrc, const char* fsSrc){
    GLuint vs = compile(GL_VERTEX_SHADER,   vsSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);

    GLuint pid = glCreateProgram();
    glAttachShader(pid, vs);
    glAttachShader(pid, fs);
    glLinkProgram(pid);

    GLint ok = GL_FALSE; glGetProgramiv(pid, GL_LINK_STATUS, &ok);
    if(!ok){
        GLint n=0; glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &n);
        std::vector<char> log(n); glGetProgramInfoLog(pid, n, nullptr, log.data());
        std::cerr << "Program link error:\n" << log.data() << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return ShaderProgram(pid);
}


void ShaderProgram::setUniform(const char* name, const glm::mat4& m) const {
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, glm::value_ptr(m));
}
void ShaderProgram::setUniform(const char* name, const glm::vec3& v) const {
    glUniform3fv(loc(name), 1, &v[0]);
}
void ShaderProgram::setUniform(const char* name, float x) const {
    glUniform1f(loc(name), x);
}
void ShaderProgram::setUniform(const char* name, int i) const {
    glUniform1i(loc(name), i);
}
