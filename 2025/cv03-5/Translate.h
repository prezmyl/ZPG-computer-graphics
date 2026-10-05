#pragma once
#include "AbstractTransformation.h"
#include <glm/vec3.hpp>

class Translate final : public AbstractTransformation {
public:
    explicit Translate(glm::vec3 delta) : d_(delta) {}
    glm::mat4 matrix() const override;
    glm::vec3 delta() const { return d_; }
    void setDelta(glm::vec3 d) { d_ = d; }
private:
    glm::vec3 d_;
};
