#pragma once
#include <memory>
#include <glm/glm.hpp>

class Model;
class ShaderProgram;
class TransformChain;

class DrawableObject {
public:
// tohle agreguje instance trid Model, ShaderProgram, TransformChain
    Model* model = nullptr;                          // nevlastni
    ShaderProgram* program = nullptr;                // nevlastni
    std::shared_ptr<TransformChain> transform;       // sdilene pres clever pointry

    void draw(const glm::mat4& V, const glm::mat4& P) const;
};
