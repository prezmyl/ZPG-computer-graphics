// Rotate.h
#pragma once
#include "AbstractTransformation.h"
#include <glm/vec3.hpp>

class Rotate final : public AbstractTransformation {
public:
    Rotate(float degrees, glm::vec3 axis) : deg_(degrees), axis_(axis) {}
    glm::mat4 matrix() const override;

    float degrees() const { return deg_; }
    void  setDegrees(float d) { deg_ = d; }
    glm::vec3 axis() const { return axis_; }
    void setAxis(glm::vec3 a) { axis_ = a; }

private:
    float deg_;
    glm::vec3 axis_;
};
