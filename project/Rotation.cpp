//
// Created by xpolas on 10/15/26.
//

#include "Rotation.h"
#include <glm/gtc/matrix_transform.hpp>

Rotation::Rotation(glm::vec3 axis, float angle) : axis(axis), angle(angle) {
}

glm::mat4 Rotation::getMatrix() const {
    return glm::rotate(glm::mat4(1.0f), angle, axis);
}

void Rotation::setAngle(float angle) {
    this->angle = angle;
}
