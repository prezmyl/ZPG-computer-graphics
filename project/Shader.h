//
// Created by xpolas on 10/10/26.
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
    GLuint get_id() const;
};


#endif //PROJECT_SHADER_H
