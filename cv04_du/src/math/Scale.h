#pragma once
#include "AbstractTransformation.h"
#include <glm/vec3.hpp>

class Scale final : public AbstractTransformation {
public:
    explicit Scale(glm::vec3 s) : s_(s) {}
    glm::mat4 matrix() const override;
    glm::vec3 value() const { return s_; }
    void setValue(glm::vec3 s) { s_ = s; }
private:
    glm::vec3 s_;
};
