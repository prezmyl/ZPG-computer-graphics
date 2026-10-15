//
// Created by pol0511 on 10/5/26.
//

#ifndef PROJECT_SHADERPROGRAM_H
#define PROJECT_SHADERPROGRAM_H


#include <glm/ext/matrix_float4x4.hpp>

#include "Shader.h"
#include "glad/gl.h"


class ShaderProgram {
private:
    GLuint id = 0;

public:
    ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
    ~ShaderProgram();

    //ban the flat copy -> otherwise not 2 program, but 2 c++ object would exist that would own the same resources
    //if it would context appropriate we would have to create rela deep copy of GPU resources or transfare owenrship
    //for now via:
    //deleted copy constructor
    ShaderProgram(const ShaderProgram&) = delete;
    //deleted copy assignment operator
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;
    void setMat4(const char* name, const glm::mat4& matrix) const; // transfer matrix to shader
};


#endif //PROJECT_SHADERPROGRAM_H
