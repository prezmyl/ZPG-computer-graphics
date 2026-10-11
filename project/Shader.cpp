//
// Created by xpolas on 10/10/26.
//

#include "Shader.h"

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <iterator>
#include <iostream>

GLuint Shader::createShaderFromFile(GLenum shaderType, const char *shaderFile) {
    //empty shader
    GLuint shaderID = glCreateShader(shaderType);

    if (shaderID == 0) {
        std::cout << "Unable to create shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    //load the content of preprepared shader file from its path
    std::ifstream file(shaderFile);

    if (!file.is_open()) {
        std::cout << "Unable to open file " << shaderFile << std::endl;

        glDeleteShader(shaderID);
        exit(EXIT_FAILURE);
    }

    std::string shaderCode(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    //source code for shader
    const char* source = shaderCode.c_str();

    glShaderSource(
        shaderID,
        1,
        &source,
        nullptr
    );

    //compilation
    glCompileShader(shaderID);

    //compilation check
    GLint success;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        char infoLog[1024];

        glGetShaderInfoLog(
            shaderID,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cout
            << "Shader failed:\n"
            << infoLog
            << std::endl;

        glDeleteShader(shaderID);
        exit(EXIT_FAILURE);
    }

    return shaderID;

}

Shader::Shader(GLenum shaderType, const char *shaderPath) : id(createShaderFromFile(shaderType, shaderPath)) {
}

GLuint Shader::get_id() const {
    return id;
}
