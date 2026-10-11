//
// Created by xpolas on 10/5/26.
//

#ifndef PROJECT_SHADERPROGRAM_H
#define PROJECT_SHADERPROGRAM_H


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
};


#endif //PROJECT_SHADERPROGRAM_H
