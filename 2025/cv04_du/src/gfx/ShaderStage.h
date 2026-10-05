#pragma once
#include <string>
#include <GL/glew.h>

class ShaderStage {
public:
    enum class Type { Vertex, Fragment };
    ShaderStage() = default;
    ShaderStage(Type t, const std::string& source);
    ~ShaderStage();

    ShaderStage(const ShaderStage&) = delete;
    ShaderStage& operator=(const ShaderStage&) = delete;

    ShaderStage(ShaderStage&& other) noexcept;
    ShaderStage& operator=(ShaderStage&& other) noexcept;

    bool compile(Type t, const std::string& source, std::string* errorLog = nullptr);
    GLuint id() const { return id_; }

private:
    GLuint id_ = 0;
};
