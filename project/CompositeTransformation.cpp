//
// Created by xpolas on 10/15/26.
//

#include "CompositeTransformation.h"


void CompositeTransformation::addTransformation(std::unique_ptr<Transformation> transformation) {
    transformations.push_back(std::move(transformation));
}

glm::mat4 CompositeTransformation::getMatrix() const {
    glm::mat4 result(1.0f);

    for (const auto& transformation : transformations) {
        result = transformation->getMatrix() * result;
    }
}
