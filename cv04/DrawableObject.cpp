#include "DrawableObject.h"

void DrawableObject::draw(const glm::mat4& V, const glm::mat4& P) const {
    if(!model || !program || !transform) return;
    program->use();
    program->setUniform("uModel", transform->modelMatrix());
    program->setUniform("uView",  V);
    program->setUniform("uProj",  P);
    model->draw();
}
