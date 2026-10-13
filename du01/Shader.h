//
// Created by pol0511 on 10/5/26.
//

#ifndef PROJECT_SHADER_H
#define PROJECT_SHADER_H
#include "glad/gl.h"

class Shader {
private:
    GLuint id = 0;

    GLuint createShaderFromFile(GLenum shaderType, const char* shaderFile);

public:
    Shader(GLenum shaderType, const char *shaderPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;


    GLuint get_id() const;
};


#endif //PROJECT_SHADER_H
