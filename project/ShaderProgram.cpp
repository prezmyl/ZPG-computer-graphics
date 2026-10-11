//
// Created by xpolas on 10/5/26.
//

#include "ShaderProgram.h"

#include <iostream>
#include <GLFW/glfw3.h>

ShaderProgram::ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader) {
    this->id = glCreateProgram();

    //attach shaders to Program
    glAttachShader(this->id, vertexShader.get_id());
    glAttachShader(this->id, fragmentShader.get_id());

    //link Program
    glLinkProgram(this->id);


    //link check
    GLint linkSuccess;
    glGetProgramiv(this->id, GL_LINK_STATUS,&linkSuccess);

    if (linkSuccess == GL_FALSE) {
        char infoLog[1024];

        glGetProgramInfoLog(this->id, sizeof(infoLog), nullptr, infoLog);

        std::cout
            << "Shader program linking failed:\n"
            << infoLog
            << std::endl;

        exit(EXIT_FAILURE);
    }

}

void ShaderProgram::use() {
    glUseProgram(id);
}
