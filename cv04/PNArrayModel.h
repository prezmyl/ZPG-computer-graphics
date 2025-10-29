#pragma once
#include <GL/glew.h>
#include "Model.h"

// Model pro data se stride 6: pos(3) + normal(3)
class PNArrayModel : public Model {
public:
    template<size_t N>
    explicit PNArrayModel(const float (&data)[N]) {
        const GLsizeiptr BYTES = sizeof(data);
        count_ = (GLsizei)N / 6;
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, BYTES, data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glBindVertexArray(0);
    }

    ~PNArrayModel() override {
        if(vbo_) glDeleteBuffers(1, &vbo_);
        if(vao_) glDeleteVertexArrays(1, &vao_);
    }

    void draw() const override {
        glBindVertexArray(vao_); glDrawArrays(GL_TRIANGLES, 0, count_); glBindVertexArray(0);
    }

private:
    GLuint vao_ = 0, vbo_ = 0;
    GLsizei count_ = 0;
};
