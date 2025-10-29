#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Model.h"
#include "ShaderProgram.h"
#include "TransformChain.h"

class DrawableObject {
public:
    Model* model = nullptr;                       // agregace
    ShaderProgram* program = nullptr;             // agregace
    std::shared_ptr<TransformChain> transform;    // agregace

    void draw(const glm::mat4& V, const glm::mat4& P) const;
};
