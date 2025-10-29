#include "ShaderStage.h"
#include <vector>

static GLenum toGl(ShaderStage::Type t){
    return (t == ShaderStage::Type::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
}

ShaderStage::ShaderStage(Type t, const std::string& source) {
    compile(t, source, nullptr);
}

ShaderStage::~ShaderStage(){
    if(id_) glDeleteShader(id_);
}

ShaderStage::ShaderStage(ShaderStage&& o) noexcept {
    id_ = o.id_; o.id_ = 0;
}

ShaderStage& ShaderStage::operator=(ShaderStage&& o) noexcept {
    if(this != &o){
        if(id_) glDeleteShader(id_);
        id_ = o.id_; o.id_ = 0;
    }
    return *this;
}

bool ShaderStage::compile(Type t, const std::string& source, std::string* err){
    if(id_) { glDeleteShader(id_); id_ = 0; }
    id_ = glCreateShader(toGl(t));
    const char* src = source.c_str();
    glShaderSource(id_, 1, &src, nullptr);
    glCompileShader(id_);
    GLint ok = GL_FALSE;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &ok);
    if(!ok){
        if(err){
            GLint len=0; glGetShaderiv(id_, GL_INFO_LOG_LENGTH,&len);
            std::vector<char> log(len);
            glGetShaderInfoLog(id_, len, nullptr, log.data());
            *err = std::string(log.begin(), log.end());
        }
        glDeleteShader(id_);
        id_ = 0;
        return false;
    }
    return true;
}
