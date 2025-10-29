#pragma once
#include <GL/glew.h>
#include <cstddef>

class Ubo {
public:
    Ubo() = default;
    Ubo(GLsizeiptr sizeBytes, GLuint bindingPoint);
    ~Ubo();

    Ubo(const Ubo&) = delete;
    Ubo& operator=(const Ubo&) = delete;

    Ubo(Ubo&&) noexcept;
    Ubo& operator=(Ubo&&) noexcept;

    void resize(GLsizeiptr sizeBytes);
    void update(const void* data, GLsizeiptr bytes, GLintptr offset = 0);
    void bindBase() const;

    GLuint id() const { return id_; }
    GLuint binding() const { return binding_; }
    GLsizeiptr size() const { return size_; }

private:
    GLuint id_ = 0;
    GLuint binding_ = 0;
    GLsizeiptr size_ = 0;
};
