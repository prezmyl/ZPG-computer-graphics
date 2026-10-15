//
// Created by pol0511 on 10/14/26.
//

#include "Translation.h"

Translation::Translation(glm::vec3 translation) : translation(translation) {
}

glm::mat4 Translation::getMatrix() const {

    return glm::translate(glm::mat4(1.0f), translation);
}
