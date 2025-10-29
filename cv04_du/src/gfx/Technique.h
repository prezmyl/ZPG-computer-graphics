#pragma once
#include <memory>
#include <string>
#include "gfx/ShaderProgram.h"

struct Technique {
    static constexpr GLuint CAMERA_BINDING   = 0;
    static constexpr GLuint MATERIAL_BINDING = 1;
    static constexpr GLuint LIGHT_BINDING    = 2;

    std::shared_ptr<ShaderProgram> program;

    static std::shared_ptr<Technique> createFromFiles(const std::string& vs, const std::string& fs, std::string* err=nullptr);

    void use() const { program->use(); }
    GLuint id() const { return program->id(); }
};
