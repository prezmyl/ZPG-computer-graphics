#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class MeshModel {
public:
    // Vytvoření z paměťových polí (pos/norm/idx)
    MeshModel(const float* pos, size_t posCount, const float* norm, size_t normCount,
              const unsigned* idx, size_t idxCount);

    // Zakázat kopii, povolit přesun
    MeshModel(const MeshModel&) = delete;
    MeshModel& operator=(const MeshModel&) = delete;
    MeshModel(MeshModel&&) noexcept;
    MeshModel& operator=(MeshModel&&) noexcept;

    ~MeshModel();

    void draw() const;
    GLuint vao() const { return vao_; }
    size_t indexCount() const { return indexCount_; }

private:
    GLuint vao_=0, vboPos_=0, vboNorm_=0, ebo_=0;
    size_t indexCount_=0;
};
