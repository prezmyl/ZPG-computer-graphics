#include "Shader.h"
#include <vector>
#include <iostream>

static GLuint compile(GLenum glType, const char* src){
    GLuint s = glCreateShader(glType);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = GL_FALSE; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){
        GLint n=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &n);
        std::vector<char> log(n); glGetShaderInfoLog(s, n, nullptr, log.data());
        std::cerr << "Shader compile error:\n" << log.data() << "\n";
    }
    return s;
}

Shader Shader::fromSource(ShaderType type, const char* src){
    GLenum glType = (type==ShaderType::Vertex)? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    Shader sh;
    sh.id_ = compile(glType, src);
    return sh;
}

Shader::~Shader(){
    if(id_) glDeleteShader(id_);
}

// MOVE ctor
Shader::Shader(Shader&& other) noexcept {
    id_ = other.id_;
    other.id_ = 0;
}

// MOVE op=
Shader& Shader::operator=(Shader&& other) noexcept {
    if(this != &other){
        if(id_) glDeleteShader(id_);
        id_ = other.id_;
        other.id_ = 0;
    }
    return *this;
}
