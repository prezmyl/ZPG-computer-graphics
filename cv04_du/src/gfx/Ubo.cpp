#include "Ubo.h"

Ubo::Ubo(GLsizeiptr sizeBytes, GLuint bindingPoint) : binding_(bindingPoint), size_(sizeBytes) {
    glGenBuffers(1, &id_);
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
    glBufferData(GL_UNIFORM_BUFFER, size_, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_, id_);
}

Ubo::~Ubo(){
    if(id_) glDeleteBuffers(1, &id_);
}

Ubo::Ubo(Ubo&& o) noexcept {
    id_ = o.id_; o.id_ = 0;
    binding_ = o.binding_;
    size_ = o.size_;
}

Ubo& Ubo::operator=(Ubo&& o) noexcept {
    if(this != &o){
        if(id_) glDeleteBuffers(1, &id_);
        id_ = o.id_; o.id_ = 0;
        binding_ = o.binding_;
        size_ = o.size_;
    }
    return *this;
}

void Ubo::resize(GLsizeiptr bytes){
    size_ = bytes;
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
    glBufferData(GL_UNIFORM_BUFFER, size_, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_, id_);
}

void Ubo::update(const void* data, GLsizeiptr bytes, GLintptr offset){
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, bytes, data);
}

void Ubo::bindBase() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_, id_);
}
