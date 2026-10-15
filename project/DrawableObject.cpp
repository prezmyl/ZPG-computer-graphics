//
// Created by xpolas on 10/15/26.
//

#include "DrawableObject.h"

#include <algorithm>
#include <ranges>

DrawableObject::DrawableObject(Model *model, ShaderProgram *shaderProgram, std::unique_ptr<Transformation> transformation) : model(model), shaderProgram(shaderProgram), transformation(std::move(transformation)) {}

void DrawableObject::draw() const {
    shaderProgram->use();

    // transfer matrix to GPU using uniforms
    //Translation move(glm::vec3(0.25f, 0.0f, 0.0f));
    glm::mat4 matrix = transformation->getMatrix();
    shaderProgram->setMat4("modelMatrix", matrix);

    model->draw();
}
