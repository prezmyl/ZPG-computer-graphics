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
    void use() const;
};


#endif //PROJECT_SHADERPROGRAM_H
