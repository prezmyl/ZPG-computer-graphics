#pragma once
#include <GL/glew.h>

enum class ShaderType { Vertex, Fragment };

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    static Shader fromSource(ShaderType type, const char* src);

    GLuint id() const { return id_; }
private:
    GLuint id_ = 0;
};
