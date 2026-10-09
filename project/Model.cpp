//
// Created by xpolas on 10/5/26.
//

#include "Model.h"

Model::Model(const float *points, GLsizei vertexCount) {
    this->vertexCount = vertexCount;

    // =====================================================
    // VBO - Vertex Buffer Object
    // =====================================================
    glGenBuffers(
        1,
        &this->VBO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        this->VBO
    );

    glBufferData(
       GL_ARRAY_BUFFER,
       sizeof(points),
       points,
       GL_STATIC_DRAW
   );

    // =====================================================
    // VAO - Vertex Array Object
    // =====================================================
    glGenVertexArrays(
        1,
        &this->VAO
    );

    glBindVertexArray(this->VAO);


    // atribut 0 = position
    glEnableVertexAttribArray(0);

    // atribut 1 = color
    glEnableVertexAttribArray(1);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        this->VBO
    );

    // Position:
    //
    // x y z
    //
    glVertexAttribPointer(
        0,                  // atribute index
        3,                  // x, y, z
        GL_FLOAT,           // data type
        GL_FALSE,           // normalization
        6 * sizeof(float),  // size of One vertex
        (GLvoid*) 0         // offset
    );


    // Color:
    //
    // r g b
    //
    glVertexAttribPointer(
        1,                         // atribute index
        3,                         // r, g, b
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (GLvoid*) (3 * sizeof(float))
    );
}


void Model::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
