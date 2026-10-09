//
// Created by xpolas on 10/5/26.
//

#include "Model.h"

Model::Model(float *points, GLsizei vertexCount) {
    this->vertexCount = vertexCount;
    this->VBO = vertexCount * 6 * sizeof(float); //hardcoded for now
    //this->VAO =
}

void Model::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
