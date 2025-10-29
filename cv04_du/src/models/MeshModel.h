#pragma once
#include <cstddef>
#include <GL/glew.h>

class MeshModel {
public:
    // interleaved: [px,py,pz, nx,ny,nz] * vertexCount
    MeshModel(const float* interleaved, size_t vertexCount, bool hasNormals=true, int strideFloats=6);
    MeshModel(const MeshModel&) = delete;
    MeshModel& operator=(const MeshModel&) = delete;
    MeshModel(MeshModel&&) noexcept;
    MeshModel& operator=(MeshModel&&) noexcept;
    ~MeshModel();
    void draw() const;
private:
    GLuint vao_=0, vbo_=0;
    GLsizei vertexCount_=0;
    GLsizei strideBytes_=0;
    bool hasNormals_=true;
};
