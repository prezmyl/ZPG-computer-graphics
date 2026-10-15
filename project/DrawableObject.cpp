//
// Created by xpolas on 10/15/26.
//

#include "DrawableObject.h"

#include <algorithm>
#include <ranges>

DrawableObject::DrawableObject(Model *model, ShaderProgram *shaderProgram) : model(model), shaderProgram(shaderProgram) {}


void DrawableObject::addTransformation(std::unique_ptr<Transformation> transformation) {
    transformations.push_back(std::move(transformation));

}

void DrawableObject::draw() const{
    glBindVertexArray(model->getVAO());
    glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
}
