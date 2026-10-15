//
// Created by xpolas on 10/15/26.
//

#include "Scale.h"
#include <glm/gtc/matrix_transform.hpp>

Scale::Scale(glm::vec3 factor) : factor(factor) {
}

glm::mat4 Scale::getMatrix() const {
    return glm::scale(glm::mat4(1.0f), factor);
}
