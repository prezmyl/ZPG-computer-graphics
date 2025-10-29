#include "models/MeshModel.h"

MeshModel::MeshModel(const float* pos, size_t posCount, const float* norm, size_t normCount,
                     const unsigned* idx, size_t idxCount)
{
    indexCount_ = idxCount;
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vboPos_);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*posCount, pos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    if (norm && normCount){
        glGenBuffers(1, &vboNorm_);
        glBindBuffer(GL_ARRAY_BUFFER, vboNorm_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normCount, norm, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*idxCount, idx, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

MeshModel::MeshModel(MeshModel&& o) noexcept {
    vao_=o.vao_; vboPos_=o.vboPos_; vboNorm_=o.vboNorm_; ebo_=o.ebo_;
    indexCount_=o.indexCount_;
    o.vao_=o.vboPos_=o.vboNorm_=o.ebo_=0; o.indexCount_=0;
}

MeshModel& MeshModel::operator=(MeshModel&& o) noexcept {
    if(this != &o){
        this->~MeshModel();
        vao_=o.vao_; vboPos_=o.vboPos_; vboNorm_=o.vboNorm_; ebo_=o.ebo_;
        indexCount_=o.indexCount_;
        o.vao_=o.vboPos_=o.vboNorm_=o.ebo_=0; o.indexCount_=0;
    }
    return *this;
}

MeshModel::~MeshModel(){
    if(ebo_)    glDeleteBuffers(1, &ebo_);
    if(vboNorm_)glDeleteBuffers(1, &vboNorm_);
    if(vboPos_) glDeleteBuffers(1, &vboPos_);
    if(vao_)    glDeleteVertexArrays(1, &vao_);
}

void MeshModel::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, (GLsizei)indexCount_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
