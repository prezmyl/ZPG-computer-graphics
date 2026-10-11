//
// Created by xpolas on 10/5/26.
//

#include "ShaderProgram.h"

#include <iostream>
#include <cstdlib>

ShaderProgram::ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader) {
    id = glCreateProgram();
    if (id == 0) {
        std::cout << "Shader program creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    //attach shaders to Program
    glAttachShader(id, vertexShader.get_id());
    glAttachShader(id, fragmentShader.get_id());

    //link Program
    glLinkProgram(id);


    //link check
    GLint linkSuccess;
    glGetProgramiv(id, GL_LINK_STATUS,&linkSuccess);

    if (linkSuccess == GL_FALSE) {
        char infoLog[1024];

        glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);

        std::cout
            << "Shader program linking failed:\n"
            << infoLog
            << std::endl;
        glDeleteProgram(id);
        exit(EXIT_FAILURE);
    }

}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::use() const{
    glUseProgram(id);
}
