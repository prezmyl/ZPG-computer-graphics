#include "models/MeshModel.h"
#include <GL/glew.h>

MeshModel::MeshModel(const float* data, size_t vertexCount, bool hasNormals, int strideFloats)
: hasNormals_(hasNormals), vertexCount_((GLsizei)vertexCount), strideBytes_((GLsizei)strideFloats*sizeof(float)) {
    glGenVertexArrays(1, &vao_); glBindVertexArray(vao_);
    glGenBuffers(1, &vbo_); glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertexCount * (size_t)strideBytes_), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideBytes_, (void*)0);

    if (hasNormals_) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes_, (void*)(3*sizeof(float)));
    }
    glBindVertexArray(0);
}

MeshModel::MeshModel(MeshModel&& o) noexcept {
    vao_=o.vao_; vbo_=o.vbo_; vertexCount_=o.vertexCount_; strideBytes_=o.strideBytes_; hasNormals_=o.hasNormals_;
    o.vao_=o.vbo_=0; o.vertexCount_=0; o.strideBytes_=0;
}
MeshModel& MeshModel::operator=(MeshModel&& o) noexcept {
    if (this!=&o){ this->~MeshModel(); vao_=o.vao_; vbo_=o.vbo_; vertexCount_=o.vertexCount_; strideBytes_=o.strideBytes_; hasNormals_=o.hasNormals_;
        o.vao_=o.vbo_=0; o.vertexCount_=0; o.strideBytes_=0; }
    return *this;
}
MeshModel::~MeshModel(){ if(vbo_) glDeleteBuffers(1,&vbo_); if(vao_) glDeleteVertexArrays(1,&vao_); }
void MeshModel::draw() const { glBindVertexArray(vao_); glDrawArrays(GL_TRIANGLES, 0, vertexCount_); glBindVertexArray(0); }
