//
// Created by pol0511 on 10/5/26.
//

#ifndef PROJECT_MODEL_H
#define PROJECT_MODEL_H
#include "glad/gl.h"


class Model {
private:
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLsizei vertexCount = 0;

public:
    Model(const float *points, GLsizei vertexCount);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw() const; 
};


#endif //PROJECT_MODEL_H
