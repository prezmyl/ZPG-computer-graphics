#pragma once
#include <GL/glew.h>
#include "core/Model.h"

class TriangleModel : public Model {
public:
    TriangleModel();
    ~TriangleModel() override;
    void draw() const override;

private:
    GLuint vao_ = 0, vbo_ = 0;
};
